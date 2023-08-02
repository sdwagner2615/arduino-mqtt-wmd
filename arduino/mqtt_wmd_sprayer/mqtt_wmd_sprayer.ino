#include "PubSubClient.h"
#include "ESP8266WiFi.h"


//USER CONFIGURED SECTION START//
const char* ssid = "###SSID###>";
const char* password = "###PASS###";

const char* mqtt_server = "###SERVER_IP####";
const int mqtt_port = 1883;
const char *mqtt_user = "###USER###";
const char *mqtt_pass = "###PASS###";
const char *mqtt_client_name = "WMD"; // Client connections can't have the same connection name
//USER CONFIGURED SECTION END//

int sprayPin = 3;
bool boot = true;

WiFiClient espClient;
PubSubClient client(espClient);

//Functions
void setup_wifi() 
{
  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, password);
  Serial.print("Connecting to wifi");
  while (WiFi.status() != WL_CONNECTED) 
  {
    Serial.print("*");
    delay(500);
  }
  Serial.println("connected!");
  Serial.print("The IP address of the module is: ");
  Serial.println(WiFi.localIP());
}

void reconnect() 
{
  // Loop until we're reconnected
  while (!client.connected()) 
  {
    // Attempt to connect
    Serial.println("Attempting MQTT Connection");
    if (client.connect(mqtt_client_name, mqtt_user, mqtt_pass, "wmd/status", 0, 0, "Dead Somewhere")) 
    {
      // Once connected, publish an announcement...
      if(boot == false)
      {
        client.publish("wmd/status", "Reconnected"); 
      }
      if(boot == true)
      {
        client.publish("wmd/status", "Rebooted");
        boot = false;
      }
      // ... and resubscribe
      client.subscribe("wmd/commands");
      Serial.println("Listening for MQTT commands");
    } 
    else 
    {
      // Wait 5 seconds before retrying
      Serial.println("MQTT Connection Failed");
      delay(5000);
    }
  }
}

void callback(char* topic, byte* payload, unsigned int length) 
{
  payload[length] = '\0';
  char* newPayload = ((char *)payload);
  Serial.print("Topic: ");
  Serial.println(topic);
  Serial.print("New Payload: ");
  Serial.println(newPayload);
  if (strcmp(topic, "wmd/commands")==0) 
  {
    if (strcmp(newPayload, "fire")==0)
    {
      client.publish("wmd/status", "Firing"); 
      digitalWrite(sprayPin, HIGH);
      delay(500);
      digitalWrite(sprayPin, LOW);
      client.publish("wmd/status", "Standby"); 
    }
    if (strcmp(newPayload, "dump")==0)
    {
      client.publish("wmd/status", "Dumping"); 
      digitalWrite(sprayPin, HIGH);
    }
    if (strcmp(newPayload, "stop")==0)
    {
      digitalWrite(sprayPin, LOW);
      client.publish("wmd/status", "Stopped"); 
    }
  }
}

void setup() 
{
  Serial.begin(115200);

  pinMode(sprayPin, OUTPUT);
  setup_wifi();
  client.setServer(mqtt_server, mqtt_port);
  client.setCallback(callback);
}

void loop() 
{
  if (!client.connected()) 
  {
    reconnect();
  }
  client.loop();
}
