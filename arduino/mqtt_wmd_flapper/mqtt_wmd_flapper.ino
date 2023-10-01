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
const char* mqtt_client_name = "flapper"; // Client connections can't have the same connection name
const char* mqtt_status_topic = "flapper/status";
const char* mqtt_command_topic = "flapper/command";
//USER CONFIGURED SECTION END//

const int OPEN_PIN = 1;  // The pin to power to open the lid
const int CLOSE_PIN = 3; // The pin to power to close the lid
const int MOTOR_TIME = 500; // How long to run the motor to open/close the lid.

char *lastCommand = "unknown";

MQTTWifiWrapper wrapper(ssid, password, mqtt_server, mqtt_port, mqtt_user, mqtt_pass, mqtt_client_name, mqtt_status_topic, mqtt_command_topic);

void callback(char* topic, byte* payload, unsigned int length) {
  payload[length] = '\0';
  char* newPayload = ((char *)payload);
  if (strcmp(topic, mqtt_command_topic)==0) {
    // Determine which command we got, since pushing anything to the topic resets the payload value.
    bool is_same  = strcmp(newPayload, lastCommand)==0;
    bool is_open  = strcmp(newPayload, "open")==0;
    bool is_close = strcmp(newPayload, "close")==0;

    // Store this payload in the "lastCommand" variable
    strcpy(lastCommand, newPayload);

    // Issuing another "close" when the lid is already closed causes the lid to pop up,
    // allowing room for the cat to get in -- so do nothing if this command is the same
    // as the last command.
    if(!is_same) {
      if(is_open){
        wrapper.publish(mqtt_status_topic, "Opening");
        digitalWrite(CLOSE_PIN, LOW);
        digitalWrite(OPEN_PIN, HIGH);
        delay(MOTOR_TIME);
        digitalWrite(OPEN_PIN, LOW);
        wrapper.publish(mqtt_status_topic, "Open");
      }

      if(is_close) {
        wrapper.publish(mqtt_status_topic, "Closing");
        digitalWrite(OPEN_PIN, LOW);
        digitalWrite(CLOSE_PIN, HIGH);
        delay(MOTOR_TIME);
        digitalWrite(CLOSE_PIN, LOW);
        wrapper.publish(mqtt_status_topic, "Closed");
      }
    }
  }
}

void setup() {
  Serial.begin(115200);

  // Setup PINs
  pinMode(OPEN_PIN, OUTPUT);
  digitalWrite(OPEN_PIN, LOW);
  pinMode(CLOSE_PIN, OUTPUT);
  digitalWrite(CLOSE_PIN, LOW);

  // Setup Wifi and MQTT connection.
  wrapper.setup(callback);

  Serial.println("Setup finished");
}

void loop() {
  // Verify MQTT Connection
  wrapper.loop();
}
