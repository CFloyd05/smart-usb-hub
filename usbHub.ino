#include <WiFi.h>
#include <PubSubClient.h>

// Wifi
const char *ssid = ""; // Wifi network name
const char *password = "";  // Wifi password

// MQTT Broker
const char *mqtt_broker = "192.168.50.58";
const int mqtt_port = 1883;

const char *topic1 = "galaxyLamp";
const char *topic2 = "sharkLamp";
const char *topic3 = "fairyLights";

const int topic1Pin = 23;
const int topic2Pin = 18;
const int topic3Pin = 19;

WiFiClient espClient;
PubSubClient client(espClient);
unsigned long lastReconnectAttempt = 0;
String client_id;

void setup() {
    Serial.begin(115200);

    // Connecting to wifi
    WiFi.begin(ssid, password);
    WiFi.setAutoReconnect(true);
    while (WiFi.status() != WL_CONNECTED) {
        delay(500);
        Serial.println("Connecting to WiFi..");
    }
    Serial.println("Connected to the Wi-Fi network");

    client_id = "esp32-1";
    client_id += String(WiFi.macAddress());

    // Connecting to mqtt broker
    client.setServer(mqtt_broker, mqtt_port);
    client.setKeepAlive(60);
    client.setCallback(callback);
    while (!client.connected()) {
        Serial.printf("The client %s is attempting to connect to MQTT broker\n", client_id.c_str());
        if (client.connect(client_id.c_str())) {
            Serial.println("MQTT broker connected");
        } else {
            Serial.print("failed with state ");
            Serial.print(client.state());
            delay(2000);
        }
    }
    pinMode(topic1Pin, OUTPUT);
    pinMode(topic2Pin, OUTPUT);
    pinMode(topic3Pin, OUTPUT);

    client.subscribe(topic1);
    client.subscribe(topic2);
    client.subscribe(topic3);

    digitalWrite(topic1Pin, LOW);
    digitalWrite(topic2Pin, LOW);
    digitalWrite(topic3Pin, LOW);
}

// Reconnect to MQTT broker
bool reconnect() {
    if (client.connect(client_id.c_str())) {
        client.subscribe(topic1);
        client.subscribe(topic2);
        client.subscribe(topic3);
    }
    return client.connected();
}

// Callback loop when message is received
void callback(char *topic, byte *payload, unsigned int length) {
    Serial.print("Message arrived in topic: ");
    Serial.println(topic);
    Serial.print("Message:");
    for (int i = 0; i < length; i++) {
        Serial.print((char) payload[i]);
    }
    Serial.println();
    Serial.println("-----------------------");

    if(strcmp(topic, topic1) == 0){
      if(payload[1] == 'N'){
        digitalWrite(topic1Pin, HIGH);
      }else{
        digitalWrite(topic1Pin, LOW);
      }
    }else if(strcmp(topic, topic2) == 0){
      if(payload[1] == 'N'){
        digitalWrite(topic2Pin, HIGH);
      }else{
        digitalWrite(topic2Pin, LOW);
      }
    }else if(strcmp(topic, topic3) == 0){
      if(payload[1] == 'N'){
        digitalWrite(topic3Pin, HIGH);
      }else{
        digitalWrite(topic3Pin, LOW);
      }
    }
}

void loop() {
    if (!client.connected()) {
        unsigned long now = millis();
        if (now - lastReconnectAttempt > 5000) {
            lastReconnectAttempt = now;
            if (reconnect()) {
                lastReconnectAttempt = 0;
                Serial.println("Reconnected to MQTT broker");
            }
        }
    } else {
        client.loop();
    }
}
