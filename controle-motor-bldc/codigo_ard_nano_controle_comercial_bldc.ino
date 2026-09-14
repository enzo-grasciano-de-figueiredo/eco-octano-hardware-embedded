// =======================================================
// CONTROLE DE MOTOR BLDC COM LIMITADOR DE VELOCIDADE
// E PERFIS: COM CARGA / SEM CARGA
// =======================================================


// -----CALIBRAÇÕES--------------------------------------------------------------------------------------------

// ==============================
// ===== LIMITE DE VELOCIDADE ===
// ==============================
// Velocidade limite desejada
float targetSpeed_kmh = 25.0;

// ==============================
// ===== CONFIGURAÇÃO MODOS =====
// ==============================
// false = SEM CARGA (roda no ar / bancada)
// true  = COM CARGA (rua / dinamômetro)
bool LOAD_MODE = false;

// ==============================
// ===== CALIBRAÇÃO VELOCIDADE ==
// ==============================
// Velocidade máxima medida REAL do sistema
float maxSpeedNoLoad_kmh = 82.0; // medido sem carga
float maxSpeedLoad_kmh   = 25.0; // AJUSTE após teste real <--------------COLOCAR AQUI VELOCIDADE MÁXIMA NA RUA

// ==============================
// ===== CONFIGURAÇÃO RODA ======
// ==============================
const float wheelRadius = 0.24; // metros (24 cm)
const float wheelCircumference = 2 * 3.1416 * wheelRadius;

// ------------------------------------------------------------------------------------------------------------

// ==============================
// ====== PINAGEM HARDWARE ======
// ==============================
const int throttlePin = 11;   // Saída PWM para o controlador (via filtro RC)
const int button1 = 2;        // Botão 1 → rampa de 5 segundos
const int button2 = 3;        // Botão 2 → rampa de 10 segundos

// ==============================
// ===== CALIBRAÇÃO PWM =========
// ==============================
// Valores de PWM que correspondem ao throttle do controlador
// IMPORTANTE: ajustar conforme seu controlador real
const int pwmMin = 56;   // ~1.1V (mínimo - motor parado)
const int pwmMax = 214;  // ~4.2V (máximo físico)

// ==============================
// ===== VARIÁVEIS INTERNAS =====
// ==============================
unsigned long startTime = 0;
bool rampActive = false;
int rampDuration = 0;

int pwmMaxLimited; // PWM máximo após limitação

// ==============================
// ========= SETUP ==============
// ==============================
void setup() {
  pinMode(throttlePin, OUTPUT);
  pinMode(button1, INPUT_PULLUP);
  pinMode(button2, INPUT_PULLUP);

  // Calcula o limite de PWM baseado no modo selecionado
  calculatePWMLimit();

  // Garante que o sistema inicia sem aceleração
  analogWrite(throttlePin, pwmMin);
}

// ==============================
// =========== LOOP =============
// ==============================
void loop() {

  // Leitura dos botões (ativo em LOW)
  bool button1Pressed = digitalRead(button1) == LOW;
  bool button2Pressed = digitalRead(button2) == LOW;

  // Seleciona tipo de rampa
  if (button1Pressed) {
    startRamp(5000); // 5 segundos
  } 
  else if (button2Pressed) {
    startRamp(10000); // 10 segundos
  }

  // Se nenhum botão estiver pressionado → corta aceleração
  if (!button1Pressed && !button2Pressed) {
    rampActive = false;
    analogWrite(throttlePin, pwmMin);
    return;
  }

  // Executa a rampa de aceleração
  if (rampActive) {

    unsigned long elapsed = millis() - startTime;

    // Progresso da rampa (0.0 até 1.0)
    float progress = (float)elapsed / rampDuration;
    if (progress > 1.0) progress = 1.0;

    // Interpolação linear entre mínimo e máximo limitado
    int pwmValue = pwmMin + (pwmMaxLimited - pwmMin) * progress;

    analogWrite(throttlePin, pwmValue);
  }
}

// ==============================
// ===== INICIA NOVA RAMPA ======
// ==============================
void startRamp(int duration) {

  // Evita reiniciar a rampa continuamente enquanto segura o botão
  if (!rampActive) {
    rampActive = true;
    rampDuration = duration;
    startTime = millis();
  }
}

// ==============================
// == CÁLCULO DO LIMITE PWM =====
// ==============================
void calculatePWMLimit() {

  float systemMaxSpeed;

  // Seleciona qual calibração usar
  if (LOAD_MODE) {
    systemMaxSpeed = maxSpeedLoad_kmh;
  } else {
    systemMaxSpeed = maxSpeedNoLoad_kmh;
  }

  // Calcula fator proporcional (0.0 a 1.0)
  float factor = targetSpeed_kmh / systemMaxSpeed;

  // Limita segurança
  if (factor > 1.0) factor = 1.0;
  if (factor < 0.0) factor = 0.0;

  // Converte fator em PWM real
  pwmMaxLimited = pwmMin + (pwmMax - pwmMin) * factor;
}