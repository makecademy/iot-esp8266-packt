// Libraries
#include <ESP8266WiFi.h>
#include <ArduinoJson.h>
#include <Wire.h>
#include "SSD1306.h"

// Pins
#define SDA 14
#define SCL 12
#define I2C 0x3D

// Create display
SSD1306 display(I2C, SDA, SCL);

// LED pins
#define LED_PIN_UP 4
#define LED_PIN_DOWN 5

// Previous Bitcoin value & threshold
float previousValue = 0.0;
float threshold = 0.05;

// WiFi settings
const char* ssid     = "wifi-network";
const char* password = "wif-password";

// API server
const char* host = "api.coindesk.com";

void setup() {

  // Serial
  Serial.begin(115200);
  delay(10);

   // Initialize display
  display.init();
  display.flipScreenVertically();
  display.clear();
  display.display();

  // LED pins as output
  pinMode(LED_PIN_DOWN, OUTPUT);
  pinMode(LED_PIN_UP, OUTPUT);

  // We start by connecting to a WiFi network
  Serial.println();
  Serial.println();
  Serial.print("Connecting to ");
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

void loop() {

  // Connect to API
  Serial.print("connecting to ");
  Serial.println(host);
  
  // Use WiFiClient class to create TCP connections
  WiFiClient client;
  const int httpPort = 80;
  if (!client.connect(host, httpPort)) {
    Serial.println("connection failed");
    return;
  }
  
  // We now create a URI for the request
  String url = "/v1/bpi/currentprice.json";
  
  Serial.print("Requesting URL: ");
  Serial.println(url);
  
  // This will send the request to the server
  client.print(String("GET ") + url + " HTTP/1.1\r\n" +
               "Host: " + host + "\r\n" + 
               "Connection: close\r\n\r\n");
  delay(100);
  
  // Read all the lines of the reply from server and print them to Serial
  String answer;
  while(client.available()){
    String line = client.readStringUntil('\r');
    answer += line;
  }

  client.stop();
  Serial.println();
  Serial.println("closing connection");

  // Process answer
  Serial.println();
  Serial.println("Answer: ");
  Serial.println(answer);

  // Convert to JSON
  String jsonAnswer;
  int jsonIndex;

  for (int i = 0; i < answer.length(); i++) {
    if (answer[i] == '{') {
      jsonIndex = i;
      break;
    }
  }

  // Get JSON data
  jsonAnswer = answer.substring(jsonIndex);
  Serial.println();
  Serial.println("JSON answer: ");
  Serial.println(jsonAnswer);
  jsonAnswer.trim();

  // Get rate as float
  int rateIndex = jsonAnswer.indexOf("rate_float");
  String priceString = jsonAnswer.substring(rateIndex + 12, rateIndex + 18);
  priceString.trim();
  float price = priceString.toFloat();

  // Print price
  Serial.println();
  Serial.println("Bitcoin price: ");
  Serial.println(price);

  // Display on OLED
  display.clear();
  display.setFont(ArialMT_Plain_24);
  display.drawString(26, 20, priceString);
  display.display();

  // Init previous value 
  if (previousValue == 0.0) {
    previousValue = price;
  }

  // Alert down ?
  if (price < (previousValue - threshold)) {

    // Flash LED
    digitalWrite(LED_PIN_DOWN, HIGH);
    delay(100);
    digitalWrite(LED_PIN_DOWN, LOW);
    delay(100);
    digitalWrite(LED_PIN_DOWN, HIGH);
    delay(100);
    digitalWrite(LED_PIN_DOWN, LOW);
    
  }

  // Alert up ?
  if (price > (previousValue + threshold)) {

    // Flash LED
    digitalWrite(LED_PIN_UP, HIGH);
    delay(100);
    digitalWrite(LED_PIN_UP, LOW);
    delay(100);
    digitalWrite(LED_PIN_UP, HIGH);
    delay(100);
    digitalWrite(LED_PIN_UP, LOW);
    
  }

  // Store value
  previousValue = price;

  // Wait 5 seconds
  delay(5000);
}

