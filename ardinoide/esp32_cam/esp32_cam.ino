#include "esp_camera.h"
#include <WiFi.h>
#include <ArduinoWebsockets.h>

#include "soc/soc.h" //disable brownout problems
#include "soc/rtc_cntl_reg.h" //disable brownout problems

#define PWDN_GPIO_NUM     32
#define RESET_GPIO_NUM    -1
#define XCLK_GPIO_NUM      0
#define SIOD_GPIO_NUM     26
#define SIOC_GPIO_NUM     27

#define Y9_GPIO_NUM       35
#define Y8_GPIO_NUM       34
#define Y7_GPIO_NUM       39
#define Y6_GPIO_NUM       36
#define Y5_GPIO_NUM       21
#define Y4_GPIO_NUM       19
#define Y3_GPIO_NUM       18
#define Y2_GPIO_NUM        5
#define VSYNC_GPIO_NUM    25
#define HREF_GPIO_NUM     23
#define PCLK_GPIO_NUM     22

const char* ssid = "Rock";
const char* password = "20212819";
const char* websocket_server_host = "ec2-3-111-213-252.ap-south-1.compute.amazonaws.com";
const uint16_t websocket_server_port = 8000;

using namespace websockets;
WebsocketsClient client;

// controll

const int motor1 = 12;
const int motor2 = 13;

const int motor4 = 15;
const int motor3 = 14;
void forward(){
  digitalWrite(motor1, HIGH);
  digitalWrite(motor2, LOW);
  digitalWrite(motor3, LOW);
  digitalWrite(motor4, HIGH);
}

void right(){
  digitalWrite(motor1, LOW);
  digitalWrite(motor2, HIGH);
  digitalWrite(motor3, LOW);
  digitalWrite(motor4, HIGH);

}

void left(){
  digitalWrite(motor1, HIGH);
  digitalWrite(motor2, LOW);
  digitalWrite(motor3, HIGH);
  digitalWrite(motor4, LOW);
}

void back(){
  digitalWrite(motor1, LOW);
  digitalWrite(motor2, HIGH);
  digitalWrite(motor3, HIGH);
  digitalWrite(motor4, LOW);
}

void stop() {
  digitalWrite(motor1, LOW);
  digitalWrite(motor2, LOW);
  digitalWrite(motor3, LOW);
  digitalWrite(motor4, LOW);
}

// end for controller



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
    if(data == "f") {
      forward();
      Serial.print("forward");   
    }
    else if(data == "b") {
      back();
      Serial.print("backward");   
    }
    else if(data == "l"){
      left();
      Serial.print("left");   
    }
    else if(data == "r"){
      right();
      Serial.print("right");   
    }
    else if(data == "x") {
      stop();
      Serial.print("stop");
    }
    Serial.print(data);
}

void setup() {
// controller pin
  pinMode(motor1, OUTPUT);
  pinMode(motor2, OUTPUT);
  pinMode(motor3, OUTPUT);
  pinMode(motor4, OUTPUT);
// end

  WRITE_PERI_REG(RTC_CNTL_BROWN_OUT_REG, 0);

  Serial.begin(115200);
  Serial.setDebugOutput(true);
  Serial.println();

  camera_config_t config;
  config.ledc_channel = LEDC_CHANNEL_0;
  config.ledc_timer = LEDC_TIMER_0;
  config.pin_d0 = Y2_GPIO_NUM;
  config.pin_d1 = Y3_GPIO_NUM;
  config.pin_d2 = Y4_GPIO_NUM;
  config.pin_d3 = Y5_GPIO_NUM;
  config.pin_d4 = Y6_GPIO_NUM;
  config.pin_d5 = Y7_GPIO_NUM;
  config.pin_d6 = Y8_GPIO_NUM;
  config.pin_d7 = Y9_GPIO_NUM;
  config.pin_xclk = XCLK_GPIO_NUM;
  config.pin_pclk = PCLK_GPIO_NUM;
  config.pin_vsync = VSYNC_GPIO_NUM;
  config.pin_href = HREF_GPIO_NUM;
  config.pin_sccb_sda = SIOD_GPIO_NUM;
  config.pin_sccb_scl = SIOC_GPIO_NUM;
  config.pin_pwdn = PWDN_GPIO_NUM;
  config.pin_reset = RESET_GPIO_NUM;
  config.xclk_freq_hz = 20000000;
  config.pixel_format = PIXFORMAT_JPEG; // for streaming
  //config.pixel_format = PIXFORMAT_RGB565; // for face detection/recognition

  config.frame_size = FRAMESIZE_HQVGA;
  // config.grab_mode = CAMERA_GRAB_WHEN_EMPTY;
  // config.fb_location = CAMERA_FB_IN_PSRAM;
  config.jpeg_quality = 15;
  config.fb_count = 2;

  // camera init
  esp_err_t err = esp_camera_init(&config);
  if (err != ESP_OK) {
    Serial.printf("Camera init failed with error 0x%x", err);
    return;
  }

  sensor_t * s = esp_camera_sensor_get();
  // initial sensors are flipped vertically and colors are a bit saturated
  // s->set_contrast(s, 0);   
  // s->set_raw_gma(s, 1);
  // s->set_framesize(s, FRAMESIZE_VGA);

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
  client.connect(websocket_server_host, websocket_server_port, "/");
  // while(){ 
  //   delay(500); 
    // esp_err_t err = esp_http_client_perform(httpClient);
    // Serial.print(err);
    // Serial.print(".");
  // }
  Serial.println("");
  Serial.println("Server connected");
}

void loop() {
  
  if (client.available()) {
    camera_fb_t *fb = esp_camera_fb_get();
    if(!fb) {
      esp_camera_fb_return(fb);
      return;
    }
    client.sendBinary((const char*) fb->buf, fb->len);
    esp_camera_fb_return(fb);
    client.poll();
  }
}
