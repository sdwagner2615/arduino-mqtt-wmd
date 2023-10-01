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
const char* mqtt_client_name = "feeder"; // Client connections can't have the same connection name
const char* mqtt_status_topic = "feeder/status";
const char* mqtt_command_topic = "feeder/command";
//USER CONFIGURED SECTION END//

const int FOOD_PIN = 1;
const int LOCK_PIN = 3;
unsigned long lastUnlock = millis();

MQTTWifiWrapper wrapper(ssid, password, mqtt_server, mqtt_port, mqtt_user, mqtt_pass, mqtt_client_name, mqtt_status_topic, mqtt_command_topic);

void callback(char* topic, byte* payload, unsigned int length) {
  payload[length] = '\0';
  char* newPayload = ((char *)payload);
  if (strcmp(topic, mqtt_command_topic)==0)
  {
    // The feeder locks after 60s. If it's been longer than 60s since
    // the feeder was last unlocked, issue a long-press on the lock pin.
    // If currentMillis is < lastUnlock, then millis() has overflowed so
    // just attempt to unlock so "lastUnlock" is reset. (will happen
    // after 50 days according to millis() docs).
    unsigned long currentMillis = millis();
    unsigned long sinceLastLock = currentMillis - lastUnlock;
    bool is_locked = (sinceLastLock > 60000 || currentMillis < lastUnlock);

    // Check the payload here since once we start publishing to MQTT,
    // the payload parameter values will be overwritten in memory.
    bool is_feed = strcmp(newPayload, "feed")==0;

    if(is_locked){
      wrapper.publish(mqtt_status_topic, "Unlocking");
      pinMode(LOCK_PIN, HIGH);
      delay(3000); // Needs to be held for 3s according to the manual.
      pinMode(LOCK_PIN, LOW);
      lastUnlock = currentMillis;
    }

    // Issue a quick press of the dispense button and then wait 3s, before allowing
    // any other command. Presses faster than that seem to get dropped.
    if (is_feed)
    {
      wrapper.publish(mqtt_status_topic, "Dispensing");
      pinMode(FOOD_PIN, HIGH);
      delay(500);
      pinMode(FOOD_PIN, LOW);
      delay(3000); // Triggering a button any sooner does nothing.
      wrapper.publish(mqtt_status_topic, "Standby");
    }
  }
}

void setup() {
  Serial.begin(115200);

  // Setup PINs
  pinMode(LOCK_PIN, LOW);
  pinMode(FOOD_PIN, LOW);

  // Setup Wifi and MQTT connection.
  wrapper.setup(callback);

  Serial.println("Setup finished");
}

void loop() {
  // Verify MQTT Connection
  wrapper.loop();
}
