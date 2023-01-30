#include <WiFi.h>
#include "ESPAsyncWebServer.h"  
#include <WebSocketsServer.h>
#include <ArduinoJson.h>
#include <Preferences.h>
// Set to true to define Relay as Normally Open (NO)
#define RELAY_NO true

// Set number of relays
#define NUM_RELAYS 8

#define CONNECTION_TIMEOUT 10

// Assign each GPIO to a relay
int relayGPIOs[NUM_RELAYS] = {14, 32, 33, 25, 26, 27, 2, 15};

String rNames[NUM_RELAYS];

typedef struct {
    uint8_t rindex;
    String rname;   
  } aufzeichnungen;

aufzeichnungen temp_array[8];
Preferences prefs;

StaticJsonDocument<200> doc_tx;
StaticJsonDocument<200> doc_rx;

const char* PARAM_INPUT_3 ="rindex";
const char* PARAM_INPUT_4 ="rname";

// Replace with your network credentials
const char* ssid = "your network name";
const char* password = "your network password";

// Set your Static IP address
IPAddress local_IP(192, 168, 0, 111);
IPAddress gateway(192, 168, 1, 1);
IPAddress subnet(255, 255, 0, 0);

const char* PARAM_INPUT_1 = "relay";  
const char* PARAM_INPUT_2 = "state";

// Create AsyncWebServer object on port 80
AsyncWebServer server(80);
WebSocketsServer webSocket = WebSocketsServer(81);

