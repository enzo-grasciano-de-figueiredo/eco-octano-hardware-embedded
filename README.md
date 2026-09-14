# ⚡ Eco Octano Hardware, Powertrain & Telemetria Energética (UFPR)

[![Team](https://img.shields.io/badge/Equipe-Eco%20Octano%20UFPR-red.svg)](https://ufpr.br/)
[![Platform](https://img.shields.io/badge/Embedded-ESP32%20%7C%20Arduino%20Nano-blue.svg)](https://espressif.com/)
[![Cloud](https://img.shields.io/badge/IoT-Google%20Firebase%20RTDB-FFCA28.svg)](https://firebase.google.com/)
[![Hardware](https://img.shields.io/badge/Sensors-SCT--013%20%7C%20EmonLib-green.svg)]()

> 🤖 **Nota de Transparência**: A documentação técnica, diagramas e estruturação deste repositório foram gerados/organizados de forma automatizada com assistência de Inteligência Artificial (Google DeepMind Antigravity / Gemini), com base no código-fonte, fotos de bancada, esquemáticos e processos de fabricação desenvolvidos pelo autor.

---

## 📸 Galeria de Hardware & Fabricação Própria (UFPR / Eco Octano)

> **Hardware real fabricado e testado pelo autor:** Placa de potência para controle de tração com corrosão química em percloreto de ferro, montagem manual e instrumentação da bancada de ensaios:

<p align="center">
  <img src="./controle-motor-bldc/photos/placa%20dc%201.jpeg" width="31%" alt="Placa DC montada e soldada" />
  <img src="./controle-motor-bldc/photos/placa%20dc%202.jpeg" width="31%" alt="Trilhas de cobre corroídas em percloreto de ferro" />
  <img src="./controle-motor-bldc/photos/placa%20dc%204.jpeg" width="31%" alt="Soldagem de componentes e conectores" />
</p>
<p align="center">
  <img src="./controle-motor-bldc/photos/placa%20dc%205.jpeg" width="48%" alt="Placa em bancada de ensaio e validação PWM" />
  <img src="./joulimetro-monitor-energia-esp32/photos/joulimetro%201.jpeg" width="48%" alt="Bancada do Joulímetro digital e telemetria" />
</p>

---

## 🏎️ Visão Geral dos Módulos

Projetos de eletrônica de potência, controle de tração e instrumentação desenvolvidos para os veículos protótipos de alta eficiência energética da **Equipe Eco Octano da Universidade Federal do Paraná (UFPR)**.

```
┌────────────────────────────────┐    PWM    ┌────────────────────────────────┐
│ Placa de Controle DC / BLDC    ├──────────►│ Controlador Comercial / Inversor│
│ (Corrosão em Percloreto Ferro) │           │ Motor Brushless DC             │
└────────────────────────────────┘           └────────────────────────────────┘

┌────────────────────────────────┐  I_rms/P  ┌────────────────────────────────┐
│ Joulímetro & Telemetria Nuvem  ├──────────►│ Google Firebase Realtime DB    │
│ (Projeto Trainee Eco Octano)   │  (Joules) │ Telemetria dos Boxes em Pista  │
└────────────────────────────────┘           └────────────────────────────────┘
```

---

## 🛠️ Detalhamento dos Projetos & Fotos Reais

### 1. Placa de Controle DC & Motor BLDC (`controle-motor-bldc/`)
- **Concepção & Fabricação Própria**: Projetada em software de layout de circuito impresso (PCB Layout) e **fabricada manualmente pelo autor através de corrosão química com Percloreto de Ferro**, furação de ilhas e soldagem de componentes discretos.
- **Controle de Tração**: Geração de sinais PWM com rampa suave de aceleração para evitar picos de partida e limitar a corrente máxima demandada da bateria.

<p align="center">
  <img src="./controle-motor-bldc/photos/placa%20dc%201.jpeg" width="30%" alt="Placa DC montada" />
  <img src="./controle-motor-bldc/photos/placa%20dc%202.jpeg" width="30%" alt="Trilhas corroídas em percloreto" />
  <img src="./controle-motor-bldc/photos/placa%20dc%204.jpeg" width="30%" alt="Soldagem e montagem" />
</p>
<p align="center">
  <img src="./controle-motor-bldc/photos/placa%20dc%205.jpeg" width="45%" alt="Bancada de testes" />
  <img src="./controle-motor-bldc/photos/placa%20dc%207.jpeg" width="45%" alt="Circuito completo" />
</p>

### 2. Joulímetro Digital & Telemetria em Nuvem (`joulimetro-monitor-energia-esp32/`)
- **Projeto de Ingresso Trainee**: Desenvolvido como projeto de admissão para a Equipe Eco Octano da UFPR.
- **Aquisição e Medição Energética**:
  - Leitura por sensor de corrente não-invasivo de núcleo bipartido (SCT-013).
  - Cálculo contínuo de **Corrente RMS ($I_{\text{RMS}}$)** e **Potência Ativa ($P$)** via biblioteca `EmonLib`.
  - **Integração temporal de energia consumida em Joules ($E = \int P \, dt$)** para fiscalização e estratégia de prova.
- **Transmissão em Tempo Real**: Envio dos dados via Wi-Fi para o **Google Firebase Realtime Database**, viabilizando o monitoramento telemétrico nos boxes em tempo real.

<p align="center">
  <img src="./joulimetro-monitor-energia-esp32/photos/joulimetro%201.jpeg" width="30%" alt="Joulímetro em bancada" />
  <img src="./joulimetro-monitor-energia-esp32/photos/joulimetro%202.jpeg" width="30%" alt="Detalhe das conexões do Joulímetro" />
  <img src="./joulimetro-monitor-energia-esp32/photos/joulimetro%203.jpeg" width="30%" alt="Instrumentação" />
</p>

### 3. Placas de Hardware Trainee & Usinagem FabLab (`placas-hardware-trainee/`)
- Esquemático técnico (`Layout_Placa_Trainee_2025.pdf`) e arte vetorial para usinagem rápida de circuitos em fresadora CNC no FabLab UFPR.
- Modelagem mecânica de suportes de isolamento do sensor de corrente (arquivo 3D `suporte_corrente_dc.STL`).

---

## 📂 Estrutura do Repositório

```bash
eco-octano-hardware-embedded/
├── controle-motor-bldc/                  # Placa de controle de motor fabricada em percloreto
│   ├── Controle_BLDC_Com_EGFV3.ino       # Firmware com rampa de aceleração
│   ├── sem_limitador_controle_comercial_bldc.ino
│   └── photos/                           # Fotos da fabricação da placa
├── joulimetro-monitor-energia-esp32/     # Joulímetro Trainee (ESP32 + Firebase)
│   ├── src/main.cpp                      # Firmware com cálculo de Joules e EmonLib
│   ├── platformio.ini                    # Configurações de build
│   └── photos/                           # Fotos do joulímetro em bancada
└── placas-hardware-trainee/              # Arquivos de fabricação e circuitos
    ├── Layout_Placa_Trainee_2025.pdf     # Desenho esquemático da placa
    ├── PCB sensor de corrente.svg        # Roteamento para usinagem FabLab
    └── suporte_corrente_dc.STL           # Suporte para fixação no chassi
```

---

## 👨‍💻 Autor

Desenvolvido por **Enzo Grasciano de Figueiredo**  
Universidade Federal do Paraná (UFPR) — Equipe Eco Octano  
E-mail: enzo.g.figueiredo@gmail.com  
GitHub: [@enzo-grasciano-de-figueiredo](https://github.com/enzo-grasciano-de-figueiredo)
