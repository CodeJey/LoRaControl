#include <DHT.h>
#include <SPI.h>
#include <LoRa.h>

//defining pins for sensor
#define DHTPIN 4
#define DHTTYPE DHT22
//defining the sensor
DHT dht(DHTPIN, DHTTYPE);
//array for sensor data
char tem_1[8]={"\0"},hum_1[8]={"\0"};
//def the node id(BOARD). Must be changed for different boards and specified for each, the id also must be added in dragino control panel 
const char *node_id = ("<8901>");  //From LG01 via web Local Channel settings on MQTT.Please refer <> dataformat in here. 
//def package length
uint8_t datasend[42];
//refresh time for receiving/sending
unsigned long new_time,old_time=0;
//message string
String message = "";
//relay pin
const int relayPin = 3;

void setup()
{
      Serial.begin(9600);
      //begin dht sensor
      dht.begin();
      //setting the pin mode for the relay
      pinMode(relayPin, OUTPUT);
      while (!Serial);
      Serial.println(F("Start MQTT Example")); 
      if (!LoRa.begin(868100000))   //868000000 is frequency
      { 
          Serial.println("Starting LoRa failed!");
          while (1);
      }
      // Setup Spreading Factor (6 ~ 12)
      LoRa.setSpreadingFactor(7);
      
      // Setup BandWidth, option: 7800,10400,15600,20800,31250,41700,62500,125000,250000,500000
      //Lower BandWidth for longer distance.
      LoRa.setSignalBandwidth(125000);
      
      // Setup Coding Rate:5(4/5),6(4/6),7(4/7),8(4/8) 
      LoRa.setCodingRate4(5);
      LoRa.setSyncWord(0x34); 
      Serial.println("LoRa init succeeded.");
      //receive setting
      LoRa.onReceive(onReceive);   
      LoRa.receive();
}

void dhtWrite()
{
     //reading data
    float tem,hum;
    tem = dht.readTemperature();
    delay(100);
    hum = dht.readHumidity();
    char data[50] = "\0";
      //setting data
    for(int i = 0; i < 50; i++)
    {
       data[i] = node_id[i];
    }
      //setting data sequence in the data array
    dtostrf(tem,0,1,tem_1);
    dtostrf(hum,0,1,hum_1);
      //getting the data in data array
     strcat(data,"t3=");
     strcat(data,tem_1);
     strcat(data,"&h3=");
     strcat(data,hum_1);;
     strcpy((char *)datasend,data);    
}


void SendData()
{
     //sending data method
     LoRa.beginPacket();
     LoRa.print((char *)datasend);
     LoRa.endPacket();
}    
    
void loop()
{   
    //setting loop time(same as "delay()" at the end)
    new_time=millis();
    if (new_time - old_time >= 1000 || old_time == 0)
    {
      //loop actions
      old_time = new_time;
      //sensor data processing
      dhtWrite();
      //sending sensor data
      SendData();
      //receive messages
      LoRa.receive();
    }
}


void onReceive(int packetSize) {
  // read package
  for (int i = 0; i < packetSize; i++) {
      //collecting the data in a string
      message = message + (char)LoRa.read();
  }
  //trim string spaces
  message.trim();
  //uncomment next 3 lines if serial printing of the message is needed
  //Serial.print(message);
  //Serial.print("Received packet : ");
  //Serial.print("\n\r"); 
  //check the received messages as commands
  if (message == "DATA")
  {
    digitalWrite(relayPin, HIGH);
  }
  else if (message == "DATAM")
  {
    digitalWrite(relayPin, LOW);
  }
  //cleaning the message string
  message = ""; 
}
