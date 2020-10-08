#include <ESP8266WiFi.h>        // Lib for esp266
#include <DNSServer.h>          //Local DNS Server used for redirecting all requests to the configuration portal
#include <ESP8266WebServer.h>   //Local WebServer used to serve the configuration portal
#include <PubSubClient.h>       // Lib for for MQTT protocols , and PUB/SUB cilent
#include <NTPClient.h>          // Lib for internet Time
#include <WiFiUdp.h>            // Lib for UDP protocols

// Wifi SSID and Password
const char *ssid = "YOUR WIFI ROUTER";
const char *password = "YOUR WIFI ROUTER's PASSWORD";

// Input/output for GPIOS of remote button and feedback led 
const int input_button = 0 ; 
const int led_output = 2;

// variables for Interrupts calls on button press
volatile unsigned long debounceDuration = 300;

// for interrupt debounce
volatile unsigned long lastValidInterruptTime_1 = 0;
unsigned long currrentMillis_interrupt_1 = 0;

volatile int myISR1_flag = 0;

const int MQTT_PORT = 8883;
//Subscribing Topic
const char* MQTT_SUB_TOPIC = "$aws/things/" THINGNAME "/shadow/update/accepted";
//Publishing Topic
const char* MQTT_PUB_TOPIC = "$aws/things/" THINGNAME "/shadow/update";

// Wifi UDP client
WiFiUDP ntpUDP;
// Time client to fetch Internet Time
NTPClient timeClient(ntpUDP, "pool.ntp.org");


// Interrupt service routine , should very light weight and shoud not contain delay() or any other interrrupts.
ICACHE_RAM_ATTR void myIRS1()
{
  if (currrentMillis_interrupt_1 - lastValidInterruptTime_1 > debounceDuration)
  {
    myISR1_flag = 1;
    lastValidInterruptTime_1 = currrentMillis_interrupt_1;
  }
}

// functions/steps to execute on interrupt 1
void myIRS1_method()
{
  Serial.println("ISR1 + myIRS1_method Called!");
  // Write here whate to do when button is pressed.
  String led = "";
    for (int i=0;i<length;i++)
    {
    led += (char)payload[i];
    }
    String led = led;
    Serial.println("Recieved [" + String(topic) + "]: "+ msgString);
    
    StaticJsonDocument<200> doc;
    StaticJsonDocument<64> filter;
    filter["state"]["reported"]["led"] = true;
    //filter["state"]["reported"]["button"] = true;
    DeserializationError error = deserializeJson(doc,msgString, DeserializationOption::Filter(filter));

    // Test if parsing succeeds.
    if (error) {
      Serial.print(F("deserializeJson() failed: "));
      Serial.println(error.c_str());
      return;
    }
    
    if(doc["state"]["reported"]["led"]=="0"){  
      digitalWrite(led_output,LOW);                //Led Turned off 
    }  
    if(doc["state"]["reported"]["led"]=="1"){ 
      digitalWrite(led_output,HIGH);               //Led Turned on 
    }
  Serial.println();
}

void connectToMqtt(bool nonBlocking = false)
{
  Serial.print("MQTT connecting ");
  while (!client.connected())
  {
    if (client.connect(THINGNAME))
    {
      Serial.println("connected!");
      if (!client.subscribe(MQTT_SUB_TOPIC))
        pubSubErr(client.state());
    }
    else
    {
      Serial.print("failed, reason -> ");
      pubSubErr(client.state());
      if (!nonBlocking)
      {
        Serial.println(" < try again in 5 seconds");
        delay(5000);
      }
      else
      {
        Serial.println(" <");
      }
    }
    if (nonBlocking)
      break;
  }
}


// Setup code for interrupt
void setupISR1()
{
  currrentMillis_interrupt_1 = millis();
  attachInterrupt(digitalPinToInterrupt(input_button_1), myIRS1, FALLING);
}

void setup(){
  Serial.begin(115200);

  // Set GPIO to OUTPUT MODE
  pinMode(led_output, OUTPUT);
  pinMode(input_button, INPUT)
  //initially set GPIO to LOW
  digitalWrite(led_output,LOW);
  
  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, pass);
  connectToWiFi(String("Attempting to connect to SSID: ") + String(ssid));

  NTPConnect();

  #ifdef ESP32
    net.setCACert(CA);
    net.setCertificate(cert);
    net.setPrivateKey(private_key);
  #else
    net.setTrustAnchors(&cert);
    net.setClientRSACert(&CA, &private_key);
  #endif

    client.setServer(MQTT_HOST, MQTT_PORT);
    client.setCallback(messageReceived);

  connectToMqtt();
  // setup IRS 1
  setupISR1();
  

}

// Iterrupt 1 method call check
void myIRS_check()
{
  // Main part of your loop code.
  // used for decouncing
  currrentMillis_interrupt_1 = millis();

  // IRS for in-built button
  if (myISR1_flag)
  {
    myISR1_flag = 0;
    myIRS1_method();
  }

}

// Loop code
void loop(){

  // check flags is there any interrupt calls made
  myIRS_check();
  
  //Publishing remote data to AWS
  {
    client.loop();
    if(digitalRead(input_button)==LOW){
      if(buttonState){
        Serial.println("Button Data Published 0");
        client.publish(MQTT_PUB_TOPIC,"{\"state\" : {\"reported\" : {\"button\" : \"0\" } } }");
         buttonState=false;
       }
       else{
        Serial.println("Button Data Published 1");
          client.publish(MQTT_PUB_TOPIC,"{\"state\" : {\"reported\" : {\"button\" : \"1\" } } }");
          buttonState=true;
       }  
    }
  }
}
