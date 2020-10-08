#include "FS.h"
#include <ESP8266WiFi.h>        // Lib for esp266
#include <DNSServer.h>          //Local DNS Server used for redirecting all requests to the configuration portal
#include <ESP8266WebServer.h>   //Local WebServer used to serve the configuration portal
#include <PubSubClient.h>       // Lib for for MQTT protocols , and PUB/SUB cilent
#include <NTPClient.h>          // Lib for internet Time
#include <WiFiUdp.h>            // Lib for UDP protocols

// Wifi SSID and Password
const char *ssid = "YOUR WIFI ROUTER";
const char *password = "YOUR WIFI ROUTER's PASSWORD";

// Output Pin for the LED
const int led_output = 2;

// Wifi UDP client
WiFiUDP ntpUDP;
// Time client to fetch Internet Time
NTPClient timeClient(ntpUDP, "pool.ntp.org");

//AWS endpoint 
const char* AWS_endpoint = "YOUR ACCOUNT ENDPOINT";

//Subscribing ESP8266 to the topics in AWS and receiving commands to control the devices
void callback(char* topic, byte* payload, unsigned int length) {
  Serial.print("Message Received : ");
  Serial.print(topic);
  for (int i = 0; i < length; i++) {
    Serial.print((char)payload[i]);  //Printing Payload content
  }
  char led = (char)payload[62]);   //Extracting the commands
  Serial.print("Led Command : ");
  Serial.println(led);
  if(led==1)   
  {
    digitalWrite(led_output, HIGH);   //2 is output led pin
  }
  else if (led==0)  
  {
    digitalWrite(led_output, LOW);   //2 is output led pin
    Serial.println()
}
WiFiClientSecure espClient;
PubSubClient client(AWS_endpoint, 8883, callback, espClient);  //set MQTT port number to 8883 as per standard

void setup_wifi() {
  esp.setBufferSizes(512, 512);
  Serial.println();
  Serial.print("Connecting to ");
  Serial.println(ssis);

  WiFi.begin(ssid, password);

  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }

  Serial.println("");
  Serial.println("WiFi connected");
  Serial.println("IP Address: ");
  Serial.println(WiFi.localIP());

  timeClient.begin();
  while(!timeClient.update()) {
    timeClient.forceUpdate();
  }

  espClient.setx509Time(timeClient.getEpochTime());
}  


void reconnect() {
  //Continuing until device is connected
  while (!client.connected()) {
    Serial.print("Attempting MQTT connection...");

    if (client.connect(YOUR THING NAME)) {
      Serial.println("Connected");

      // After Connecting Publish and Subscribe
      client.publish("YOUR TOPIC NAME");

      clinet.subscribe("TOPIC NAME");
    }
    else
    {
      Serial.print("Failed, rc = ");
      Serial.print(client.state());
      Serial.println("Try again...");

      char buf[256];
      espClient.getLastSSLError(buf, 256);
      Serial.print("WiFiClientSecure SSL error: ");
      Serial.println(buf);

      delay(5000);
    }
  }
}  


// Setup code
void setup(){
  Serial.begin(115200);
  Serial.setDebugOutput(true);  
  //Initializing digital pin as an output
  pinMode(2, OUTPUT);
  setup_wifi();
  delay(1000)
  if (!SPIFFS.begin()){              //Checks if the system is mounted properly
    Serial.println("Failed to mount file System");
    return;
  }  
  Serial.print("Heap : ");
  Serial.println(ESP.getFreeHeap());

  
  // Loading Certificate
  File cert - SPIFFS.open("YOUR FILE NAME.EXTENSION", "r")
  if(!cert) {
    Serial.println("Failed to open File");
  }
  else
  {
    Serial.println("Certificate opened Successfully");
  }

  delay(1000);
  
  if(espClient.loadCertificate(cert)) {
    Serial.print("Certificate Loaded");
  }
  else
  {
    Serial.println("Error Loading Certificate");
  }
  
  // Loading Private Key File
  File private_key - SPIFFS.open("YOUR FILE NAME.EXTENSION", "r")
  if(!private_key) {
    Serial.println("Failed to open File");
  }
  else
  {
    Serial.println("Private Key opened Successfully");
  }

  delay(1000);
  
  if(espClient.loadPrivateKey(private_key)) {
    Serial.print("Private Key Loaded");
  }
  else
  {
    Serial.println("Error Loading Private Key");
  }
  
  // Loading CA file
  File ca - SPIFFS.open("YOUR FILE NAME.EXTENSION", "r")
  if(!ca) {
    Serial.println("Failed to open File");
  }
  else
  {
    Serial.println("CA File opened Successfully");
  }

  delay(1000);
  
  if(espClient.loadCACert(ca)) {
    Serial.print("CA Loaded");
  }
  else
  {
    Serial.println("Error Loading CA");
  }

  Serial.print("Heap : ");
  Serial.println(ESP.getFreeHeap());
}



// Loop code
void loop() {
  if(!client.connected()) {
    reconnect();
  }
  client.loop();
}
