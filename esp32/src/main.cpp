#include <Arduino.h>
#include "WiFi.h"
#include "ESPAsyncWebServer.h"
#include "SPIFFS.h"
#include "HTTPSRedirect.h"
#include "DebugMacros.h"

const char* ssid = "YOUR_SSID";
const char* password = "YOUR_PASSWORD";

const int tx = 17;
const int rx = 16;
int t=0;
bool p=0;

String ledState;

AsyncWebServer server(80);

extern const char* host="script.google.com";
extern const char *GScriptId="YOUR_GOOGLE_APPS_SCRIPT_ID";

const int httpsPort = 443;

String url = String("/macros/s/") + GScriptId + "/exec";
HTTPSRedirect* client = nullptr;

// const IPAddress ip(192,168,128,100);
// const IPAddress subnet(255,255,255,0);

void connectWifi(){
  Serial.print("Connecting to wifi: ");
  // if (!WiFi.config(ip,ip,subnet)){
  //   Serial.println("Failed to configure!");
  // }
  Serial.println(ssid);
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("");
  Serial.println("WiFi connected");
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());
}

void createTLSConnection(){
  client = new HTTPSRedirect(httpsPort);
  client->setInsecure();
  client->setPrintResponseBody(true);
  client->setContentTypeHeader("application/json");
  
  Serial.print("Connecting to ");
  Serial.println(host);

  bool flag = false;
  for (int i=0; i<5; i++){
    int retval = client->connect(host, httpsPort);
    if (retval == 1) {
       flag = true;
       break;
    }
    else
      Serial.println("Connection failed. Retrying...");
  }

  if (!flag){
    Serial.print("Could not connect to server: ");
    Serial.println(host);
    Serial.println("Exiting...");
    return;
  }
}

void reqestSheet(){
  client->GET(url, host);
  Serial.println("Suceess");
}

void closeConnection(){
  delete client;
  client = nullptr;
}

void writeDateOnSheet(){
  createTLSConnection();
  reqestSheet();
  closeConnection();
}

String processor(const String& var){
}
 
void setup(){
  Serial.begin(9600);
  pinMode(tx, OUTPUT);

  if(!SPIFFS.begin(true)){
    Serial.println("An Error has occurred while mounting SPIFFS");
    return;
  }

  connectWifi();

  server.on("/", HTTP_GET, [](AsyncWebServerRequest *request){
    request->send(SPIFFS, "/index.html", String(), false, processor);
  });
  
  server.on("/style.css", HTTP_GET, [](AsyncWebServerRequest *request){
    request->send(SPIFFS, "/style.css", "text/css");
  });

  server.on("/on", HTTP_GET, [](AsyncWebServerRequest *request){
    digitalWrite(tx, HIGH);
    p=1;
    request->send(SPIFFS, "/index.html", String(), false, processor);
  });
  
  server.on("/off", HTTP_GET, [](AsyncWebServerRequest *request){
    digitalWrite(tx, LOW);    
    request->send(SPIFFS, "/index.html", String(), false, processor);
  });

  server.begin();
}
 
void loop(){
  if(p){
    writeDateOnSheet();
    p=0;
  }
}