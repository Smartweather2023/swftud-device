#include <ArduinoJson.h>
#include <ctime>
#include <ESP8266HTTPClient.h>
#include <ESP8266WiFi.h>
#include <WiFiClientSecure.h>

const char *ssid = "";
const char *password = "";
const char *server = "smart-weather-station-ft-ud-default-rtdb.firebaseio.com";
const int httpsPort = 443;
const char *endpoint = "/data/FT002.json?auth=";
// openssl s_client -connect smart-weather-station-ft-ud-default-rtdb.firebaseio.com:443 < /dev/null | openssl x509 -noout -fingerprint -sha1
const char *fingerprint = "";
const char* ntpServer = "pool.ntp.org";

WiFiClientSecure client;
HTTPClient https;

unsigned long getTime() {
  time_t now;
  struct tm timeinfo;
  if (!getLocalTime(&timeinfo)) {
    return(0);
  }
  time(&now);
  return now;
}

String buildJsonData(int date, float carbonGas, float pm10Particles, float precipitation, float relativeHumidity, float solarRadation, float temperature, float wind) {
  String stringDate = String(date);
  StaticJsonDocument<300> doc;
  JsonObject data = doc.createNestedObject(stringDate);

  data["carbonGas"] = carbonGas;
  data["date"] = date;
  data["pm10Particles"] = pm10Particles;
  data["precipitation"] = precipitation;
  data["relativeHumidity"] = relativeHumidity;
  data["solarRadation"] = solarRadation;
  data["temperature"] = temperature;
  data["wind"] = wind;

  String serializedData;
  serializeJson(doc, serializedData);

  return serializedData;
}

void sendWeatherMeasure(float carbonGas, float pm10Particles, float precipitation, float relativeHumidity, float solarRadation, float temperature, float wind) {
  unsigned long timestamp = getTime();
  int timestampInt = static_cast<int>(timestamp);
  client.setFingerprint(fingerprint);
  https.begin(client, server, httpsPort, endpoint);
  https.addHeader("Content-Type", "application/json");

  StaticJsonDocument<200> data;
  String jsonData = buildJsonData(timestampInt, carbonGas, pm10Particles, precipitation, relativeHumidity, solarRadation, temperature, wind);

  Serial.print("JSON data: ");
  Serial.println(jsonData);

  int httpResponseCode = https.PATCH(jsonData);
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

void setup() {
  Serial.begin(115200);
  delay(1000);

  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    Serial.println("Connecting to WiFi...");
    delay(1000);
  }

  Serial.println("Connected to WiFi");
  configTime(0, 0, ntpServer);

  // Move to loop
  sendWeatherMeasure(6.66, 6.66, 6.66, 6.66, 6.66, 6.66, 6.66);
}

void loop() {}