#include <SharpIR.h>
#include <DHT.h>
#include <SPI.h>
#include <LoRa.h>

//setting sensors properties and defs
#define DHTPIN1 3
#define DHTPIN2 4
#define DHTTYPE1 DHT11 
#define DHTTYPE2 DHT22
DHT dht1(DHTPIN1, DHTTYPE1);
DHT dht2(DHTPIN2, DHTTYPE2);

#define IRPin A0
#define model 20150
SharpIR mySensor = SharpIR(IRPin, model);

char tem_1[8]={"\0"},hum_1[8]={"\0"}, tem_2[8]={"\0"},hum_2[8]={"\0"}, dis[8]={"\0"};
const char *node_id = ("<4567>");  //From LG01 via web Local Channel settings on MQTT.Please refer <> dataformat in here. 
uint8_t datasend[42];
//refresh time 
unsigned long new_time,old_time=0;

void setup()
{
      //begin sensors and serial
      Serial.begin(9600);
      dht1.begin();
      dht2.begin();

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
}

void dhtWrite()
{
    //sensord data reading
    float tem,hum,tem1,hum1;
    tem = dht1.readTemperature();
    delay(100);
    tem1 = dht2.readTemperature();
    delay(100);
    hum = dht1.readHumidity();
    delay(100);
    hum1 = dht2.readHumidity();
    //setting in array
    char data[50] = "\0";
    for(int i = 0; i < 50; i++)
    {
       data[i] = node_id[i];
    }

    ////setting data sequence in the data array
    dtostrf(tem,0,1,tem_1);
    dtostrf(hum,0,1,hum_1);
    dtostrf(tem1,0,1,tem_2);
    dtostrf(hum1,0,1,hum_2);
    dtostrf(distF(),0,1,dis);
    //getting the data in data array
     strcat(data,"t1=");
     strcat(data,tem_1);
     strcat(data,"&h1=");
     strcat(data,hum_1);
     strcat(data,"&t2=");
     strcat(data,tem_2);
     strcat(data,"&h2=");
     strcat(data,hum_2);
     strcat(data,"&d=");
     strcat(data,dis);
     strcpy((char *)datasend,data);
}


void SendData()
{
     //sending data to modem dragino
     LoRa.beginPacket();
     LoRa.print((char *)datasend);
     LoRa.endPacket();
}

int distF()
{
  //reading distance from the IRsensor
  return mySensor.distance();
}

void loop()
{
    new_time=millis();
    if (new_time - old_time >= 1000 || old_time == 0)
    {
      old_time = new_time;
      dhtWrite();
      if (distF() <= 50){
        SendData();
      }
      else Serial.print("");
    }

}