const char index_html[] PROGMEM = R"rawliteral(
<!DOCTYPE HTML><html>
<head>
  <title>ESP 32 Steuerung</title>
  <meta name="viewport" content="width=device-width, initial-scale=1">  
  <style>
    html {font-family: Arial; display: inline-block; text-align: center;}
    h2 {font-size: 3.0rem;}
    p {font-size: 3.0rem;}
    body {max-width: 600px; margin:0px auto; padding-bottom: 25px; background: linear-gradient(to right, #4ca2cd, #67B26F);}
    .switch {position: relative; display: inline-block; width: 120px; height: 68px} 
    .switch input {display: none}
    .slider {position: absolute; top: 0; left: 0; right: 0; bottom: 0; background-color: #ccc; border-radius: 34px}
    .slider:before {position: absolute; content: ""; height: 52px; width: 52px; left: 8px; bottom: 8px; background-color: #fff; -webkit-transition: .4s; transition: .4s; border-radius: 68px}
    input:checked+.slider {background-color: #2196F3}
    input:checked+.slider:before {-webkit-transform: translateX(52px); -ms-transform: translateX(52px); transform: translateX(52px)}
    button {font-size: 3.0rem;}

      .saveNamebtn {
            text-align: center;
            text-transform: uppercase;
            cursor: pointer;
            font-size: 20px;
            letter-spacing: 4px;
            position: relative;
            background-color: #16a085;
            border: none;
            color: #fff;
            padding: 20px;
            width: 200px;
            text-align: center;
            transition-duration: 0.4s;
            overflow: hidden;
            box-shadow: 0 5px 15px #193047;
            border-radius: 4px;
        }
        .saveNamebtn:hover {
            background: #fff;
            box-shadow: 0px 2px 10px 5px #1abc9c;
            color: #000;
        }
        .saveNamebtn:after {
            content: "";
            background: #1abc9c;
            display: block;
            position: absolute;
            padding-top: 300%;
            padding-left: 350%;
            margin-left: -20px !important;
            margin-top: -120%;
            opacity: 0;
            transition: all 0.8s
        }
        .saveNamebtn:active:after {
            padding: 0;
            margin: 0;
            opacity: 1;
            transition: 0s
        }
        .saveNamebtn:focus { outline:0; }       

         h1 {
            position: relative;
            padding: 0;
            margin: 0;
            font-family: "Raleway", sans-serif;
            font-weight: 300;
            font-size: 40px;
            color: #080808;
            -webkit-transition: all 0.4s ease 0s;
            -o-transition: all 0.4s ease 0s;
            transition: all 0.4s ease 0s;
        }
        h1 span {
            display: block;
            font-size: 0.5em;
            line-height: 1.3;
        }
        h1 em {
            font-style: normal;
            font-weight: 600;
        }
        .four h1 {
            text-align: center;
            padding-bottom: 0.7em;
        }
        .four h1 span {
            font-weight: 300;
            word-spacing: 3px;
            line-height: 2em;
            padding-bottom: 0.35em;
            color: rgba(0, 0, 0, 0.5);
        }
        .four h1:before {
            position: absolute;
            left: 0;
            bottom: 0;
            width: 60px;
            height: 1px;
            content: "";
            left: 50%;
            margin-left: -30px;
            background-color: #777;
        } 


        .changeNameinput {
            color: #ffffff;
            background-color: #2d63c8;
            font-size: 19px;
            border: 1px solid #2d63c8;
            border-radius: 10px;
            padding: 15px 30px;
            cursor: pointer;
            margin-top: 0.3rem;
            margin-bottom: 1.0rem;
            margin-right: 1.0rem;
            position: center;
            text-align: center;
            min-width: 5rem;
        }
  </style>
</head>
<body>
  <div class="four">
    <h1><span>8 Relays - </span> ESP32 <em>WEB</em> Server</h1>
  </div>
  %BUTTONPLACEHOLDER%
<script>
      var Socket;
      function init()
      {
        Socket = new WebSocket('ws://'+ window.location.hostname + ':81/');
        //Socket.onmessage = function(event){
        //  processCommand(event);
        //};
      }

      function button_send_back(element)
      {
        var ibtnId = parseInt(element.id);
        var itmp = ibtnId-11;    
        var elId = itmp.toString();

        if(itmp > 0){          
          var elIdinput = (itmp+101).toString();
          var str = document.getElementById(elIdinput).value;          
          var newValue ={
            rindex: itmp,
            rname: str
          };
          Socket.send(JSON.stringify(newValue));
        }

        else{                   
          var str = document.getElementById('101').value;
          var newValue ={
            rindex: itmp,
            rname: str
          };
          Socket.send(JSON.stringify(newValue));
        }

      }  
  function toggleCheckbox(element) {
  var xhr = new XMLHttpRequest();
  if(element.checked){ xhr.open("GET", "/update?relay="+element.id+"&state=1", true); }
  else { xhr.open("GET", "/update?relay="+element.id+"&state=0", true); }
  xhr.send(); 
  }
  window.onload=function(event){init();}
</script>
</body>
</html>
)rawliteral";

// Replaces placeholder with button section in your web page
String processor(const String& var){
  //Serial.println(var);
  if(var == "BUTTONPLACEHOLDER"){    
    String buttons ="";
    String aId = "";
    String bId = "";
   for(int i=1; i<=NUM_RELAYS; i++){ 
      String relayStateValue = relayState(i);
      String v4input = rNames[i-1];
      String switchButton = "<label class=\"switch\"><input type=\"checkbox\" onchange=\"toggleCheckbox(this)\" id=\"" + String(i) + "\" "+ relayStateValue +"><span class=\"slider\"></span></label>";

      String lbName = "<span><input id=\""+String(i+100)+"\" value=\" "+ v4input +" \"rows=\"1\" cols=\"18\" class=\"changeNameinput\" contenteditable=\"true\"></span>";
      String btnSave = "<span><button class=\"saveNamebtn\" id=\""+String((i+10))+"\" onclick=\"button_send_back(this)\">Save</button></span>";
      String mdiv = "<div>" +lbName+ btnSave+"</div>";
      
      buttons+= "<h4>Relay #" + String(i) + " - GPIO " + relayGPIOs[i-1] + "</h4>"+ switchButton + mdiv;
    }
    return buttons;
  }
  return String();
}
String relayState(int numRelay){
  if(RELAY_NO){
    if(digitalRead(relayGPIOs[numRelay-1])){ //-1
      return "";
    }
    else {
      return "checked";
    }
  }
  else {
    if(digitalRead(relayGPIOs[numRelay-1])){ //-1
      return "checked";
    }
    else {
      return "";
    }
  }
  return "";
}
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void setup(){
  // Serial port for debugging purposes
  Serial.begin(115200);
  
  prefs.begin("relayNames", false);
    size_t schLen = prefs.getBytes("aufzeichnungen", NULL, NULL);
    char buffer[schLen];
    prefs.getBytes("aufzeichnungen", buffer, schLen);
    memcpy(temp_array, buffer, schLen);
  
    for(size_t i = 0; i < 8; i++){
      int ind = temp_array[i].rindex;
      String val = temp_array[i].rname;
      rNames[ind] = val;
      String temp = rNames[i];
      Serial.println(String(temp_array[i].rindex));
      Serial.println(String(temp_array[i].rname));
      Serial.println("I am Array value ->"+temp);
      delay(200);
    }
  prefs.end();

  // Set all relays to off when the program starts - if set to Normally Open (NO), the relay is off when you set the relay to HIGH
  for(int i=1; i<=NUM_RELAYS; i++){
    pinMode(relayGPIOs[i-1], OUTPUT);
    if(RELAY_NO){
      digitalWrite(relayGPIOs[i-1], HIGH);
    }
    else{
      digitalWrite(relayGPIOs[i-1], LOW);
    }
  }
  // Config WiFi
  WiFi.config(local_IP, gateway, subnet);  
    // Connect to Wi-Fi
  WiFi.begin(ssid, password); 
  int timeout_counter = 0;
  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
    timeout_counter++;
        if(timeout_counter >= CONNECTION_TIMEOUT*3){
        ESP.restart();        
    }
    Serial.println("Connecting to WiFi..");
  }

  // Print ESP32 Local IP Address
  Serial.println(WiFi.localIP());
  
  // Route for root / web page
  server.on("/", HTTP_GET, [](AsyncWebServerRequest *request){
    request->send_P(200, "text/html", index_html, processor);
  });

  // Send a GET request to <ESP_IP>/update?relay=<inputMessage>&state=<inputMessage2>
  server.on("/update", HTTP_GET, [] (AsyncWebServerRequest *request) {
    String inputMessage;
    String inputParam;
    String inputMessage2;
    String inputParam2;
    // GET input1 value on <ESP_IP>/update?relay=<inputMessage>
    if (request->hasParam(PARAM_INPUT_1) & request->hasParam(PARAM_INPUT_2)) {
      inputMessage = request->getParam(PARAM_INPUT_1)->value();
      inputParam = PARAM_INPUT_1;
      inputMessage2 = request->getParam(PARAM_INPUT_2)->value();
      inputParam2 = PARAM_INPUT_2;
      if(RELAY_NO){
        Serial.print("NO ");
        digitalWrite(relayGPIOs[inputMessage.toInt()-1], !inputMessage2.toInt());
      }
      else{
        Serial.print("NC ");
        digitalWrite(relayGPIOs[inputMessage.toInt()-1], inputMessage2.toInt());
      }
    }
    else {
      inputMessage = "No message sent";
      inputParam = "none";
    }
    Serial.println(inputMessage + inputMessage2);
    request->send(200, "text/plain", "OK");
  });  
  server.begin();   
  webSocket.begin();
  webSocket.onEvent(WebSocketEvent);   
   
  //server.on("/", HTTP_GET, [](AsyncWebServerRequest *request){
  //  request->send_P(200, "text/html", index_html, processor);
  //});
  //////////////////////////////////////////////////////////////////////////////////////     
} 

void loop() {  
  webSocket.loop();
}

void WebSocketEvent(byte num, WStype_t type, uint8_t* payload, size_t length){
    switch(type) {
      case WStype_DISCONNECTED:
      Serial.println("Client Disconnected");
      break;
      case WStype_CONNECTED:
      Serial.println("Client Connected");

      break;
      case WStype_TEXT:
      DeserializationError error = deserializeJson(doc_rx, payload);
      if(error){
        Serial.println("deserializeJson() failed");
        return;
      }
      else{
        const int r_index = doc_rx["rindex"];
        const char* r_name = doc_rx["rname"];
        Serial.println("Received info!");
        Serial.println("Index: " + String(r_index));
        Serial.println("Name: " + String(r_name));
        rNames[r_index] = String(r_name);
        prefs.begin("relayNames", false);
        for(size_t i = 0; i < 8; i++){
          Serial.println(String("I am from rNames"+rNames[i]));          
          temp_array[i].rindex=i;
          temp_array[i].rname=rNames[i];
        }
        prefs.putBytes("aufzeichnungen", &temp_array, sizeof(temp_array));
        prefs.end();
      }
       Serial.print("GG");
      break;
    }
}