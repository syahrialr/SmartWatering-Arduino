#include "ThingSpeak.h"
#define USE_ETHERNET_SHIELD
#if!defined(USE_ETHERNET_SHIELD)
#error "Uncomment the #define for either USE_WIFI101_SHIELD or USE_ETHERNET_SHIELD"
#endif
#if defined(USE_ETHERNET_SHIELD)
#include <DHT.h>
#include <SPI.h>
#include <Ethernet.h>


// Set Koneksi
byte mac[] = {0xDE, 0xAD, 0xBE, 0xEF, 0xFE, 0xED};
//the IP address is dependent on your network
IPAddress ip(192, 168, 1, 177);
char server[] = "krstudio.web.id";
// Start a server at port 80 (http)
EthernetClient client;
int Buffer=8;
bool aha=false;
#endif
//Deklarasi pin
int Relay = 7;
int relay_level=400;
int Solenoid =4;
byte humidity_sensor_pin = A0;
byte humidity_sensor_vcc = 6;
byte humidity_sensor_pin2 = A1;
byte humidity_sensor_vcc2 = 5;
byte sensorPin = 2;
#define DHTPIN 3
#define DHTTYPE DHT11   // DHT 11
DHT dht(DHTPIN, DHTTYPE);

//deklarasi variable
int value;//value soil moisture1
int value2;//value soil moisture 2
byte sensorInterrupt = 0;
float calibrationFactor = 4.5;
volatile byte pulseCount;
float flowRate;
unsigned int flowMilliLitres;
unsigned long totalMilliLitres;
unsigned long oldTime;
unsigned long channel = 274748;//channel thingspeak
unsigned int field = 1;//field awal
String kondisisoil;
String kondisisoil2;
int solen;
float data;

//ini adalah fungsi untuk millis
unsigned long milissebelumnya=0;
unsigned long interval=1800000;

int sensorValue() 
    {
      digitalWrite(humidity_sensor_vcc, HIGH);
      delay(500);
      value = analogRead(humidity_sensor_pin);
      digitalWrite(humidity_sensor_vcc, LOW);
      return 1023 - value;
    }
int sensorValue2() 
    {
      digitalWrite(humidity_sensor_vcc2, HIGH);
      delay(500);
      value2 = analogRead(humidity_sensor_pin2);
      digitalWrite(humidity_sensor_vcc2, LOW);
      return 1023 - value2;
    }



void setup() 
    {
    Serial.begin(9600);
       // initialize the ethernet device
     
      //set pinMode
      pinMode(sensorPin, INPUT);
      pinMode(Solenoid, OUTPUT);   
      pinMode(Relay, OUTPUT);
      pinMode(humidity_sensor_vcc, OUTPUT);
      pinMode(humidity_sensor_vcc2, OUTPUT);
      pinMode(Buffer,OUTPUT);
    
      //kondisi awal sensor
      digitalWrite(humidity_sensor_vcc, LOW);
      digitalWrite(humidity_sensor_vcc2, LOW);
      digitalWrite(sensorPin, HIGH);
      digitalWrite(Solenoid,LOW);
      digitalWrite(Relay,LOW);
      

      Ethernet.begin(mac);
      //print out the IP address
    
    
      // Open serial communications
      
      ThingSpeak.begin(client);
      // start the Ethernet connection and the server
      
      dht.begin();
    
      pulseCount        = 0;
      flowRate          = 0.0;
      flowMilliLitres   = 0;
      totalMilliLitres  = 0;
      oldTime           = 0;
    
      attachInterrupt(sensorInterrupt, pulseCounter, FALLING);
    }

//

