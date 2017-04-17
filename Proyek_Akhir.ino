#include <DHT.h>
#include <SPI.h>
#include <Ethernet.h>


byte sensorInterrupt = 0;
byte sensorPin = 2;

float calibrationFactor = 4.5;

volatile byte pulseCount;

float flowRate;
unsigned int flowMilliLitres;
unsigned long totalMilliLitres;

unsigned long oldTime;

#define DHTPIN 3
#define DHTTYPE DHT11   // DHT 11



DHT dht(DHTPIN, DHTTYPE);

// Set the MAC address
byte mac[] = {0xDE, 0xAD, 0xBE, 0xEF, 0xFE, 0xED};

// Set the IP address
IPAddress ip(192, 168, 1, 177);

char server[] = "krstudio.web.id";

// Start a server at port 80 (http)
EthernetClient client;

void setup() {
  // Open serial communications
  Serial.begin(9600);

  // start the Ethernet connection and the server
  Ethernet.begin(mac, ip);
  dht.begin();

  pinMode(sensorPin, INPUT);
  digitalWrite(sensorPin, HIGH);
  pulseCount        = 0;
  flowRate          = 0.0;
  flowMilliLitres   = 0;
  totalMilliLitres  = 0;
  oldTime           = 0;

  attachInterrupt(sensorInterrupt, pulseCounter, FALLING);

  // Pin 22 - 24 output (leds)
  pinMode(5, OUTPUT);
  pinMode(6, OUTPUT);
  pinMode(7, OUTPUT);



}


void loop() {
  // read the input on analog pin 0:
  int sensorValue = analogRead(A0); 
  String kondisisoil;
  
  
  sensorValue = analogRead(A0);
  sensorValue = map(sensorValue,160,1023,1023,0);
  delay(100);
  
  
  if(sensorValue>=0&&sensorValue<=300)
  {
    kondisisoil="DrySoil";
    Serial.print(kondisisoil);
    Serial.print(sensorValue);
  }else if(sensorValue>=300&&sensorValue<=700)
  {
    kondisisoil="HumidSoil";
    Serial.print(kondisisoil);
    Serial.print(sensorValue);

  }else if(sensorValue>=700)
  {
    kondisisoil="InWater";
    Serial.print(kondisisoil);
    Serial.print(sensorValue);

  }
  
  //Humidity & Temp
  float h = dht.readHumidity();
  float t = dht.readTemperature();
 
  
  if (client.connect(server, 80)) { 
    client.print("GET /write_data.php?");
    client.print("temp=");
    client.print(t);
    client.print("&&");
    client.print("humi=");
    client.print(h);
    client.print("&&");
    client.print("kondisisoil=");
    client.print(kondisisoil);
    client.print("&&");
    client.print("sensorsoil=");
    client.print(sensorValue);
    
    if((millis() - oldTime) > 1000)
              {
                detachInterrupt(sensorInterrupt);
                flowRate = ((1000.0 / (millis() - oldTime)) * pulseCount) / calibrationFactor;
                oldTime = millis();
                flowMilliLitres = (flowRate / 60) * 1000;
                totalMilliLitres += flowMilliLitres;
                unsigned int frac;
                client.print("&&");
                client.print("flowrate=");
                client.print(int(flowRate));
                frac = (flowRate - int(flowRate)) * 10;
                client.print(frac, DEC);
                client.print("&&");
                client.print("currentliquid=");
                client.print(flowMilliLitres); 
                pulseCount = 0;
                attachInterrupt(sensorInterrupt, pulseCounter, FALLING);
              }
   
    client.print("&&");
    client.print("solenoid=");
     
    client.println(" HTTP/1.1");
    client.println("Host: krstudio.web.id");
    client.println("Connection: close");
    client.println();
    client.println();
    client.stop();
  }else {
    // If Arduino can't connect to the server (your computer or web page)
    Serial.println("--> connection failed\n");
  }

  delay(10000);
  
}
void pulseCounter(){
  pulseCount++;
}
