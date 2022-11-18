
#include <WiFi.h>
#include <cstring>
#include <iostream>
#include <time.h>
#include <string.h>

#define LED 2
#define RELAY_IN 15

const long PASS     = 5829;

const char* ssid      = "MOTOA176";
const char* password  = "3eak9ekn25";

const char* host      = "www.timfoldy-porto.com";

WiFiClient client;
const int httpPort = 80;
String url = "http://www.timfoldy-porto.com/random/data2.txt";
String request;
int val = 0;
int prev_val = 0;
char cs_line[16];
bool debug = false;

const char* ntpServer = "pool.ntp.org";
const long  gmtOffset_sec = 0;
const int   daylightOffset_sec = 0;
time_t epoch;

void retrieveTime()
{
  struct tm timeinfo;
  if(!getLocalTime(&timeinfo)){
    Serial.println("Failed to obtain time");
    return;
  }
  epoch = mktime(&timeinfo);
}

void setup()
{
    Serial.begin(115200);
    delay(10);

    pinMode(LED, OUTPUT);
    pinMode(RELAY_IN, OUTPUT);

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

    configTime(gmtOffset_sec, daylightOffset_sec, ntpServer);
    retrieveTime();

    if (!client.connect(host, httpPort)) { Serial.println("connection failed"); }

    request = String("GET ") + url + " HTTP/1.1\r\n" +
                  "Host: " + host + "\r\n" +
                  "Connection: keep-alive\r\n\r\n";


    client.print(request);
}


void loop()
{
    if (debug) Serial.println ("stop 1");
    client.print(request);

    if (debug) Serial.println ("stop 2");
    unsigned long timeout = millis();
    while (client.available() == 0) { 
      if (millis() - timeout > 3000) {
            Serial.println(">>> Client Timeout !");
            client.stop();
            delay (1000);
            client.connect (host, httpPort);
            return;
      }
    }

    if (debug) Serial.println ("stop 3");
    while(client.available()) {
        String line = client.readStringUntil('\r');
        if (line.length() == 16) {
          std::string str(line.c_str());
          std::string s_code = str.substr (0,5);
          std::string s_time = str.substr (5,12);
          long l_code = std::stol (s_code);
          long l_time = std::stol (s_time);
          Serial.println (l_code);
          Serial.println (l_time);
          Serial.println (epoch);
            if (l_time == epoch || l_time == epoch-1) {
              if (l_code == PASS) {
                digitalWrite (LED, HIGH);
                digitalWrite (RELAY_IN, HIGH);
                delay (500);
                digitalWrite (LED, LOW);
                digitalWrite (RELAY_IN, LOW);
              }
            }
        }
    }

    retrieveTime();
}
