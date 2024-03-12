#include <ESP8266WiFi.h>
#include <ArduinoWebsockets.h>

const char* ssid = "NSUT-Campus";
const char* password = NULL;
const char* websocket_server_host = "ws://ec2-43-204-229-175.ap-south-1.compute.amazonaws.com:8000/";

using namespace websockets;
WebsocketsClient client;
bool on = false;

void onEventsCallback(WebsocketsEvent event, String data) {
    if(event == WebsocketsEvent::ConnectionOpened) {
        Serial.println("Connection Opened");
    } else if(event == WebsocketsEvent::ConnectionClosed) {
        Serial.println("Connection Closed");
        ESP.restart();
    } else if(event == WebsocketsEvent::GotPing) {
        Serial.println("Got a Ping!");
    } else if(event == WebsocketsEvent::GotPong) {
        Serial.println("Got a Pong!");
    }
}

void onMessageCallback(WebsocketsMessage message) {
    String data = message.data();
    Serial.print(data);
    if(on)
      digitalWrite(D1, LOW);
    else
      digitalWrite(D1, HIGH);
    on = !on;

}

void setup() {
  Serial.begin(115200);
  Serial.print("Started");
  pinMode(D1, OUTPUT);
  WiFi.begin(ssid, password);
  WiFi.setSleep(false);

  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("");
  Serial.println("WiFi connected");

  client.onMessage(onMessageCallback);
  client.onEvent(onEventsCallback);
  // while(
    client.connect(websocket_server_host);
    // ){ 
  //   delay(500); 
  //   Serial.print(".");
  // }
  Serial.println("");
  Serial.println("Server connected");
  client.send("Hello");
}

// the loop function runs over and over again forever
void loop() {
  client.poll();
}