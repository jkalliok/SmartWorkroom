#include <WiFi.h>

#include <HTTPClient.h>
#include <WiFiUdp.h>

#include <WakeOnLan.h>

WiFiUDP UDP;
WakeOnLan WOL(UDP);

const char* ssid     = "YOUR-SSID";
const char* password = "YOUR-PASSWORD";


bool pcRunning = false;
void wakeMyPC() {
    const char *MACAddress = "YOUR-MAC-ADDRESS";
    WOL.sendMagicPacket(MACAddress);
    sendRequest("{\"on\": true}");
     // Send Wake On Lan packet with the above MAC address. Default to port 9.
    // WOL.sendMagicPacket(MACAddress, 7); // Change the port number
}

void shutMyPC()
{
  sendRequest("{\"on\": false}");
  UDP.beginPacket("YOUR-IP-ADDRESS",4650);
  UDP.printf("SHUTDOWN");
  UDP.endPacket();
  delay(5000);
}

void sendRequest(char* message)
{
  Serial.println(message);
  if(WiFi.status()== WL_CONNECTED){
      HTTPClient http;
      
      // Your Domain name with URL path or IP address with path
      http.begin("http://HUE-BRIDGE-IP-ADDRESS/api/FhcWNuLdWIVVZV8oDxNoO3yePvu7X5-HZknOTqTt/lights/2/state");
      // Data to send with HTTP POST
      int httpResponseCode = http.PUT(message);
      
      /*
      // If you need an HTTP request with a content type: application/json, use the following:
      http.addHeader("Content-Type", "application/json");
      // JSON data to send with HTTP POST
      String httpRequestData = "{\"value1\":\"" + String(random(40)) + "\",\"value2\":\"" + String(random(40)) + "\",\"value3\":\"" + String(random(40)) + "\"}";
      // Send HTTP POST request
      int httpResponseCode = http.POST(httpRequestData);
      */
     
      Serial.print("HTTP Response code: ");
      Serial.println(httpResponseCode);
        
      // Free resources
      http.end();
    }
}
// defines pins numbers
const int trigPin = 4;
const int echoPin = 5;
// defines variables
long duration;
int distance;
int slerpDistance = 1000;

//int[] forAverage = int[10];
void setup() {
  pinMode(trigPin, OUTPUT); // Sets the trigPin as an Output
  pinMode(echoPin, INPUT); // Sets the echoPin as an Input
  Serial.begin(9600); // Starts the serial communication
  Serial.println("Starting");

  WOL.setRepeat(3, 100); // Optional, repeat the packet three times with 100ms between. WARNING delay() is used between send packet function.

  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, password);

  while (WiFi.status() != WL_CONNECTED) {
      delay(500);
      Serial.print(".");
  }

  WOL.calculateBroadcastAddress(WiFi.localIP(), WiFi.subnetMask()); // Optional  => To calculate the broadcast address, otherwise 255.255.255.255 is used (which is denied in some networks).
  
}
void loop() {
  // Clears the trigPin
  digitalWrite(trigPin, LOW);
  delayMicroseconds(2);
  // Sets the trigPin on HIGH state for 10 micro seconds
  digitalWrite(trigPin, HIGH);
  delayMicroseconds(10);
  digitalWrite(trigPin, LOW);
  // Reads the echoPin, returns the sound wave travel time in microseconds
  duration = pulseIn(echoPin, HIGH);
  // Calculating the distance
  distance= duration*0.034/2;
  // Prints the distance on the Serial Monitor
  Serial.print("Distance: ");
  Serial.println(slerpDistance);

  if(distance <1000)//Clean out errors
  {
    //Slerp the value (slow down changes)
    slerpDistance = slerpDistance + (distance- slerpDistance)*0.1;

    //When person is close start
    if(slerpDistance < 100)
    {
      if(!pcRunning)
      {
        wakeMyPC();
        Serial.println("Starting");
        pcRunning = true;
      }
    }

    //When person is far go to sleep mode
    if(slerpDistance>200)
    {
      if(pcRunning)
      {
        shutMyPC();
        Serial.println("Stopping");
        pcRunning = false;
      }
    }
    delay(100);
  }

  
  delay(1);
}
