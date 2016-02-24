#include <SPI.h>
#include <ESP8266WiFi.h>
#include <Temboo.h>
#include "TembooAccount.h" // Contains Temboo account information
#include "DHT.h"

WiFiClient client;

// DHT11 sensor pins
#define DHTPIN 5
#define DHTTYPE DHT11

// Initialize DHT sensor
DHT dht(DHTPIN, DHTTYPE, 15);

int numRuns = 1;   // Execution count, so this doesn't run forever
int maxRuns = 10;   // Maximum number of times the Choreo should be executed

void setup() {
  Serial.begin(115200);
  
  // For debugging, wait until the serial console is connected
  delay(4000);
  while(!Serial);

  // Init DHT 
  dht.begin();

  int wifiStatus = WL_IDLE_STATUS;

  // Determine if the WiFi Shield is present
  Serial.print("\n\nShield:");
  if (WiFi.status() == WL_NO_SHIELD) {
    Serial.println("FAIL");

    // If there's no WiFi shield, stop here
    while(true);
  }

  Serial.println("OK");

  // Try to connect to the local WiFi network
  while(wifiStatus != WL_CONNECTED) {
    Serial.print("WiFi:");
    wifiStatus = WiFi.begin(WIFI_SSID, WPA_PASSWORD);

    if (wifiStatus == WL_CONNECTED) {
      Serial.println("OK");
    } else {
      Serial.println("FAIL");
    }
    delay(5000);
  }

  Serial.println("Setup complete.\n");
}

void loop() {
  
  if (numRuns <= maxRuns) {
    Serial.println("Running StatusesUpdate - Run #" + String(numRuns++));

    // Read data
    float h = dht.readHumidity();
    float t = dht.readTemperature();

    TembooChoreo StatusesUpdateChoreo(client);

    // Invoke the Temboo client
    StatusesUpdateChoreo.begin();

    // Set Temboo account credentials
    StatusesUpdateChoreo.setAccountName(TEMBOO_ACCOUNT);
    StatusesUpdateChoreo.setAppKeyName(TEMBOO_APP_KEY_NAME);
    StatusesUpdateChoreo.setAppKey(TEMBOO_APP_KEY);

    // Set Choreo inputs
    String StatusUpdateValue = "The temperature is " + String(t) + " and the humidity is " + String(h) + ".";
    StatusesUpdateChoreo.addInput("StatusUpdate", StatusUpdateValue);
    String AccessTokenValue = "81862926-onpF7S0Na3JcjOsYr83IX2fBTYTQqY9pWW4VvVEuj";
    StatusesUpdateChoreo.addInput("AccessToken", AccessTokenValue);
    String ConsumerKeyValue = "8LW85ikOltE3f1RT105gDs6Pd";
    StatusesUpdateChoreo.addInput("ConsumerKey", ConsumerKeyValue);
    String ConsumerSecretValue = "43IDipZ9dnPFz45jJmKStdtdvTpuy619Xcn6UerB5MxvlWVdoB";
    StatusesUpdateChoreo.addInput("ConsumerSecret", ConsumerSecretValue);
    String AccessTokenSecretValue = "Bv0cfPjBE1RQDDvW1EZBbE7XFbnFUDKoveAu8C8s3Qztt";
    StatusesUpdateChoreo.addInput("AccessTokenSecret", AccessTokenSecretValue);

    // Identify the Choreo to run
    StatusesUpdateChoreo.setChoreo("/Library/Twitter/Tweets/StatusesUpdate");

    // Run the Choreo; when results are available, print them to serial
    StatusesUpdateChoreo.run();

    while(StatusesUpdateChoreo.available()) {
      char c = StatusesUpdateChoreo.read();
      Serial.print(c);
    }
    StatusesUpdateChoreo.close();
  }

  Serial.println("\nWaiting...\n");
  delay(30000); // wait 30 seconds between StatusesUpdate calls
}
