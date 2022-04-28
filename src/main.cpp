#include <Arduino.h>
#include <ESP8266WiFi.h>
#include <ESPAsyncTCP.h>

#include "ESPAsyncWebServer.h"


#define relayPin D4

const char* PARAM_INPUT_1 = "password";


//test
const char* ssid = "home";
const char* password = "39498482789698294254";

const char index_html[] PROGMEM = R"rawliteral(
<!DOCTYPE HTML><html>
<head>
  <title>Open Garage</title>
  <meta name="viewport" content="width=device-width, initial-scale=1">
  <link rel="icon" href="data:,">
  <style>
    html {font-family: Arial; display: inline-block; text-align: center;}
    h2 {font-size: 3.0rem;}
    p {font-size: 3.0rem;}
    button{width: 80%; height: 50%; font-size: 20vmin;}

    button:active{width: 80%; height: 50%; font-size: 20vmin; background-color: rgb(118, 118, 118);}

    body {max-width: 600px; margin:0px auto; padding-bottom: 25px;}
  </style>
</head>
<body>
  <h2>Open Garage</h2>
  <button onclick="openGarage()">Open Garage</button>

<script>function openGarage(){
  var xhr = new XMLHttpRequest();
  xhr.open("GET", "/open?password=thanner2022", true);
  xhr.send();
}
</script>
</body>
</html>
)rawliteral";

bool open = false;

AsyncWebServer server(80);

 
WiFiClient wifi;

void setup() {
  // put your setup code here, to run once:

  Serial.begin(9600);
  Serial.println();
  Serial.print("Connecting to ");
  Serial.println(ssid);

  pinMode(relayPin, OUTPUT);

  WiFi.hostname("garageESP");
  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, password);

  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("Connected to the Wifi");
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());

  WiFi.setAutoReconnect(true);
  WiFi.persistent(true);


  server.on("/garage", HTTP_GET, [] (AsyncWebServerRequest *request){
        String message;
        Serial.println("recieved ");

        if (request->hasParam(PARAM_INPUT_1)) {
            message = request->getParam(PARAM_INPUT_1)->value();

            if (message == "thanner2022"){
                 

              request->send_P(200, "text/html", index_html);


            }else {
              request->send(401, "text/plain", "Wrong password sent");
            }
        } else {
          request->send(401, "text/plain", "No password sent");
        }
         
    });

    server.on("/open", HTTP_GET, [] (AsyncWebServerRequest *request){
        String message;
        Serial.println("recieved ");

        if (request->hasParam(PARAM_INPUT_1)) {
            message = request->getParam(PARAM_INPUT_1)->value();

            if (message == "thanner2022"){
              Serial.println("Opening the Garage");
              
              open = true;          

              request->send_P(200, "text/html", index_html);


            }else {
              request->send(401, "text/plain", "Wrong password sent");
            }
        } else {
          request->send(401, "text/plain", "No password sent");
        }
         
    });
 
  server.begin();

  
}

void loop() {

  if (open){
    digitalWrite(relayPin, HIGH);  
    delay(100);
    digitalWrite(relayPin, LOW);

    open = false;
  }
}

