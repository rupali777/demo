#include <SimpleDHT.h>

#include <Adafruit_CircuitPlayground.h>
#include <Adafruit_Circuit_Playground.h>

#include <Adafruit_ESP8266.h>


#include <ArduinoJson.h>
#include <ESP8266WiFi.h>
#include <ESP8266WebServer.h>
#include <DHT.h>
#define DHTTYPE   DHT11
#define DHTPIN    13

#define WLAN_SSID     "Ashwini"
#define WLAN_PASSWORD "12345678"

DHT dht(DHTPIN, DHTTYPE, 11);
ESP8266WebServer server(80);

float temperature, humidity;
unsigned long previousMillis = 0;
const long interval = 2000;

void setup() {
    Serial.begin(115200);
    delay(10);

    dht.begin();
    humidity = dht.readHumidity();
    temperature = dht.readTemperature();

    Serial.println(); Serial.println();
    Serial.print("Connecting to ");
    Serial.println(WLAN_SSID);

    WiFi.mode(WIFI_STA);
    WiFi.begin(WLAN_SSID, WLAN_PASSWORD);
    while (WiFi.status() != WL_CONNECTED) {
        delay(500);
        Serial.print(".");
    }
    Serial.println();

    Serial.println("WiFi connected");
    Serial.println("IP address: "); Serial.println(WiFi.localIP());

    server.on("/dht11", HTTP_GET, [](){
        unsigned long currentMillis = millis();

        if (currentMillis - previousMillis >= interval) {
            previousMillis = currentMillis;

            humidity = dht.readHumidity();
            temperature = dht.readTemperature();
            if (isnan(humidity) || isnan(temperature)) {
                Serial.println("Failed to read from DHT sensor!");
                return;
            }
        }

        String webString = "Humiditiy " + String((int)humidity) + "%   Temperature: " + String((int)temperature) + " C";
        Serial.println(webString);
        server.send(200, "text/plain", webString);
    });
    server.on("/dht11.json", [](){
        unsigned long currentMillis = millis();

        if (currentMillis - previousMillis >= interval) {
            previousMillis = currentMillis;

            humidity = dht.readHumidity();
            temperature = dht.readTemperature();

            if (isnan(humidity) || isnan(temperature)) {
                Serial.println("Failed to read from DHT sensor!");
                return;
            }

            Serial.println("Reporting " + String((int)temperature) + "C and " + String((int)humidity) + " % humidity");
        }

        StaticJsonBuffer<500> jsonBuffer;
        JsonObject& root = jsonBuffer.createObject();
        root["temperature"] = temperature;
        root["humidity"] = humidity;

        String jsonString;
        root.printTo(jsonString);

        Serial.println(jsonString);
        server.send(200, "application/json", jsonString);
    });

    server.begin();
    Serial.println("HTTP server started! Waiting for clients!");
}

void loop() {
    server.handleClient();
}
