# Multi-Interface Edge Computing Node: Heterogeneous Data Splitting

## 🎓 Academic Portfolio & Student Metadata
* **Student Name:** [Your Name Here] * **Roll Number:** [Your Roll Number Here] * **Institution:** Chitkara University Institute of Engineering and Technology (CUIET)
* **Evaluation Track:** Lab Evaluation – IV (IoT Subsystems & Heterogeneous Routing)

---

## 🚀 Key Project Features
This project configures an ESP32 edge device to split environmental telemetry data pathways cleanly: transmitting humidity metrics to a remote cloud dashboard using an MQTT broker over WebSockets, while piping temperature metrics locally to an on-site hardware serial interface (Realterm) via UART.

* **Dual-Route Telemetry Split:** Decouples environmental metrics to maximize data availability and eliminate standard points of failure in critical IIoT deployments.
* **Edge Resiliency Check:** A non-blocking filter ensures that local hardware serial logs continue to pipe metrics seamlessly over UART even if the network uplink drops completely.
* **Active Collection LED Indicator:** Provides quick on-site visual confirmation pulses by toggling a dedicated LED on GPIO Pin 2 when data processing maps successfully.

---

## 🔌 Hardware Setup & Connection Topology

The system uses an ESP32-WROOM developer module connected to a single-wire DHT22 digital sensor. An indicator LED is map-routed to display a physical blink whenever a successful sampling cycle executes.

### Pin Mapping Matrix

| Physical Component | Component Pin Name | ESP32 Target Pin Assignment | Connection Line Function |
| :--- | :--- | :--- | :--- |
| **DHT22 Sensor** | VCC | 3.3V Pin | Positive Rail Power Supply (+3.3V DC) |
| **DHT22 Sensor** | DATA | GPIO 23 | Single-Wire Digital Signal Data Bus |
| **DHT22 Sensor** | GND | GND Pin | Shared Ground Return Path |
| **Indicator LED** | Anode (+) | GPIO 2 (Onboard Blue LED) | Signal Status Trigger for Collection Cycle |
| **Indicator LED** | Cathode (-) | GND Pin | Shared Ground Return Path |

---

## 🛠️ Step-by-Step Deployment Tutorial

### Phase 1: Local Firmware Setup (Arduino IDE)
1. Ensure you have downloaded and installed the **Arduino IDE**.
2. Navigate to your IDE Preferences and add the ESP32 Board Manager URL:
   `https://raw.githubusercontent.com/espressif/arduino-esp32/gh-pages/package_esp32_index.json`
3. Go to **Tools > Board > Board Manager**, search for `esp32` by Espressif, and install it.
4. Open **Tools > Manage Libraries...** and install the following dependencies:
   * `DHT sensor library` (by Adafruit)
   * `PubSubClient` (by Nick O'Leary)
5. Open the firmware file inside `/src/IOT-AD1.ino`, update your local network credentials (`ssid` and `password`), and flash it to your ESP32 board over **COM3**.

### Phase 2: Local Serial Intercept Terminal Verification (Realterm)
1. Launch **Realterm** on your diagnostic monitoring machine.
2. Navigate directly to the **Port** configuration tab. 
3. Configure the hardware connection parameters precisely: Set the **Baud Rate to 115200**, **Data Bits to 8**, **Parity to None**, and **Stop Bits to 1**.
4. Select the active port matching your device interface (e.g., `COM3`) and click **Open**.
5. Observe the clean text blocks streaming raw ambient temperature fields locally over the serial bus in a human-readable ASCII layout.

### Phase 3: Cloud Web Dashboard Activation
1. Locate the frontend dashboard deployment file inside your repository workspace (`/src/INDEX.html`).
2. Open the file inside any modern web browser to launch the client interface.
3. The built-in console logging block will establish and verify the secure handshake over `wss://broker.emqx.io:8084/mqtt`. 
4. Once connected, the virtual gauge dashboard cards will begin rendering live relative humidity metrics parsed straight from the incoming JSON telemetry frames.

---

## 📁 Repository File Layout
```text
├── src/
│   ├── IOT-AD1.ino          # Embedded C++ device production source script
│   └── INDEX.html           # Live broker subscription tracking interface code
├── docs/
│   └── schematic.png        # Technical connection routing schema map
└── images/
    ├── realterm_snapshot.png # Realterm hardware window trace log screenshot
    └── physical_setup.jpg   # Photo of the completed physical test circuit