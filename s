[1mdiff --git a/TempMon1.ino b/TempMon1.ino[m
[1mnew file mode 100644[m
[1mindex 0000000..7b992e6[m
[1m--- /dev/null[m
[1m+++ b/TempMon1.ino[m
[36m@@ -0,0 +1,165 @@[m
[32m+[m[32m/*[m
[32m+[m[32m  Web client[m
[32m+[m[41m [m
[32m+[m[32m This sketch connects to a website (http://www.google.com)[m
[32m+[m[32m using an Arduino Wiznet Ethernet shield.[m[41m [m
[32m+[m[41m [m
[32m+[m[32m Circuit:[m
[32m+[m[32m * Ethernet shield attached to pins 10, 11, 12, 13[m
[32m+[m[41m [m
[32m+[m[32m created 18 Dec 2009[m
[32m+[m[32m by David A. Mellis[m
[32m+[m[32m modified 9 Apr 2012[m
[32m+[m[32m by Tom Igoe, based on work by Adrian McEwen[m
[32m+[m[32m */[m
[32m+[m[32m#include <DHT.h>[m
[32m+[m[32m#include <SPI.h>[m
[32m+[m[32m#include <Ethernet.h>[m
[32m+[m
[32m+[m[32m//Constants, variables and notes for Temp/Humid data collection[m
[32m+[m[32m#define DHTPIN 2     // what pin we're connected to[m
[32m+[m[32m#define DHTTYPE DHT22   // DHT 22  (AM2302)[m
[32m+[m[32m// Connect pin 1 (on the left) of the sensor to +5V[m
[32m+[m[32m// NOTE: If using a board with 3.3V logic like an Arduino Due connect pin 1[m
[32m+[m[32m// to 3.3V instead of 5V![m
[32m+[m[32m// Connect pin 2 of the sensor to whatever your DHTPIN is[m
[32m+[m[32m// Connect pin 4 (on the right) of the sensor to GROUND[m
[32m+[m[32m// Connect a 10K resistor from pin 2 (data) to pin 1 (power) of the sensor[m
[32m+[m[32m// Initialize DHT sensor for normal 16mhz Arduino[m
[32m+[m[32mDHT dht(DHTPIN, DHTTYPE); //Define class variable[m
[32m+[m[32mlong previousMillis = 0;    //Counters for time[m
[32m+[m[32munsigned long currentMillis = 0;[m
[32m+[m[32mlong interval = 60000;  //Reading Interval[m
[32m+[m
[32m+[m[32munsigned long lastConnectionTime = 0;          // last time you connected to the server, in milliseconds[m
[32m+[m[32mboolean lastConnected = false;                 // state of the connection last time through the main loop[m
[32m+[m[32mconst unsigned long postingInterval = 60*1000;  // delay between updates, in milliseconds[m
[32m+[m
[32m+[m
[32m+[m[32mint t = 0; //Variable to store Temp[m
[32m+[m[32mint h = 0; //Variable to store Humidity[m
[32m+[m[32mString data;//String to format for POST operation.[m
[32m+[m[32m/*************************************************/[m
[32m+[m[32m//Constants, variables and notes for Ethernet[m
[32m+[m[32m// Enter a MAC address for your controller below.[m
[32m+[m[32m// Newer Ethernet shields have a MAC address printed on a sticker on the shield[m
[32m+[m[32mbyte mac[] = { 0xDE, 0xAD, 0xBE, 0xEF, 0xFE, 0xED };[m
[32m+[m[32m// if you don't want to use DNS (and reduce your sketch size)[m
[32m+[m[32m// use the numeric IP instead of the name for the server:[m
[32m+[m[32m//IPAddress server(74,125,232,128);  // numeric IP for Google (no DNS)[m
[32m+[m[32mchar server[] = "www.ken-fam.dynamic-dns.net";    // name address for ken-fam.dynamic-dns.net (using DNS)[m
[32m+[m
[32m+[m[32m// Set the static IP address for ken-fam server to use if the DHCP fails to assign[m
[32m+[m[32mIPAddress ip(108,30,174,103);[m
[32m+[m
[32m+[m[32m// Initialize the Ethernet client library[m
[32m+[m[32m// with the IP address and port of the server[m[41m [m
[32m+[m[32m// that you want to connect to (port 80 is default for HTTP):[m
[32m+[m[32mEthernetClient client;[m
[32m+[m
[32m+[m[32m//Declare function[m
[32m+[m[32m//void SendTempData(void);[m
[32m+[m
[32m+[m[32mvoid setup() {[m
[32m+[m[32m // Open serial communications and wait for port to open:[m
[32m+[m[32m  Serial.begin(9600);[m
[32m+[m[32m   while (!Serial) {[m
[32m+[m[32m    ; // wait for serial port to connect. Needed for Leonardo only[m
[32m+[m[32m  }[m
[32m+[m[32m  Serial.println("TempMon Version 1.0!");[m
[32m+[m
[32m+[m[32m  // start the Ethernet connection:[m
[32m+[m[32m  if (Ethernet.begin(mac) == 0) {[m
[32m+[m[32m    Serial.println("Failed to configure Ethernet using DHCP");[m
[32m+[m[32m    // no point in carrying on, so do nothing forevermore:[m
[32m+[m[32m    // try to congifure using IP address instead of DHCP:[m
[32m+[m[32m    Ethernet.begin(mac, ip);[m
[32m+[m[32m  }[m
[32m+[m
[32m+[m[32m  Serial.println("connecting...");[m
[32m+[m[32m  // give the Ethernet shield a second to initialize:[m
[32m+[m[32m  delay(1000);[m
[32m+[m[32m  Serial.print("localIP: ");[m
[32m+[m[32m  Serial.println(Ethernet.localIP());[m
[32m+[m[32m  Serial.print("subnetMask: ");[m
[32m+[m[32m  Serial.println(Ethernet.subnetMask());[m
[32m+[m[32m  Serial.print("gatewayIP: ");[m
[32m+[m[32m  Serial.println(Ethernet.gatewayIP());[m
[32m+[m[32m  Serial.print("dnsServerIP: ");[m
[32m+[m[32m  Serial.println(Ethernet.dnsServerIP());[m
[32m+[m[32m  //Initialize the Temp/Hum sensor[m
[32m+[m[32m  dht.begin();[m
[32m+[m[32m  delay(10000); //Give the sensor some time[m
[32m+[m[32m  h = (int) dht.readHumidity();[m
[32m+[m[32m  t = (int) dht.readTemperature();[m
[32m+[m[32m}[m
[32m+[m
[32m+[m[32mvoid loop()[m
[32m+[m[32m{[m
[32m+[m[32m  // if there are incoming bytes available[m[41m [m
[32m+[m[32m  // from the server, read them and print them:[m
[32m+[m[32m  if (client.available()) {[m
[32m+[m[32m    char c = client.read();[m
[32m+[m[32m    Serial.print(c);[m
[32m+[m[32m  }[m
[32m+[m
[32m+[m[32m  // if there is no net connection, but there was one the last tim[m
[32m+[m[32m  // through the loop then stop the client[m
[32m+[m[32m  if (!client.connected() && lastConnected){[m
[32m+[m[32m    Serial.println();[m
[32m+[m[32m    Serial.println("disconnecting.");[m
[32m+[m[32m    client.stop();[m
[32m+[m[32m  }[m
[32m+[m[41m  [m
[32m+[m[32m  // If you are not connected, and ten seconds have passed since[m
[32m+[m[32m  // your last connection, then connect again and send the data.[m
[32m+[m[41m  [m
[32m+[m[32m  if(!client.connected() && (millis() - lastConnectionTime > postingInterval)) {[m
[32m+[m[32m    SendTempData();[m[41m [m
[32m+[m[32m  }[m
[32m+[m
[32m+[m[32m  lastConnected = client.connected();[m
[32m+[m[32m}[m
[32m+[m
[32m+[m[32mvoid SendTempData()[m
[32m+[m[32m([m
[32m+[m[32m  // Reading temperature or humidity takes about 250 milliseconds![m
[32m+[m[32m  // Sensor readings may also be up to 2 seconds 'old' (its a very slow sensor)[m
[32m+[m[32m  h = (int) dht.readHumidity();[m
[32m+[m[32m  // Read temperature as Celsius[m
[32m+[m[32m  t = (int) dht.readTemperature();[m[41m [m
[32m+[m[32m  //Format the data string to the Host servers specifications.[m
[32m+[m[32m  data= "temp1=";[m
[32m+[m[32m  data +=t;[m
[32m+[m[32m  data +="&hum1=";[m
[32m+[m[32m  data +=h;[m
[32m+[m[41m [m
[32m+[m[32m  // if there is a successful connection[m
[32m+[m[32m  if(client.connect(server, 80))[m
[32m+[m[32m  {[m[41m [m
[32m+[m[32m    //Assemble the POST message to the server.[m
[32m+[m[32m    client.println("POST /Logging/add.php HTTP/1.1");[m
[32m+[m[32m    Serial.println("POST /Logging/add.php HTTP/1.1");[m
[32m+[m[32m    client.println("Host: www.ken-fam.dynamic-dns.net");[m
[32m+[m[32m    Serial.println("Host: www.ken-fam.dynamic-dns.net");[m
[32m+[m[32m    client.println("Accept: */*");[m
[32m+[m[32m    Serial.println("Accept: */*");[m
[32m+[m[32m    client.println("Content-Type: application/x-www-form-urlencoded");[m
[32m+[m[32m    Serial.println("Content-Type: application/x-www-form-urlencoded");[m
[32m+[m[32m    client.print("Content-Length: ");[m
[32m+[m[32m    Serial.print("Content-Length: ");[m
[32m+[m[32m    client.println(data.length());[m
[32m+[m[32m    Serial.println(data.length());[m
[32m+[m[32m    client.println();[m
[32m+[m[32m    Serial.println();[m
[32m+[m[32m    client.print(data);[m
[32m+[m[32m    Serial.print(data);[m
[32m+[m[32m  }[m
[32m+[m[32m  else {[m
[32m+[m[32m    //if you could not make a connection[m
[32m+[m[32m    Serial.println("Connection failed");[m
[32m+[m[32m    Serial.println("disconnecting");[m
[32m+[m[32m    client.stop();[m
[32m+[m[32m  }[m[41m [m
[32m+[m[32m}[m
[32m+[m
