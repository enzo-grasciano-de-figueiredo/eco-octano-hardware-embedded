#include <Arduino.h>

int sensor_V = A0;
float corrente = 0.0;

// Tarefa para ler o sensor
void readSensorTask(void *parameter) {
  float portA0 = 0.0; 
  float loop = 1000.0;

  while (true) {
    portA0 = 0.0; // Resetar a leitura a cada iteração

    for (int i = 0; i < loop; i++) {
      portA0 += analogRead(sensor_V);
    }

    portA0 = portA0 / loop;
    corrente = (portA0 * 0.0253) + 2.71; //ajuste equação  

    vTaskDelay(1000 / portTICK_PERIOD_MS); // Delay de 100 milissegundos
  }
}

// Tarefa para enviar os dados via Serial
void sendDataTask(void *parameter) {
  while (true) {
    
    //Serial.print("PortA0: ");  
    Serial.println(corrente, 2);
    //Serial.println(" A");
    
    vTaskDelay(1000 / portTICK_PERIOD_MS); // Delay de 100 milissegundos
  }
}

void setup() {
  pinMode(sensor_V, INPUT);
  Serial.begin(115200);

  // Configurar a resolução para 12 bits (0-4096)
  analogReadResolution(12);

  // Criar tarefas
  xTaskCreate(readSensorTask, "ReadSensor", 2048, NULL, 1, NULL);
  xTaskCreate(sendDataTask, "SendData", 2048, NULL, 1, NULL);
}

void loop() {
  // O loop principal pode estar vazio, pois as tarefas estão gerenciando a execução
}
