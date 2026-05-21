#include <WiFi.h>
#include <PubSubClient.h>
#include <DHT.h>

/* Wireless and MQTT Network Profile Configuration -------------------------*/
const char* ssid = "99999999";        // Your target Wi-Fi Network SSID Name
const char* password = "99999999";    // Your target Wi-Fi Network Password Key

const char* mqtt_server = "broker.emqx.io";
const int mqtt_port = 1883;  
const char* mqtt_topic_temp = "home/sensor/dht22/temperature";
const char* mqtt_topic_hum = "home/sensor/dht22/humidity";
const char* client_id = "ESP32_WROOM_Industrial_Node_01";

/* Hardware Peripherals Routing Definitions ---------------------------------*/
#define DHTPIN 23
#define DHTTYPE DHT22
DHT dht(DHTPIN, DHTTYPE);

// Define the Indicator LED pin (GPIO 2 is the built-in blue LED on most ESP32 boards)
#define LED_PIN 2 

WiFiClient espClient;
PubSubClient client(espClient);

unsigned long lastPublishTime = 0;
const unsigned long publishInterval = 5000; // 5-second sampling loop interval

/* Network Initialization Infrastructure ------------------------------------*/
void setup_wifi() {
  delay(10);
  Serial.println();
  Serial.println("=========================================================");
  Serial.print("[WIFI-INIT] Initializing Radio Link to Network: ");
  Serial.println(ssid);
  
  WiFi.mode(WIFI_STA);  
  WiFi.begin(ssid, password);
  
  int attempts = 0;
  while (WiFi.status() != WL_CONNECTED && attempts < 20) {  
    delay(500);
    Serial.print(".");
    attempts++;
  }
  
  if (WiFi.status() == WL_CONNECTED) {
    Serial.println("\n[WIFI-SUCCESS] Wireless link lease achieved.");
    Serial.print("[WIFI-IP] Assigned Local IP Address: ");
    Serial.println(WiFi.localIP());
  } else {
    Serial.println("\n[WIFI-TIMEOUT] Network attachment failed.");
  }
}

void reconnect() {
  while (!client.connected()) {
    Serial.print("[MQTT-CONNECT] Connecting to Broker Server: ");
    Serial.println(mqtt_server);
    
    if (client.connect(client_id)) {
      Serial.println("[MQTT-SUCCESS] Connection Broker Network Layer Synchronized.");
    } else {
      Serial.print("[MQTT-ERROR] Handshake Refused. Return Error State = ");
      Serial.print(client.state());
      Serial.println(". Retrying connection loop in 5000ms...");
      // Flash the LED rapidly if the MQTT broker connection is failing
      digitalWrite(LED_PIN, HIGH); delay(100); digitalWrite(LED_PIN, LOW);
      delay(4900);
    }
  }
}

/* System Initialization and Setup -----------------------------------------*/
void setup() {
  Serial.begin(115200);
  Serial.println("\n\n=======================================================");
  Serial.println("****** ESP32-WROOM MULTI-INTERFACE SMART EDGE NODE ******");
  Serial.println("=======================================================");
  
  // Configure the LED Pin as an Output
  pinMode(LED_PIN, OUTPUT);
  digitalWrite(LED_PIN, LOW); // Start with the LED off
  
  dht.begin();
  Serial.println("[HARDWARE] DHT22 Single-Wire Digital Bus Initialized.");
  
  setup_wifi();
  client.setServer(mqtt_server, mqtt_port);
}

/* Main Core Execution Subsystem Loop ---------------------------------------*/
void loop() {
  if (WiFi.status() != WL_CONNECTED) {
    Serial.println("[WIFI-WARN] Wi-Fi Link Dropped! Initializing Recovery Intercept...");
    setup_wifi();
    if (WiFi.status() == WL_CONNECTED) {
      reconnect();
    }
    delay(5000);
    return;
  }
  
  if (!client.connected()) {
    reconnect();
  }
  client.loop();
  
  if (millis() - lastPublishTime >= publishInterval) {
    lastPublishTime = millis();
    
    // 💡 TURN LED ON: Visual indication that data collection has begun
    digitalWrite(LED_PIN, HIGH);
    
    float humidity = dht.readHumidity();
    float temperature = dht.readTemperature();
    
    if (isnan(humidity) || isnan(temperature)) {
      Serial.println("[CRITICAL-ERROR] Failed to read from DHT22! Check hardware bus lines.");
      // Rapid blinking indicates a sensor error
      for(int i=0; i<3; i++) {
        digitalWrite(LED_PIN, HIGH); delay(100); digitalWrite(LED_PIN, LOW); delay(100);
      }
      return;
    }
    
    char tempStr[16];
    char humStr[16];
    dtostrf(temperature, 4, 2, tempStr);
    dtostrf(humidity, 4, 2, humStr);
    
    /* INTERFACE 1: LOCAL UART TRANSMISSION LINE (FOR REALTERM LOGGING) */
    Serial.println("\n================== LOCAL FIELD DIAGNOSTIC ==================");
    Serial.print("  Captured Target Local Temperature : ");
    Serial.print(tempStr);
    Serial.println(" \xC2\xB0C"); 
    Serial.println("  System Hardware Bus State         : HEALTHY");
    Serial.println("============================================================");
    
    /* INTERFACE 2: MQTT BROKER WIRELESS TRANSMISSION LAYER */
    client.publish(mqtt_topic_temp, tempStr);
    
    char jsonBuffer[128];
    snprintf(jsonBuffer, sizeof(jsonBuffer), 
             "{\"node\":\"%s\",\"humidity\":%s,\"status\":\"OPERATIONAL\"}", 
             client_id, humStr);
             
    Serial.print("[MQTT-PUB] Routing Packet Frame to Topic: ");
    Serial.println(mqtt_topic_hum);
    
    if (client.publish(mqtt_topic_hum, jsonBuffer)) {
      Serial.print("           Payload Delivery Verified: ");
      Serial.println(jsonBuffer);
    } else {
      Serial.println("           [ERROR] Network Delivery Layer Refused Frame Delivery.");
    }
    
    // 💡 TURN LED OFF: Visual confirmation that transmission cycle is finished
    digitalWrite(LED_PIN, LOW);
  }
}