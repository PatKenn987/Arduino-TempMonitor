/*
  Web client
 
 This sketch connects to a website (http://www.google.com)
 using an Arduino Wiznet Ethernet shield. 
 
 Circuit:
 * Ethernet shield attached to pins 10, 11, 12, 13
 
 created 18 Dec 2009
 by David A. Mellis
 modified 9 Apr 2012
 by Tom Igoe, based on work by Adrian McEwen
 */
#include <DHT.h>
#include <SPI.h>
#include <Ethernet.h>

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
long previousMillis = 0;    //Counters for time
unsigned long currentMillis = 0;
long interval = 60000;  //Reading Interval

unsigned long lastConnectionTime = 0;          // last time you connected to the server, in milliseconds
boolean lastConnected = false;                 // state of the connection last time through the main loop
const unsigned long postingInterval = 60*1000;  // delay between updates, in milliseconds


int t = 0; //Variable to store Temp
int h = 0; //Variable to store Humidity
String data;//String to format for POST operation.
/*************************************************/
//Constants, variables and notes for Ethernet
// Enter a MAC address for your controller below.
// Newer Ethernet shields have a MAC address printed on a sticker on the shield
byte mac[] = { 0xDE, 0xAD, 0xBE, 0xEF, 0xFE, 0xED };
// if you don't want to use DNS (and reduce your sketch size)
// use the numeric IP instead of the name for the server:
//IPAddress server(74,125,232,128);  // numeric IP for Google (no DNS)
char server[] = "www.ken-fam.dynamic-dns.net";    // name address for ken-fam.dynamic-dns.net (using DNS)

// Set the static IP address for ken-fam server to use if the DHCP fails to assign
IPAddress ip(108,30,174,103);

// Initialize the Ethernet client library
// with the IP address and port of the server 
// that you want to connect to (port 80 is default for HTTP):
EthernetClient client;

//Declare function
//void SendTempData(void);

void setup() {
 // Open serial communications and wait for port to open:
  Serial.begin(9600);
   while (!Serial) {
    ; // wait for serial port to connect. Needed for Leonardo only
  }
  Serial.println("TempMon Version 1.0!");

  // start the Ethernet connection:
  if (Ethernet.begin(mac) == 0) {
    Serial.println("Failed to configure Ethernet using DHCP");
    // no point in carrying on, so do nothing forevermore:
    // try to congifure using IP address instead of DHCP:
    Ethernet.begin(mac, ip);
  }

  Serial.println("connecting...");
  // give the Ethernet shield a second to initialize:
  delay(1000);
  Serial.print("localIP: ");
  Serial.println(Ethernet.localIP());
  Serial.print("subnetMask: ");
  Serial.println(Ethernet.subnetMask());
  Serial.print("gatewayIP: ");
  Serial.println(Ethernet.gatewayIP());
  Serial.print("dnsServerIP: ");
  Serial.println(Ethernet.dnsServerIP());
  //Initialize the Temp/Hum sensor
  dht.begin();
  delay(10000); //Give the sensor some time
  h = (int) dht.readHumidity();
  t = (int) dht.readTemperature();
}

void loop()
{
  // if there are incoming bytes available 
  // from the server, read them and print them:
  if (client.available()) {
    char c = client.read();
    Serial.print(c);
  }

  // if there is no net connection, but there was one the last tim
  // through the loop then stop the client
  if (!client.connected() && lastConnected){
    Serial.println();
    Serial.println("disconnecting.");
    client.stop();
  }
  
  // If you are not connected, and ten seconds have passed since
  // your last connection, then connect again and send the data.
  
  if(!client.connected() && (millis() - lastConnectionTime > postingInterval)) {
    SendTempData(); 
  }

  lastConnected = client.connected();
}

void SendTempData()
(
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
 
  // if there is a successful connection
  if(client.connect(server, 80))
  { 
    //Assemble the POST message to the server.
    client.println("POST /Logging/add.php HTTP/1.1");
    Serial.println("POST /Logging/add.php HTTP/1.1");
    client.println("Host: www.ken-fam.dynamic-dns.net");
    Serial.println("Host: www.ken-fam.dynamic-dns.net");
    client.println("Accept: */*");
    Serial.println("Accept: */*");
    client.println("Content-Type: application/x-www-form-urlencoded");
    Serial.println("Content-Type: application/x-www-form-urlencoded");
    client.print("Content-Length: ");
    Serial.print("Content-Length: ");
    client.println(data.length());
    Serial.println(data.length());
    client.println();
    Serial.println();
    client.print(data);
    Serial.print(data);
  }
  else {
    //if you could not make a connection
    Serial.println("Connection failed");
    Serial.println("disconnecting");
    client.stop();
  } 
}

