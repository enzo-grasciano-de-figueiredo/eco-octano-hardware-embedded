// =======================================================
// ========= PINAGEM =====================================
// =======================================================
const int button1Pin = 2;
const int button2Pin = 3;
const int outputPin  = 11;

// Chaves de seleção de potência
const int switch80Pin = 7; // 80%
const int switch60Pin = 9; // 60%

// =======================================================
// ========= PARÂMETROS AJUSTÁVEIS ========================
// =======================================================

// Tensão inicial (NÃO MEXER)
float Vinicio = 0.9;

// Perfis de velocidade
float Vfim1 = 5.0;
float Tempo1 = 10000;

float Vfim2 = 5.0;
float Tempo2 = 10000;

// Amortecimento (cruzeiro)
float Vamortecida = 0.97;
float TempoVamortecida = 1000;

// =======================================================
// ========= VARIÁVEIS INTERNAS ===========================
// =======================================================
float currentVoltage = 0.0;

// =======================================================
// ========= SETUP ========================================
// =======================================================
void setup() {
  Serial.begin(115200);

  pinMode(button1Pin, INPUT_PULLUP);
  pinMode(button2Pin, INPUT_PULLUP);

  pinMode(switch80Pin, INPUT_PULLUP);
  pinMode(switch60Pin, INPUT_PULLUP);

  pinMode(outputPin, OUTPUT);

  analogWrite(outputPin, 0);
}

// =======================================================
// ========= LOOP PRINCIPAL ===============================
// =======================================================
void loop() {

  float fatorPotencia = getPowerFactor();

  // Botão 1
  if (digitalRead(button1Pin) == LOW) {
    rampVoltage(Vinicio, Vfim1, Tempo1);
  }

  // Botão 2
  if (digitalRead(button2Pin) == LOW) {
    rampVoltage(Vinicio, Vfim2 * fatorPotencia, Tempo2);
  }

  // Nenhum botão pressionado
  if (digitalRead(button1Pin) == HIGH && digitalRead(button2Pin) == HIGH) {
    analogWrite(outputPin, 0);
    currentVoltage = 0.0;
    Serial.println(currentVoltage);
    delay(10);
    return;
  }
}

// =======================================================
// ========= LEITURA DAS CHAVES ===========================
// =======================================================
// Retorna fator de potência baseado nas chaves
float getPowerFactor() {

  bool s80 = digitalRead(switch80Pin) == LOW;
  bool s60 = digitalRead(switch60Pin) == LOW;

  // Ambas ligadas → 40%
  if (s80 && s60) {
    return 0.3;
  }

  // Apenas D7 → 80%
  if (s80) {
    return 0.7;
  }

  // Apenas D9 → 60%
  if (s60) {
    return 0.5;
  }

  // Nenhuma → 100%
  return 1.0;
}

// =======================================================
// ========= RAMPA DE TENSÃO ==============================
// =======================================================
void rampVoltage(float startVoltage, float endVoltage, unsigned long duration) {

  unsigned long startTime = millis();
  float voltageRange = endVoltage - startVoltage;

  float finalVoltage = endVoltage * Vamortecida;

  if (currentVoltage < endVoltage) {

    while (millis() - startTime < duration) {

      // Se soltou botão → corta tudo
      if (digitalRead(button1Pin) == HIGH && digitalRead(button2Pin) == HIGH) {
        analogWrite(outputPin, 0);
        currentVoltage = 0.0;
        return;
      }

      unsigned long elapsedTime = millis() - startTime;
      float normalizedTime = (float)elapsedTime / (float)duration;

      // Curva logarítmica
      currentVoltage = startVoltage +
        voltageRange * log(1 + 9 * normalizedTime) / log(10);

      int outputValue = (int)(currentVoltage / 5.0 * 255);
      analogWrite(outputPin, outputValue);

      Serial.println(currentVoltage);

      delay(10);
    }

    // Garante valor final
    analogWrite(outputPin, (int)(endVoltage / 5.0 * 255));
    currentVoltage = endVoltage;

    delay(TempoVamortecida);

    // Entra em modo cruzeiro
    analogWrite(outputPin, (int)(finalVoltage / 5.0 * 255));
    currentVoltage = finalVoltage;

    // Mantém enquanto botão pressionado
    while (digitalRead(button1Pin) == LOW || digitalRead(button2Pin) == LOW) {
      analogWrite(outputPin, (int)(finalVoltage / 5.0 * 255));
      Serial.println(finalVoltage);
      delay(10);
    }
  }

  Serial.println(finalVoltage);
}