#include <ESP8266WiFi.h>
#include <WebSocketsClient.h>

#include <Adafruit_MPU6050.h>
#include <Adafruit_Sensor.h>
#include <Wire.h>

Adafruit_MPU6050 mpu;

 //Robot Code: https://github.com/byronin/WiFi-MotorDriver/tree/main/Software/Arduino/Unipad-Control

const char *ssid =  "wb-sct";     
const char *pass =  "12345678";


WebSocketsClient webSocket;

WiFiClient client;

unsigned long messageInterval = 200;
bool connected = false;

#define DEBUG_SERIAL Serial

void webSocketEvent(WStype_t type, uint8_t * payload, size_t length) {
  switch (type) {
    case WStype_DISCONNECTED:
      DEBUG_SERIAL.printf("[WSc] Disconnected!\n");
      connected = false;
      break;
    case WStype_CONNECTED: {
        DEBUG_SERIAL.printf("[WSc] Connected to url: %s\n", payload);
        connected = true;

        // send message to server when Connected
        DEBUG_SERIAL.println("[WSc] SENT: Connected");
        webSocket.sendTXT("Connected");
      }
      break;
    case WStype_TEXT:
      DEBUG_SERIAL.printf("%s", payload);
      break;
    case WStype_BIN:
      DEBUG_SERIAL.printf("[WSc] get binary length: %u\n", length);
      hexdump(payload, length);
      break;
    case WStype_PING:
      // pong will be send automatically
      DEBUG_SERIAL.printf("[WSc] get ping\n");
      break;
    case WStype_PONG:
      // answer to a ping we send
      DEBUG_SERIAL.printf("[WSc] get pong\n");
      break;
  }
  //["Right"]
}
void setup()
{
  pinMode(15, OUTPUT);
  pinMode(13, OUTPUT);
  pinMode(12, OUTPUT);
  pinMode(16, OUTPUT);
  Serial.begin(115200);
  delay(10);

  Serial.println("Connecting to ");
  Serial.println(ssid);

  WiFi.begin(ssid, pass);
  while (WiFi.status() != WL_CONNECTED)
  {
    delay(150);
    Serial.print(".");
    digitalWrite(15, HIGH);
    digitalWrite(16, LOW);
    delay(150);
    digitalWrite(13, HIGH);
    digitalWrite(15, LOW);
    delay(150);
    digitalWrite(12, HIGH);
    digitalWrite(13, LOW);
    delay(150);
    digitalWrite(16, HIGH);
    digitalWrite(12, LOW);

  }
  digitalWrite(16, LOW);
  Serial.println("");
  Serial.println("WiFi connected");

  webSocket.begin("192.168.4.1", 8080, "/");

  // event handler
  webSocket.onEvent(webSocketEvent);
  pinMode(LED_BUILTIN, OUTPUT);

  if (!mpu.begin()) {
    Serial.println("Failed to find MPU6050 chip");
    while (1) {
      digitalWrite(16, HIGH);
      delay(250);
      digitalWrite(16, LOW);
      delay(250);
    }
  }
  Serial.println("MPU6050 Found!");
  mpu.setAccelerometerRange(MPU6050_RANGE_8_G);
  mpu.setGyroRange(MPU6050_RANGE_500_DEG);
  mpu.setFilterBandwidth(MPU6050_BAND_21_HZ);
  delay(100);
}

void loop()
{
  webSocket.loop();
  if (Serial.available() > 0) {
    webSocket.sendTXT(Serial.read());
  }

  sensors_event_t a, g, temp;
  mpu.getEvent(&a, &g, &temp);

  if (a.acceleration.y  < -5.0) {
    webSocket.sendTXT("[\"Up\"]");
    digitalWrite(13, HIGH);
  } else if (a.acceleration.y  > 5.0) {
    webSocket.sendTXT("[\"Down\"]");
    digitalWrite(12, HIGH);
  } else if (a.acceleration.x  < -5.0) {
    webSocket.sendTXT("[\"Right\"]");
    digitalWrite(15, HIGH);
  } else if (a.acceleration.x  > 5.0) {
    webSocket.sendTXT("[\"Left\"]");
    digitalWrite(16, HIGH);
  } else {
    webSocket.sendTXT("[]");
    digitalWrite(15, LOW);
    digitalWrite(13, LOW);
    digitalWrite(12, LOW);
    digitalWrite(16, LOW);
  }

}
