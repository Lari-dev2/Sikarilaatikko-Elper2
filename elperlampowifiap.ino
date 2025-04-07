#include <WiFi.h>
#include <WebServer.h>

const char* ssid = "ESP32_AP";
const char* password = "12345678"; 

WebServer server(80);

const int tempPin = 34; 

void handleRoot() {
  int analogValue = analogRead(tempPin);
  
  float voltage = analogValue * (3.3 / 4095.0);
  float temperatureC = (0.5 - voltage) * 100.0;  
  
 
  String html = "<!DOCTYPE html><html><head>";
  html += "<meta charset=\"UTF-8\">"; 
  html += "<meta name=\"viewport\" content=\"width=device-width, initial-scale=1\">";
  html += "<meta http-equiv=\"refresh\" content=\"5\">";  
  html += "<style>body { font-family: Arial; text-align: center; margin-top: 50px; }";
  html += "h1 { color: #444; }</style></head><body>";
  html += "<h1>Lämpötila: " + String(temperatureC, 1) + " °C</h1>";
  html += "</body></html>";
  
  server.send(200, "text/html", html);
}

void setup() {
  Serial.begin(115200);
  

  pinMode(tempPin, INPUT);
  
 
  Serial.println("Setting up AP...");
  WiFi.softAP(ssid, password);
  
  Serial.print("AP IP address: ");
  Serial.println(WiFi.softAPIP());
  
  // Määritä serverin reitit
  server.on("/", handleRoot);
  
  // Aloita serveri
  server.begin();
  Serial.println("HTTP server started");
}

void loop() {
  server.handleClient();
  delay(2); 
}