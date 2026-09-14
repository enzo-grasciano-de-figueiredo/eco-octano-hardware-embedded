# ⚡ Eco Octano Hardware, Powertrain & Embedded Telemetry

[![Team](https://img.shields.io/badge/Equipe-Eco%20Octano%20UFPR-red.svg)](https://ufpr.br/)
[![Platform](https://img.shields.io/badge/Embedded-ESP32%20%7C%20Arduino%20Nano-blue.svg)](https://espressif.com/)
[![Cloud](https://img.shields.io/badge/IoT-Google%20Firebase%20RTDB-FFCA28.svg)](https://firebase.google.com/)
[![Hardware](https://img.shields.io/badge/Sensors-SCT--013%20%7C%20EmonLib-green.svg)]()

> **Módulos de eletrônica de potência, controle de motores elétricos e telemetria energética** desenvolvidos para veículos elétricos e projetos de eficiência energética na **Universidade Federal do Paraná (UFPR)** — Equipe Eco Octano.

---

## 🏎️ Visão Geral dos Módulos

Este repositório reúne três desenvolvimentos centrais de engenharia elétrica e eletrônica embarcada:

```
┌────────────────────────────────┐    PWM    ┌────────────────────────────────┐
│ Módulo de Controle BLDC        ├──────────►│ Controlador Comercial / Inversor│
│ (Arduino Nano / Algoritmo EGF) │           │ Motor Brushless DC             │
└────────────────────────────────┘           └────────────────────────────────┘

┌────────────────────────────────┐  I_rms/P  ┌────────────────────────────────┐
│ Joulímetro & Telemetria Nuvem  ├──────────►│ Google Firebase Realtime DB    │
│ (ESP32 + EmonLib + SCT-013)    │  (Joules) │ Dashboard em Tempo Real        │
└────────────────────────────────┘           └────────────────────────────────┘

┌────────────────────────────────┐   Layout  ┌────────────────────────────────┐
│ Placas Hardware Trainee        ├──────────►│ Fabricação PCB (Fresadora CNC) │
│ (Módulo de Medição & Suportes) │           │ Impressão 3D de Suportes STL   │
└────────────────────────────────┘           └────────────────────────────────┘
```

---

## 🔌 Detalhamento dos Projetos

### 1. Controle Eletrônico de Motor BLDC (`controle-motor-bldc/`)
- **Firmware**: Implementado para Arduino Nano / microcontroladores AVR.
- **Modulação PWM Suave**: Geração de sinais de controle para acionamento de controladores comerciais de motores sem escovas (BLDC).
- **Tratamento de Rampa & Limitador de Aceleração**: Proteção contra picos de corrente na partida e limitação de potência configurável para otimização de consumo de bateria em provas de eficiência energética.

### 2. Joulímetro Digital & Telemetria em Nuvem (`joulimetro-monitor-energia-esp32/`)
- **Microcontrolador**: ESP32 DevKit v1 operando com PlatformIO.
- **Sensor de Corrente**: Sensor não-invasivo de efeito transformador de corrente (SCT-013).
- **Cálculos Físicos em Tempo Real**:
  - Amostragem e cálculo de **Corrente RMS ($I_{\text{RMS}}$)** via biblioteca `EmonLib`.
  - **Potência Ativa ($P = V_{\text{RMS}} \cdot I_{\text{RMS}} \cdot \cos\theta$)** em Watts.
  - **Energia Acumulada em Joules ($E = \int P \, dt$)** integrada continuamente.
- **Nuvem em Tempo Real**: Transmissão sem fio instantânea dos valores amostrados para o **Google Firebase Realtime Database**, permitindo acompanhamento do consumo do veículo na pista pela equipe de telemetria nos boxes.

### 3. Placas de Circuito Impresso & Hardware Trainee (`placas-hardware-trainee/`)
- Layout esquemático e arte vetorial (SVG/PDF) para usinagem rápida de PCBs em fresadora CNC no FabLab UFPR.
- Modelagem mecânica de suportes de isolamento e fixação do sensor de corrente (arquivos STL para manufatura aditiva 3D).

---

## 📂 Estrutura do Repositório

```bash
eco-octano-hardware-embedded/
├── controle-motor-bldc/                  # Firmwares de controle de aceleração BLDC
│   ├── Controle_BLDC_Com_EGFV3.ino       # Versão V3 com tratamento de rampa
│   └── sem_limitador_controle_comercial_bldc.ino
├── joulimetro-monitor-energia-esp32/     # Monitor de energia ESP32 + Firebase
│   ├── src/main.cpp                      # Firmware com EmonLib e cliente Firebase
│   └── platformio.ini                    # Configuração de build
└── placas-hardware-trainee/              # Arquivos de fabricação e circuitos
    ├── Layout_Placa_Trainee_2025.pdf     # Desenho esquemático da placa
    ├── PCB sensor de corrente.svg        # Roteamento para usinagem FabLab
    ├── Sensor_Corrente_verde_v2.ino      # Código de teste e calibração
    └── suporte_corrente_dc.STL           # Suporte para fixação no chassi
```

---

## 👨‍💻 Autor

Desenvolvido por **Enzo Grasciano de Figueiredo**  
Universidade Federal do Paraná (UFPR) — Equipe Eco Octano  
E-mail: enzo.g.figueiredo@gmail.com  
GitHub: [@enzo-grasciano-de-figueiredo](https://github.com/enzo-grasciano-de-figueiredo)
