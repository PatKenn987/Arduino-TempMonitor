/*
Temperature and Humidity monitoring.

This code was derived from the repeating Web Client sample that is provided
from Arduino.  These are the notes from that program:
*****************************************************************************
 This sketch connects to a a web server and makes a request
 using a Wiznet Ethernet shield. You can use the Arduino Ethernet shield, or
 the Adafruit Ethernet shield, either one will work, as long as it's got
 a Wiznet Ethernet module on board.
 This example uses DNS, by assigning the Ethernet client with a MAC address,
 IP address, and DNS address.
 
 Circuit:
 * Ethernet shield attached to pins 10, 11, 12, 13
 
 created 19 Apr 2012
 by Tom Igoe
 
 http://arduino.cc/en/Tutorial/WebClientRepeating
 This code is in the public domain.
 ***************************************************************************

 Modification were made to the program to use a 



 */

#include <DHT.h>
#include <SPI.h>
#include <Ethernet.h>


#define DEBUG_BUILD 1
//Constants, variables and notes for Temp/Humid data collection
#define DHTPIN 2     // what pin we're connected to
#define DHTTYPE DHT22   // DHT 22  (AM2302)
// Connect pin 1 (on the left) of the sensor to +5V
// NOTE: If using a board with 3.3V logic like an Arduino Due connect pin 1
// to 3.3V instead of 5V!
// Connect pin 2 of the sensor to whatever your DHTPIN is
// Connect pin 4 (on the right) of the sensor to GROUND
// Connect a 10K resistor from pin 2 (data) to pin 1 (power) of the sensor
// Initialize DHT sensor for normal 16mhz Arduino
DHT dht(DHTPIN, DHTTYPE); //Define class variable

int t = 0; //Variable to store Temp
int h = 0; //Variable to store Humidity
String data;//String to format for POST operation.
/**********************************************************/
//Constants, variables and notes for network connection
// assign a MAC address for the ethernet controller.
// fill in your address here:
byte mac[] = { 0xDE, 0xAD, 0xBE, 0xEF, 0xFE, 0xED};
// fill in an available IP address on your network here,
// for manual configuration:
IPAddress ip(108,30,174,103);

// fill in your Domain Name Server address here:
IPAddress myDns(192,168,1,1);

// initialize the library instance:
EthernetClient client;

char server[] = "www.ken-fam.dynamic-dns.net";

/****************************************************************/
//Variables for message timing
unsigned long lastConnectionTime = 0;          // last time you connected to the server, in milliseconds
boolean lastConnected = false;                 // state of the connection last time through the main loop
const unsigned long postingInterval = 1000;  // delay between updates, in milliseconds

void setup() {
  // start serial port:
  Serial.begin(9600);
  Serial.println("TempMon Version 1.0!");

  // give the ethernet module time to boot up:
  delay(1000);
  // start the Ethernet connection using a fixed IP address and DNS server:
  //Ethernet.begin(mac,ip, myDns);
  Ethernet.begin(mac);
 // print the Ethernet board/shield's IP address:
  Serial.print("My IP address: ");
  Serial.println(Ethernet.localIP());
#ifdef DEBUG_BUILD
  Serial.print("localIP: ");
  Serial.println(Ethernet.localIP());
  Serial.print("subnetMask: ");
  Serial.println(Ethernet.subnetMask());
  Serial.print("gatewayIP: ");
  Serial.println(Ethernet.gatewayIP());
  Serial.print("dnsServerIP: ");
  Serial.println(Ethernet.dnsServerIP());
#endif

  //Initialize the Temp/Hum sensor
  dht.begin();
  delay(10000); //Give the sensor some time
  h = (int) dht.readHumidity();
  t = (int) dht.readTemperature();
  data = "";
}

void loop() {
  // if there's incoming data from the net connection.
  // send it out the serial port.  This is for debugging
  // purposes only:
  if (client.available()) {
    char c = client.read();
    Serial.print(c);
  }

  // if there's no net connection, but there was one last time
  // through the loop, then stop the client:
  if (!client.connected() && lastConnected) {
    Serial.println();
    Serial.println("disconnecting.");
    client.stop();
  }

  // if you're not connected, and ten seconds have passed since
  // your last connection, then connect again and send data:
  if(!client.connected() && (millis() - lastConnectionTime > postingInterval)) {
    httpRequest();
  }
  // store the state of the connection for next time through
  // the loop:
  lastConnected = client.connected();
}

// this method makes a HTTP connection to the server:
void httpRequest() {
  // Reading temperature or humidity takes about 250 milliseconds!
  // Sensor readings may also be up to 2 seconds 'old' (its a very slow sensor)
  h = (int) dht.readHumidity();
  // Read temperature as Celsius
  t = (int) dht.readTemperature();  

  //Format the data string to the Host servers specifications.
  data= "temp1=";
  data +=t;
  data +="&hum1=";
  data +=h;
  // if there's a successful connection:
  if (client.connect(server, 80)) {
    Serial.println("connecting...");
    //Assemble the POST message to the server.
    client.println("POST /Logging/add.php HTTP/1.1");
    client.println("Host: www.ken-fam.dynamic-dns.net");
    client.println("Accept: */*");
    client.println("Content-Type: application/x-www-form-urlencoded");
    client.print("Content-Length: ");
    client.println(data.length());
    client.println();
    client.print(data);
#ifdef DEBUG_BUILD
    Serial.println("POST /Logging/add.php HTTP/1.1");
    Serial.println("Host: www.ken-fam.dynamic-dns.net");
    Serial.println("Accept: */*");
    Serial.println("Content-Type: application/x-www-form-urlencoded");
    Serial.print("Content-Length: ");
    Serial.println(data.length());
    Serial.println();
    Serial.print(data);
#endif
    // note the time that the connection was made:
    lastConnectionTime = millis();
  } 
  else {
    // if you couldn't make a connection:
    Serial.println("connection failed");
    Serial.println("disconnecting.");
    client.stop();
  }
}
