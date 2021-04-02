/*----------- WIFI libraries and setup -----------*/
#include <ESP8266WiFi.h>
#include <WiFiClientSecure.h> 
#include <ESP8266WebServer.h>
#include <ESP8266HTTPClient.h>

const char *ssid = "TMNL";
const char *password = "Buddy1970";

// const char *ssid = "Tesla IoT";
// const char *password = "fsL6HgjN";


/*----------- Website info -----------*/
const char *host = "jmnvapp.azurewebsites.net";
const int port = 443;

// Fingerprint of certificate in browser
const char fingerprint[] PROGMEM = "8b00830bccf246f796d38eb5e12fcfbd1d3a6150";


/*----------- In/ouput pins ----------- */
#define ldrPin A0
#define ledPin 5


/*----------- Variables for sending data to server ----------- */
String data;
String postVariableValue = "brightness=";
String ldrValue;
bool light = false;


void setup() {
  // Start serial monitor and set the led pin to output
  delay(1000);
  Serial.begin(115200);
  pinMode(ledPin, OUTPUT);

  WiFi.mode(WIFI_OFF);                     // Prevents reconnection issue (taking too long to connect)
  delay(1000);
  WiFi.mode(WIFI_STA);                     // Set wifi to station mode
  

  /*----------- Connect to wifi ----------- */
  WiFi.begin(ssid, password);
  // Wait for connection
  Serial.print("Connecting");
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }

  // Print info when connected
  Serial.println("Connected to '" + String(ssid) + "' as: " + WiFi.localIP().toString().c_str());
}


void loop() {
  // Read ldr value and prepare data variable
  ldrValue = analogRead(ldrPin);
  data = postVariableValue + ldrValue;

  // Send the get request to server
  getRequest();

  // Turn on light if needed
  if (light){
    digitalWrite(ledPin, HIGH);
  } else {
    digitalWrite(ledPin, LOW);
  }
  delay(1000);
}


void getRequest(){
  WiFiClientSecure httpsClient;
  httpsClient.setFingerprint(fingerprint);
  httpsClient.setTimeout(15000);
  delay(1000);
  
  // Connect to the host (maximum of 30 failed tries)
  int r = 0;
  Serial.printf("\n\n\nConnecting to '%s:%i' using fingerprint '%s'.", host, port, fingerprint);
  while((!httpsClient.connect(host, port)) && (r < 30)){
      delay(100);
      Serial.print(".");
      r++;
  }

  if(r==30) {
    Serial.println(" Connection failed");
  } else {
    Serial.println(" Connected to web");
  }
  
  // Prepare link with data
  String address = "/api/input?" + data;

  Serial.print("Requesting URL: ");
  Serial.println(host + address);

  // Send the get request to the website
  httpsClient.print(String("GET ") + address + " HTTP/1.1\r\n" +
               "Host: " + host + "\r\n" + 
               "Connection: close\r\n\r\n");

  // Read reply line by line 
  Serial.println("Request sent, reply was:");
  Serial.println("==========");
  String line = "";
  while (httpsClient.available()){        
    line += httpsClient.readStringUntil('\r');
  }

  // Check if the response has the words "turn light ON" in it
  if (line != ""){
    if (line.indexOf("turn light ON") != -1){
      light = true;
    } else {
      light = false;
    }
  }

  // Print response
  Serial.println(line);
  Serial.println("==========");
  Serial.println("closing connection");
}