#ifndef MQTTWifiWrapper_h
#define MQTTWifiWrapper_h

#include "Arduino.h"
#include "PubSubClient.h"
#include "ESP8266WiFi.h"

class MQTTWifiWrapper
{
    public:
        MQTTWifiWrapper(const char* wifiSsid, const char* wifiPass, const char* mqttServer, unsigned int mqttPort, const char* mqttUser, const char* mqttPass, const char* mqttClient, const char* statusTopic, const char* commandTopic);
        void setup(MQTT_CALLBACK_SIGNATURE);
        void loop();
        void publish(const char* topic, const char* payload);
    private:
        const char* wifiSsid;
        const char* wifiPass;
        const char* mqttServer;
        unsigned int mqttPort;
        const char* mqttUser;
        const char* mqttPass;
        const char* mqttClient;
        const char* statusTopic;
        const char* commandTopic;
        bool boot;
        WiFiClient wifiClient;
        PubSubClient psClient;
};

#endif