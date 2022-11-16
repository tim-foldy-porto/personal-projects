
#include <WiFi.h>
#include <cstring>
#include <iostream>

#define LED 2
#define RELAY_IN 15

const char* ssid     = "MOTOA176";
const char* password = "3eak9ekn25";

const char* host = "www.timfoldy-porto.com";

WiFiClient client;
const int httpPort = 80;
String url = "http://www.timfoldy-porto.com/random/data2.txt";
String request;
int val = 0;
int prev_val = 0;
char str[3];

void setup()
{
    Serial.begin(115200);
    delay(10);

    pinMode(LED, OUTPUT);
    pinMode(RELAY_IN, OUTPUT);

    // We start by connecting to a WiFi network

    Serial.println();
    Serial.println();
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

    if (!client.connect(host, httpPort)) { Serial.println("connection failed"); }

    request = String("GET ") + url + " HTTP/1.1\r\n" +
                  "Host: " + host + "\r\n" +
                  "Connection: keep-alive\r\n\r\n";


    client.print(request);
}


void loop()
{
    Serial.println ("stop 1");
    client.print(request);

    Serial.println ("stop 2");
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

    Serial.println ("stop 3");
    while(client.available()) {
        String line = client.readStringUntil('\r');
        if (line.length() == 2) {
          prev_val = val;
          strcpy(str, line.c_str());
          val = atoi(str);
          Serial.print(val);
          Serial.print('\t');
          Serial.print(val == 3);
          Serial.print('\t');
          Serial.print(prev_val == 2);
          Serial.print('\t');
          Serial.print(val == 3 && prev_val == 2);
          Serial.print('\n');
          if (val == 3 && prev_val == 2) {
            digitalWrite (LED, HIGH);
            digitalWrite (RELAY_IN, HIGH);
          }
          else {
            digitalWrite (LED, LOW);
            digitalWrite (RELAY_IN, LOW);
          }
        }
    }
}
