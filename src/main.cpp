#ifdef ARDUINO_ARCH_ESP32
#include <WiFi.h>
#include <HTTPClient.h>
#elif defined(ARDUINO_ARCH_ESP8266)
#include <ESP8266WiFi.h>
#include <ESP8266HTTPClient.h>
#endif
#include <ArduinoJson.h>

// WiFi credentials
const char *ssid = "";
const char *password = "";

// API details
const String apiKey = "0000";
const String serverUrl = "IP";

float napiecie[3];
float prad[3];
float moc_czynna[3];
float moc_bierna[3];
float czestotliwosc[3];
float wspolczynnik_mocy[3];

void foxPobierzDane()
{
  if (WiFi.status() != WL_CONNECTED)
  {
    Serial.println("[WIFI] WiFi not connected!");
    return;
  }

  String url = serverUrl + apiKey + "/get_current_parameters";

  HTTPClient http;
#ifdef ARDUINO_ARCH_ESP32
  http.begin(url);
#elif defined(ARDUINO_ARCH_ESP8266)
  WiFiClient client;
  http.begin(client, url);
#endif

  int httpCode = http.GET();

  if (httpCode > 0)
  { // Check for valid response
    if (httpCode == HTTP_CODE_OK)
    {
      String payload = http.getString();

      // Parse JSON response
      JsonDocument jsonDoc;
      DeserializationError error = deserializeJson(jsonDoc, payload);

      if (error)
      {
        Serial.print("[FOX] JSON deserialization failed: ");
        Serial.println(error.c_str());
        return;
      }

      // Extract data
      JsonArray voltage = jsonDoc["voltage"];
      JsonArray current = jsonDoc["current"];
      JsonArray powerActive = jsonDoc["power_active"];
      JsonArray powerReactive = jsonDoc["power_reactive"];
      JsonArray frequency = jsonDoc["frequency"];
      JsonArray powerFactor = jsonDoc["power_factor"];

      for (int i = 0; i < 3; i++)
      {
        napiecie[i] = voltage[i];
        prad[i] = current[i];
        moc_czynna[i] = powerActive[i];
        moc_bierna[i] = powerReactive[i];
        czestotliwosc[i] = frequency[i];
        wspolczynnik_mocy[i] = powerFactor[i];
      }
    }
    else
    {
      Serial.printf("[FOX] HTTP GET failed with code: %d\n", httpCode);
    }
  }
  else
  {
    Serial.printf("[FOX] HTTP request failed: %s\n", http.errorToString(httpCode).c_str());
  }

  http.end();
#ifdef ARDUINO_ARCH_ESP8266
  client.stop();
#endif
}

void setup()
{
  Serial.begin(115200);

  WiFi.begin(ssid, password);
  Serial.print("[WIFI] Wifi...");
  while (WiFi.status() != WL_CONNECTED)
  {
    delay(1000);
    Serial.print(".");
  }
  Serial.println("\nconnected!");
}

void loop()
{
  foxPobierzDane();
  String msg = "";
  msg += "V:  " + String(napiecie[0]) + " " + String(napiecie[1]) + " " + String(napiecie[2]) + "\n";
  msg += "I:  " + String(prad[0]) + " " + String(prad[1]) + " " + String(prad[2]) + "\n";
  msg += "P:  " + String(moc_czynna[0]) + " " + String(moc_czynna[1]) + " " + String(moc_czynna[2]) + "\n";
  msg += "Q:  " + String(moc_bierna[0]) + " " + String(moc_bierna[1]) + " " + String(moc_bierna[2]) + "\n";
  msg += "F:  " + String(czestotliwosc[0]) + " " + String(czestotliwosc[1]) + " " + String(czestotliwosc[2]) + "\n";
  msg += "Pf: " + String(wspolczynnik_mocy[0]) + " " + String(wspolczynnik_mocy[1]) + " " + String(wspolczynnik_mocy[2]) + "\n\n";
  Serial.println(msg);
  delay(10000);
}