void loop() 
    {
      data = ThingSpeak.readFloatField(channel, field);
      Serial.println(data);
      if(data==1||data==1.00){
        aha=true;
      }else{
        aha=false;
      }
      if(aha==true)
      {
      PenyiramanManual(aha);
      }
       bacakondisi();
       sensor1();
       sensor2();
      
      //Humidity & Temp
      float h = dht.readHumidity();
      float t = dht.readTemperature();
  unsigned long millissekarang=millis();  
 if(millissekarang-milissebelumnya>interval)
 {
       milissebelumnya=millissekarang;
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
        client.print(sensorValue());
        client.print("&&");
        client.print("kondisisoil2=");
        client.print(kondisisoil2);
        client.print("&&");
        client.print("sensorsoil2=");
        client.print(sensorValue2()); 
        
//        if((millis() - oldTime) > 1000)
//                {
//                  detachInterrupt(sensorInterrupt);
//                  flowRate = ((1000.0 / (millis() - oldTime)) * pulseCount) / calibrationFactor;
//                  oldTime = millis();
//                  flowMilliLitres = (flowRate / 60) * 1000;
//                  totalMilliLitres += flowMilliLitres;
//                  unsigned int frac;
//                  client.print("&&");
//                  client.print("flowrate=");
//                  client.print(int(flowRate));
//                  frac = (flowRate - int(flowRate)) * 10;
//                  client.print(frac, DEC);
//                  client.print("&&");
//                  client.print("currentliquid=");
//                  client.print(flowMilliLitres); 
//                  pulseCount = 0;
//                  attachInterrupt(sensorInterrupt, pulseCounter, FALLING);
//                }
//       
//        client.print("&&");
//    
        // wait for a minute
         
        client.println(" HTTP/1.1");
        client.println("Host: krstudio.web.id");
        client.println("Connection: close");
        client.println();
        client.println();
        client.stop();
      }
      else 
        {
          
          // If Arduino can't connect to the server (your computer or web page)
          Serial.println("--> connection failed\n");
          tone(Buffer,1000);
          delay(500);
          noTone(Buffer);
          
        }  
       
 }
   
}



//tempatnya fungsi
void pulseCounter()
{
   pulseCount++;
}
void PenyiramanManual(bool stat)
    {
        // Read the latest value from field 4 of channel 12397
     
      Serial.println(data);
      if(stat==true)
      {
          if(data==0.00||data==0)
          {
            Serial.print(data);
            digitalWrite(Relay,HIGH);
            digitalWrite(Solenoid,LOW);
            aha=false;
            delay(500);
          }
          else if(data==1.00 || data==1)
          {
            Serial.print(data);
            digitalWrite(Relay,LOW);
            digitalWrite(Solenoid,HIGH);
            delay(500); // Note that the weather station only updates once a minute
          }
      }
    }
void sensor1()
{

  if(aha==false){
   if (sensorValue() > relay_level) 
          {  
            digitalWrite(Relay, HIGH);
            delay(500);
          } 
        else if (sensorValue() < relay_level)
          {
            digitalWrite(Relay, LOW);
            delay(500);
          }
          else if(sensorValue==0)
          {
            digitalWrite(Relay,HIGH);
            delay(500);
          }
    }
}
void sensor2(){
  if(aha==false)
  {
   if (sensorValue2() > relay_level) 
          {  
            digitalWrite(Solenoid, LOW);
            delay(500);
          } 
        else if (sensorValue2() < relay_level)
          {
            digitalWrite(Solenoid, HIGH);
            delay(500);
          }
          else if(sensorValue==0)
          {
            digitalWrite(Relay,LOW);
            delay(500);
          }
  }
}
void bacakondisi()
{
  //soil moisture 1
      if(sensorValue==0)
        {
          kondisisoil="Disconnect_Sensor";
          
        }
     else  if(sensorValue()>0&&sensorValue()<=300)
        {
          kondisisoil="DrySoil";
          Serial.print(kondisisoil);
          Serial.print(sensorValue());
        }
      else if(sensorValue()>=300&&sensorValue()<=700)
        {
          kondisisoil="HumidSoil";
          Serial.print(kondisisoil);
          Serial.print(sensorValue());
      
        }
      else if(sensorValue()>=700)
        {
          kondisisoil="InWater";
          Serial.print(kondisisoil);
          Serial.print(sensorValue());
        }
        
      
      //soil moisture 2
      if(sensorValue2==0)
        {
          kondisisoil2="Disconnect_Sensor";
         
        }
      else if(sensorValue2()>0&&sensorValue2()<=300)
        {
          kondisisoil2="Dry";
          Serial.print(kondisisoil2);
          Serial.print(sensorValue2());
        }
      else if(sensorValue2()>=300&&sensorValue2()<=700)
        {
          kondisisoil2="Humid";
          Serial.print(kondisisoil2);
          Serial.print(sensorValue2());
      
        }
      else if(sensorValue2>=700)
        {
          kondisisoil2="Water";
          Serial.print(kondisisoil2);
          Serial.print(sensorValue2());
      
        }
}
