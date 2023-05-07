#include <Arduino.h>
#include <ESP8266WiFi.h>
#include <ESPAsyncTCP.h>

#include "ESPAsyncWebServer.h"


#define relayPin D4
#define openPin D6
#define closePin D7

const char* PARAM_INPUT_1 = "password";
const char* backgroundColor = "green";

boolean checkAccess(AsyncWebServerRequest *request); 
String processor(const String& var);
String getStatus();
void open();
void close();
void toggle();
void toggleGarageDoor();

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
    html {font-family: Arial; display: inline; text-align: center;}
    body {display: inline;}
    h2 {font-size: 3.0rem;}
    p {font-size: 3.0rem;}
    button{width: 150px; height: 150px; font-size:5vmax; margin: 20px; display: inline-block;}
    #status{background-color: %backgroundColor%; width: %withpercent%; margin: auto; height: 50px; font-size: xx-large;}
    button:active{ background-color: rgb(118, 118, 118);}

    body {max-width: 600px; margin:0px auto; padding-bottom: 25px;}
  </style>
</head>
<body>
  <h2>Open Garage</h2>
  <div id="status">
    %statusInsert%
  </div>
  <button onclick="doRequest('open')">Open Garage</button>
  <button onclick="doRequest('close')">Close Garage</button>
  <button onclick="doRequest('toggle')">Toggle Garage</button>
  <button onclick="doRequest('stop')">Stop Garage</button>


<script>

function doRequest(endpoint){
  changeStatus(endpoint)
  var xhr = new XMLHttpRequest();
  xhr.open("GET", `/${endpoint}?password=thanner2022`, true);
  xhr.send();
}

function changeStatus(endpoint){
  switch (endpoint) {
    case 'open':
      changeStatusOpen()
      break;
    case 'close':
      changeStatusClosed()
      break;
    case 'toggle':
      toggleStatus()
      break;
  }
}
function toggleStatus(){
  switch (document.getElementById("status").innerHTML){
    case 'Status: Open':
      changeStatusClosed()
      break;
    case 'Status: Closed':
      changeStatusOpen()
      break;
    
  }
}

function changeStatusOpen(){
  document.getElementById("status").innerHTML = "Status: Open";
  document.getElementById("status").style.backgroundColor = "green";
}

function changeStatusClosed(){
  document.getElementById("status").innerHTML = "Status: Closed";
  document.getElementById("status").style.backgroundColor = "red";
}

</script>
</body>
</html>
)rawliteral";

AsyncWebServer server(80);
 
WiFiClient wifi;

void setup() {
  // put your setup code here, to run once:

  Serial.begin(9600);
  Serial.println();
  Serial.print("Connecting to ");
  Serial.println(ssid);

  pinMode(relayPin, OUTPUT);
  pinMode(openPin, INPUT);
  pinMode(closePin, INPUT);

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
    if(checkAccess(request)){
      request->send_P(200, "text/html", index_html, processor);
    }         
  });

  server.on("/open", HTTP_GET, [] (AsyncWebServerRequest *request){
      if(checkAccess(request)){
        open();
        request->send(200);
      } 
  });

  server.on("/close", HTTP_GET, [] (AsyncWebServerRequest *request){
      if(checkAccess(request)){
        close();
        request->send(200);
      }  
  });
 
  server.on("/toggle", HTTP_GET, [] (AsyncWebServerRequest *request){
    if(checkAccess(request)){
      toggle();
      request->send(200);
    }  
  });

  server.on("/stop", HTTP_GET, [] (AsyncWebServerRequest *request){
    if(checkAccess(request)){
      toggleGarageDoor();
      request->send(200);
    }  
  });

  server.begin();
}

String processor(const String& var){
    if(var == "backgroundColor"){
      return backgroundColor;
    }
    if (var == "statusInsert") {
      return getStatus();
    }
    if (var == "withpercent") {
      return "80%%";
    }
    return String();
  }

boolean checkAccess(AsyncWebServerRequest *request) {

  if (request->hasParam(PARAM_INPUT_1)) {
    String message = request->getParam(PARAM_INPUT_1)->value();
    if (message == "thanner2022"){
      Serial.println("Opening the Garage");
      return true;

    }else {
      request->send(401, "text/plain", "Wrong password sent");
    }
  } else {
    request->send(401, "text/plain", "No password sent");
  }
  return false;
}

String getStatus(){
  if(digitalRead(openPin)){
    return "Status: Closed";
  }

  if(digitalRead(closePin)){
    return "Status: Closed";
  }
  return "Status: Error";
}

void open() {
  //if openPin is at 5V the garage is closed
  if (digitalRead(openPin)){
    backgroundColor = "green";
    toggleGarageDoor();
  }
}

void close() {
    //if closePin is at 5V the garage is open
  if (digitalRead(closePin)){
    backgroundColor = "red";

    toggleGarageDoor();
  }
}

void toggle(){
  if (digitalRead(closePin) || digitalRead(openPin)){
    toggleGarageDoor();
  }
}

void toggleGarageDoor(){
  digitalWrite(relayPin, HIGH);  
  delay(100);
  digitalWrite(relayPin, LOW);
}

void loop() {
  Serial.println("open: ");
  Serial.println(digitalRead(openPin));
  Serial.println("close: ");
  Serial.println(digitalRead(closePin));

  delay(200);

}

