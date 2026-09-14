const int throttlePin = 11; // PWM saída
const int btn5s = 2;
const int btn10s = 3;

// Valores de PWM correspondentes ao throttle
// Ajuste fino pode ser necessário!
const int pwmMin = 56;   // ~1.1V
const int pwmMax = 214;  // ~4.2V

unsigned long startTime = 0;
bool rampActive = false;
int rampDuration = 0; // em ms

void setup() {
  pinMode(throttlePin, OUTPUT);
  pinMode(btn5s, INPUT_PULLUP);
  pinMode(btn10s, INPUT_PULLUP);

  analogWrite(throttlePin, pwmMin); // começa em idle
}

void loop() {
  bool btn5Pressed = digitalRead(btn5s) == LOW;
  bool btn10Pressed = digitalRead(btn10s) == LOW;

  // Detecta qual botão foi pressionado
  if (btn5Pressed) {
    startRamp(5000);
  } else if (btn10Pressed) {
    startRamp(10000);
  }

  // Se nenhum botão pressionado → corta aceleração
  if (!btn5Pressed && !btn10Pressed) {
    rampActive = false;
    analogWrite(throttlePin, pwmMin);
    return;
  }

  // Executa a rampa
  if (rampActive) {
    unsigned long elapsed = millis() - startTime;

    float progress = (float)elapsed / rampDuration;
    if (progress > 1.0) progress = 1.0;

    int pwmValue = pwmMin + (pwmMax - pwmMin) * progress;
    analogWrite(throttlePin, pwmValue);
  }
}

void startRamp(int duration) {
  // Só reinicia se ainda não estiver ativo
  if (!rampActive) {
    rampActive = true;
    rampDuration = duration;
    startTime = millis();
  }
}