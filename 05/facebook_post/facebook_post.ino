#include <SPI.h>
#include <ESP8266WiFi.h>
#include <Temboo.h>
#include "TembooAccount.h" // Contains Temboo account information

WiFiClient client;

int numRuns = 1;   // Execution count, so this doesn't run forever
int maxRuns = 10;   // Maximum number of times the Choreo should be executed

void setup() {
  Serial.begin(9600);
  
  // For debugging, wait until the serial console is connected
  delay(4000);
  while(!Serial);

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
    Serial.println("Running Post - Run #" + String(numRuns++));

    TembooChoreo PostChoreo(client);

    // Invoke the Temboo client
    PostChoreo.begin();

    // Set Temboo account credentials
    PostChoreo.setAccountName(TEMBOO_ACCOUNT);
    PostChoreo.setAppKeyName(TEMBOO_APP_KEY_NAME);
    PostChoreo.setAppKey(TEMBOO_APP_KEY);

    // Set Choreo inputs
    String MessageValue = "A simple message from the ESP8266!";
    PostChoreo.addInput("Message", MessageValue);
    String AccessTokenValue = "token";
    PostChoreo.addInput("AccessToken", AccessTokenValue);

    // Identify the Choreo to run
    PostChoreo.setChoreo("/Library/Facebook/Publishing/Post");

    // Run the Choreo; when results are available, print them to serial
    PostChoreo.run();

    while(PostChoreo.available()) {
      char c = PostChoreo.read();
      Serial.print(c);
    }
    PostChoreo.close();
  }

  Serial.println("\nWaiting...\n");
  delay(30000); // wait 30 seconds between Post calls
}
