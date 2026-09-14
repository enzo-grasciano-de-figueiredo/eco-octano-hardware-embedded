#include <Arduino.h>
#include <WiFi.h>
#include <Firebase_ESP_Client.h>

// Provide the token generation process info.
#include "addons/TokenHelper.h"
// Provide the RTDB payload printing info and other helper functions.
#include "addons/RTDBHelper.h"

#include "EmonLib.h"

// ==========================================
// CONFIGURAÇÕES DO HARDWARE E REDE
// ==========================================
#define WIFI_SSID "SEU_WIFI"
#define WIFI_PASSWORD "SUA_SENHA_WIFI"

#define API_KEY "SUA_API_KEY_DO_FIREBASE"
#define DATABASE_URL "SUA_URL_DO_DATABASE.firebaseio.com" // Sem https://

// Se for autenticar por usuário:
#define USER_EMAIL "SEU_EMAIL_CRIADO_NO_FIREBASE"
#define USER_PASSWORD "SUA_SENHA_CRIADA_NO_FIREBASE"

const int PINO_SCT = 34; // GPIO 34 do ESP32
const float TENSAO_REDE_V = 127.0; 
const float CALIBRACAO_SCT = 60.6; // Valor do Shunt

// ==========================================
// OBJETOS GLOBAIS
// ==========================================
EnergyMonitor emon1;

FirebaseData fbdo;
FirebaseAuth auth;
FirebaseConfig config;

// ==========================================
// ESTRUTURA PARA COMPARTILHAR DADOS ENTRE CORES
// ==========================================
struct SensorData {
  double correnteRMS;
  double potenciaWatts;
  double energiaJoules;
};

SensorData currentData = {0.0, 0.0, 0.0};
SemaphoreHandle_t dataMutex;

// ==========================================
// TAREFAS DOS NÚCLEOS
// ==========================================
TaskHandle_t TaskMeasureHandle;
TaskHandle_t TaskFirebaseHandle;

// Função executada no Core 1 (Medição)
void TaskMeasure(void *pvParameters) {
  unsigned long tempoUltimaLeitura = millis();
  double energiaAcumuladaJoules = 0.0;

  Serial.print("Core da Medição inicializado no Core: ");
  Serial.println(xPortGetCoreID());

  for(;;) {
    // 1. O método calcIrms tira uma média quadrática amostrando profundamente o pino AC.
    double correnteRMS = emon1.calcIrms(1480);
    
    // Filtro ruído
    if (correnteRMS < 0.05) {
      correnteRMS = 0.0;
    }

    // 2. Calcula Potência
    double potenciaWatts = correnteRMS * TENSAO_REDE_V;

    // 3. Integração no tempo
    unsigned long tempoAtual = millis();
    float deltaTempoS = (tempoAtual - tempoUltimaLeitura) / 1000.0;
    tempoUltimaLeitura = tempoAtual;

    // 4. Acumula energia
    double energiaParcialJ = potenciaWatts * deltaTempoS;
    energiaAcumuladaJoules += energiaParcialJ;

    // Atualiza a estrutura compartilhada protegida por MUTEX
    if (xSemaphoreTake(dataMutex, portMAX_DELAY) == pdTRUE) {
      currentData.correnteRMS = correnteRMS;
      currentData.potenciaWatts = potenciaWatts;
      currentData.energiaJoules = energiaAcumuladaJoules;
      xSemaphoreGive(dataMutex);
    }

    // Delay da medição
    vTaskDelay(1000 / portTICK_PERIOD_MS);
  }
}

// Função executada no Core 0 (Firebase)
void TaskFirebase(void *pvParameters) {
  Serial.print("Core do Firebase inicializado no Core: ");
  Serial.println(xPortGetCoreID());
  
  // 1. Conecta ao WiFi
  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
  Serial.print("Conectando ao WiFi");
  while (WiFi.status() != WL_CONNECTED) {
    Serial.print(".");
    vTaskDelay(500 / portTICK_PERIOD_MS);
  }
  Serial.println();
  Serial.println("WiFi Conectado!");

  // 2. Configura Firebase
  config.api_key = API_KEY;
  config.database_url = DATABASE_URL;
  auth.user.email = USER_EMAIL;
  auth.user.password = USER_PASSWORD;

  // Callback de status do Token (necessário para o Mobizt ESP Client)
  config.token_status_callback = tokenStatusCallback; 

  Firebase.begin(&config, &auth);
  Firebase.reconnectWiFi(true);

  for(;;) {
    if (Firebase.ready()) {
      SensorData localData;

      // Lê os dados sincronizando pelo MUTEX
      if (xSemaphoreTake(dataMutex, portMAX_DELAY) == pdTRUE) {
        localData = currentData;
        xSemaphoreGive(dataMutex);
      }

      // Prepara objeto em formato JSON
      String path = "/sensor";
      
      FirebaseJson json;
      json.set("corrente_A", localData.correnteRMS);
      json.set("potencia_W", localData.potenciaWatts);
      json.set("energia_J", localData.energiaJoules);
      json.set("timestamp", millis()); // opcional: só para registrar um temporal do uC

      Serial.println("-----------------------------------");
      Serial.printf("Enviando -> Corrente: %.3f A | Potencia: %.2f W | Energia: %.2f J\n", 
                    localData.correnteRMS, localData.potenciaWatts, localData.energiaJoules);
                    
      // Envia os dados para a raiz (/sensor) no database
      if (Firebase.RTDB.setJSON(&fbdo, path.c_str(), &json)) {
        Serial.println("SUCESSO: Dados enviados para o Firebase!");
      } else {
        Serial.println("ERRO: O envio falhou: " + fbdo.errorReason());
      }
    }

    // Aguarda 5 segundos antes de enviar no banco de novo (evita spam de dados)
    vTaskDelay(5000 / portTICK_PERIOD_MS); 
  }
}

void setup() {
  Serial.begin(115200);
  while (!Serial) { ; } 

  Serial.println("=========================================");
  Serial.println("Iniciando Joulimetro - Dual Core (FreeRTOS)");
  Serial.println("=========================================");

  emon1.current(PINO_SCT, CALIBRACAO_SCT);

  // Instancia/Gera o Mutex para impedir colisão de leitura/escrita na RAM
  dataMutex = xSemaphoreCreateMutex();
  if (dataMutex == NULL) {
    Serial.println("Erro crítico: Falha ao criar o Mutex");
    while(1); 
  }

  // Cria a tarefa Firebase no Core 0 (Processamento de Rede padrão do ESP32)
  xTaskCreatePinnedToCore(
    TaskFirebase,           // Função
    "TaskFirebase",         // Nome abstrato
    10240,                  // Pilha na Memória - 10kb (Seguro p/ SSL/Firebase)
    NULL,                   // Argumentos extras
    1,                      // Prioridade média
    &TaskFirebaseHandle,    // Handle 
    0);                     // Core 0

  // Cria a tarefa de Medição no Core 1 (Aplicação Principal)
  xTaskCreatePinnedToCore(
    TaskMeasure,            
    "TaskMeasure",          
    4096,                   // Pilha na Memória - 4kb (SoS)                
    NULL,                   
    2,                      // Prioridade mais alta, queremos leitura exata
    &TaskMeasureHandle,     
    1);                     // Core 1
}

void loop() {
  // Como usamos tasks do freeRTOS vinculadas aos cores fixos
  // deletamos o loop padrão para liberar o uso da task principal da aplicação base
  vTaskDelete(NULL);
}
