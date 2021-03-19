
#include <ESP8266WiFi.h>
#include <ESP8266HTTPClient.h>
#include <WiFiClient.h>

const char* ssid = "Your SSID";
const char* password = "Your PASSWORD";

//Replace the organization and project in the url
String url = "https://vsrm.dev.azure.com/<organization>/<project>/_apis/release/releases?api-version=6.1-preview.8";
const char fingerprint[] PROGMEM = "61 66 D0 97 53 63 80 53 D0 FF E5 22 6A 89 9E 7E 3A 92 63 CA";

const int buttonPin = D1;         // the number of the pushbutton pin

int buttonState;             // the current reading from the input pin
int lastButtonState = HIGH;   // the previous reading from the input pin

unsigned long lastDebounceTime = 0;  // the last time the output pin was toggled
unsigned long debounceDelay = 50;    // the debounce time; increase if the output flickers

void setup() {
  
  pinMode(buttonPin, INPUT_PULLUP);

  Serial.begin(9600);
  Serial.println();
  Serial.print("connecting to ");
  Serial.println(ssid);
  WiFi.mode(WIFI_STA);
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
  // read the state of the switch into a local variable:
  int reading = digitalRead(buttonPin);

  if (reading != lastButtonState) {
    lastDebounceTime = millis();
  }

  if ((millis() - lastDebounceTime) > debounceDelay) {
    if (reading != buttonState) {
      buttonState = reading;

      if(reading == 0){
        Serial.println("Button Pressed");

          // Use WiFiClientSecure class to create TLS connection
          WiFiClientSecure client;
          Serial.printf("Using fingerprint '%s'\n", fingerprint);
          client.setFingerprint(fingerprint);
          
          HTTPClient http;

          http.begin(client, url); //HTTP
          http.addHeader("Content-Type", "application/json");

          //<PatPlaceholder> holds a Base64 encoded string containing: "username:PersonalAccessToken"
          http.addHeader("Authorization", "Basic <PatPlaceholder>");

          Serial.println("Executing POST request");
          //You may need to change the definitionId in this JSON string
          int httpCode = http.POST("{\r\n    \"definitionId\": 1,\r\n    \"description\": \"Release from button push\",\r\n    \"isDraft\": false,\r\n    \"reason\": \"none\",\r\n    \"manualEnvironments\": null\r\n}");
        
          Serial.println(httpCode);
      }
    }
  }

  // save the reading. Next time through the loop, it'll be the lastButtonState:
  lastButtonState = reading;
}
