#include "PubSubClient.h"
#include "ESP8266WiFi.h"
#include "MQTTWifiWrapper.h"

//USER CONFIGURED SECTION START//
const char* ssid = "###SSID###>";
const char* password = "###PASS###";

const char* mqtt_server = "###SERVER_IP####";
const int   mqtt_port = 1883;
const char *mqtt_user = "###USER###";
const char *mqtt_pass = "###PASS###";
const char* mqtt_client_name = "sprayer"; // Client connections can't have the same connection name
const char* mqtt_status_topic = "sprayer/status";
const char* mqtt_command_topic = "sprayer/command";
//USER CONFIGURED SECTION END//

const int SPRAY_PIN = 3;

MQTTWifiWrapper wrapper(ssid, password, mqtt_server, mqtt_port, mqtt_user, mqtt_pass, mqtt_client_name, mqtt_status_topic, mqtt_command_topic);

void callback(char* topic, byte* payload, unsigned int length) {
  payload[length] = '\0';
  char* newPayload = ((char *)payload);
  if (strcmp(topic, mqtt_command_topic)==0) {
    if (strcmp(newPayload, "fire")==0) {
      wrapper.publish(mqtt_status_topic, "Firing");
      digitalWrite(SPRAY_PIN, HIGH);
      delay(500);
      digitalWrite(SPRAY_PIN, LOW);
      wrapper.publish(mqtt_status_topic, "Standby");
    }
  }
}

void setup() {
  Serial.begin(115200);

  // Setup PINs
  pinMode(SPRAY_PIN, OUTPUT);

  // Setup Wifi and MQTT connection.
  wrapper.setup(callback);

  Serial.println("Setup finished");
}

void loop() {
  // Verify MQTT Connection
  wrapper.loop();
}
