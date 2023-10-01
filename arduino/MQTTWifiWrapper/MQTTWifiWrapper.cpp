#include "MQTTWifiWrapper.h"
#include <string.h>

MQTTWifiWrapper::MQTTWifiWrapper(const char* wifiSsid, const char* wifiPass, const char* mqttServer, unsigned int mqttPort, const char* mqttUser, const char* mqttPass, const char* mqttClient, const char* statusTopic, const char* commandTopic) {
    this->wifiSsid = wifiSsid;
    this->wifiPass = wifiPass;
    this->mqttServer = mqttServer;
    this->mqttPort = mqttPort;
    this->mqttUser = mqttUser;
    this->mqttPass = mqttPass;
    this->mqttClient = mqttClient;
    this->statusTopic = statusTopic;
    this->commandTopic = commandTopic;
    this->boot = true;  // Always initialize to TRUE
    this->wifiClient = WiFiClient();
    this->psClient = PubSubClient(this->wifiClient);
}

void MQTTWifiWrapper::setup(MQTT_CALLBACK_SIGNATURE) {
    // Setup Wifi Connection
    WiFi.mode(WIFI_STA);
    WiFi.begin(this->wifiSsid, this->wifiPass);
    Serial.print("Connecting to wifi");
    while (WiFi.status() != WL_CONNECTED)
    {
        Serial.print("*");
        delay(500);
    }
    Serial.println("connected!");
    Serial.print("The IP address of the module is: ");
    Serial.println(WiFi.localIP());

    // Print topic debug info
    Serial.print("Status topic: ");
    Serial.println(this->statusTopic);
    Serial.print("Command topic: ");
    Serial.println(this->commandTopic);

    // Setup MQTT Connection
    this->psClient.setServer(this->mqttServer, this->mqttPort);
    this->psClient.setCallback(callback);
    Serial.println("MQTT setup finished.");
}

void MQTTWifiWrapper::loop() {
    // If not connected, try to re-establish the MQTT connection
    while (!this->psClient.connected()) {
        // Attempt to connect
        Serial.println("Attempting MQTT Reconnection");
        if (this->psClient.connect(this->mqttClient, this->mqttUser, this->mqttPass, this->statusTopic, 0, 0, "Dead Somewhere"))
        {
            // Once connected, publish an announcement...
            if(this->boot == false)
            {
                this->psClient.publish(this->statusTopic, "Reconnected");
            }
            if(this->boot == true)
            {
                this->psClient.publish(this->statusTopic, "Rebooted");
                this->boot = false;
            }
            // ... and resubscribe
            this->psClient.subscribe(this->commandTopic);
            Serial.println("Connection established. Listening for MQTT commands");
        }
        else
        {
            // Wait 5 seconds before retrying
            Serial.println("MQTT Connection failed, waiting 5 seconds and retrying.");
            delay(5000);
        }
    }
    this->psClient.loop();
}

void MQTTWifiWrapper::publish(const char* topic, const char* payload) {
    this->psClient.publish(topic, payload);
}