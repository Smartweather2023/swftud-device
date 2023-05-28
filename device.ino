#include <ArduinoJson.h>
#include <ctime>
#include <ESP8266HTTPClient.h>
#include <ESP8266WiFi.h>
#include <WiFiClientSecure.h>

// Device location
#ID 1
#LAT 4.60971
#LON -74.08175

const char *ssid = "";
const char *password = "";

const char *server = "smart-weather-station-ft-ud-default-rtdb.firebaseio.com";

const int httpsPort = 443;

//const char *endpoint* = "/prueba.json?auth=ACA_VA_EL_TOKEN_DE_FIREBASE; prueba es el nombre de la base de datos
const char *endpoint = "/smwfud.json?auth="; //  ACA_VA_EL_TOKEN_DE_FIREBASE despues del signo de igual

// openssl s_client -connect smart-weather-station-ft-ud-default-rtdb.firebaseio.com:443 < /dev/null | openssl x509 -noout -fingerprint -sha1
const char *fingerprint = "91144184c3f8489d29568cd43543f6b853f1fefe";

String getData(int ID, time_t now, float temperature, float humidity, float pressure, float speed, float gas, float precipitation, float LAT, float LON) {
  // Create a JSON object
  StaticJsonDocument<200> data;
  data["id"] = ID;
  data["date"] = now;
  data["temperature"] = temperature;
  data["humidity"] = humidity;
  data["pressure"] = pressure;
  data["speed"] = speed;
  data["gas"] = gas;
  data["precipitation"] = precipitation;
  data["lat"] = LAT;
  data["lon"] = LON;

  // Serialize the JSON object to a string
  String jsonData;
  serializeJson(data, jsonData);

  // Return the JSON string
  return jsonData;
}

void setup() {
  Serial.begin(115200);
  delay(1000);

  WiFi.begin(ssid, password);

  while (WiFi.status() != WL_CONNECTED) {
    Serial.println("Connecting to WiFi...");
    delay(1000);
  }
  Serial.println("Connected to WiFi");

  WiFiClientSecure client;
  HTTPClient https;

  client.setFingerprint(fingerprint);
  https.begin(client, server, httpsPort, endpoint);
  https.addHeader("Content-Type", "application/json");

  StaticJsonDocument<200> data;
  String jsonData = getData(1, 1620000000, 25.5, 50.5, 1000.5, 10.5, 100.5, 0.5, 4.60971, -74.08175);
  
  int httpResponseCode = https.POST(jsonData);
  if (httpResponseCode > 0) {
    Serial.print("HTTP response code: ");
    Serial.println(httpResponseCode);
    String response = https.getString();
    Serial.print("Response: ");
    Serial.println(response);
  } else {
    Serial.print("Error in HTTP request: ");
    Serial.println(httpResponseCode);
  }

  https.end();
}



void loop() {}