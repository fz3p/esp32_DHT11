// Example testing sketch for various DHT humidity/temperature sensors written by ladyada
// REQUIRES the following Arduino libraries:
// - DHT Sensor Library: https://github.com/adafruit/DHT-sensor-library
// - Adafruit Unified Sensor Lib: https://github.com/adafruit/Adafruit_Sensor

#include <DHT.h>
#include <WiFi.h>
#include <HTTPClient.h>

//connexion wifi
const char* ssid = "";
const char* password =  "";


//connexion domoticz
const char* host = "";
//pour la base64 https://andrologiciels.wordpress.com/materiels-android/esp8266/arduino-esp-domoticz/
const char* user =  "";
const char* pass = "";
const int   port = 8080;
const int   watchdog = 60000; // Fréquence d'envoi des données à Domoticz 
unsigned long previousMillis = millis(); 
HTTPClient http;

#define DHTPIN 4  

#define DHTTYPE DHT11   // DHT 11
//#define DHTTYPE DHT22   // DHT 22  (AM2302), AM2321
//#define DHTTYPE DHT21   // DHT 21 (AM2301)

DHT dht(DHTPIN, DHTTYPE);

void setup() {
  Serial.begin(115200);
  Serial.println(F("DHT11 test!"));
  dht.begin();
  
  connectToNetwork(); 
}

void loop() {
  delay(30000);

  float h = dht.readHumidity();
  float t = dht.readTemperature();
  float f = dht.readTemperature(true);

  // Check if any reads failed and exit early (to try again).
  if (isnan(h) || isnan(t) || isnan(f)) {
    Serial.println(F("Failed to read from DHT sensor!"));
    return;
  }

  // Compute heat index in Fahrenheit (the default)
  float hif = dht.computeHeatIndex(f, h);
  // Compute heat index in Celsius (isFahreheit = false)
  float hic = dht.computeHeatIndex(t, h, false);

  Serial.print(F("Humidity: "));
  Serial.print(h);
  Serial.print(F("%  Temperature: "));
  Serial.print(t);
  Serial.print(F("°C Heat index: "));
  Serial.print(hic);
  Serial.print(F("°C "));


  if(WiFi.status() != WL_CONNECTED) {
      Serial.println("WiFi not connected !");
    } else {  
      Serial.println("Send data to Domoticz");
      String url = "/json.htm?username=";
        url += String(user);
        url += "&password=";
        url += String(pass);
        url += "&type=command&param=udevice&idx=3&nvalue=0&svalue=";
        url += String(hic); 
        url += ";";
        url += String(h); 
        url += ";1;";
        sendDomoticz(url);
    }
}


void sendDomoticz(String url){
  Serial.print("connecting to ");
  Serial.println(host);
  Serial.print("Requesting URL: ");
  Serial.println(url);

  String complete_url = "http://";
  complete_url += String(host);
  complete_url += ":";
  complete_url += String(port);
  complete_url += String(url); 
  http.begin(host,port,url);
  int httpCode = http.GET();
    if (httpCode) {
      if (httpCode == 200) {
        String payload = http.getString();
        Serial.println("Domoticz response "); 
        Serial.println(payload);
      }else{
        String payload = http.getString();
        Serial.println("Domoticz not response ");
        Serial.println(String(httpCode));
        // pour interprétation des codes d'erreur : https://github.com/espressif/arduino-esp32/blob/master/libraries/HTTPClient/src/HTTPClient.h
        Serial.println(complete_url);
      }
    }
  Serial.println("closing connection");
  http.end();
}

void connectToNetwork() {
  WiFi.begin(ssid, password);
 
  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
    Serial.println("Establishing connection to WiFi..");
  }
 
  Serial.println("Connected to network");
 
}
