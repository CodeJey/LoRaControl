#include <SharpIR.h>
#include <DHT.h>
#include <SPI.h>
#include <LoRa.h>

//defining pins for sensors
#define DHTPIN1 3
#define DHTPIN2 4
#define DHTTYPE1 DHT11 
#define DHTTYPE2 DHT22
//defining the semsors 
DHT dht1(DHTPIN1, DHTTYPE1);
DHT dht2(DHTPIN2, DHTTYPE2);
//def the IR distance sensor pin
#define IRPin A0
#define model 20150
//array for sensor data
char tem_1[8]={"\0"},hum_1[8]={"\0"}, tem_2[8]={"\0"},hum_2[8]={"\0"}, dis[8]={"\0"};
//def the node id(BOARD)
const char *node_id = ("<4567>");  //From LG01 via web Local Channel settings on MQTT.Please refer <> dataformat in here. 
//def package length
uint8_t datasend[42];
//counter start
unsigned int count = 1; 
//refresh time for receiving/sending
unsigned long new_time,old_time=0;
//def the IR distance sensor
SharpIR mySensor = SharpIR(IRPin, model);

void setup()
{
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
      
      LoRa.onReceive(onReceive);   
      LoRa.receive();
}

void dhtTem()
{
       //dht data method
       float tem = dht1.readHumidity();      
       float hum = dht1.readTemperature();             
       Serial.println(F("The temperature and humidity:"));
       Serial.print("[");
       Serial.print(tem);
       Serial.print("℃");
       Serial.print(",");
       Serial.print(hum);
       Serial.print("%");
       Serial.print("]");
       Serial.println("");
}
int distF()
{
      //distance sensor f
      return mySensor.distance();
}
void dhtWrite()
{
      //reading data
    float tem,hum,tem1,hum1;
    tem = dht1.readTemperature();
    delay(100);
    tem1 = dht2.readTemperature();
    delay(100);
    hum = dht1.readHumidity();
    delay(100);
    hum1 = dht2.readHumidity();
    char data[50] = "\0";
      //setting data
    for(int i = 0; i < 50; i++)
    {
       data[i] = node_id[i];
    }
      //setting data seq in the data array
    dtostrf(tem,0,1,tem_1);
    dtostrf(hum,0,1,hum_1);
    dtostrf(tem1,0,1,tem_2);
    dtostrf(hum1,0,1,hum_2);
    dtostrf(distF(),0,1,dis);

    // Serial.println(tem_1);
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
     
   //Serial.println((char *)datasend);
    //Serial.println(sizeof datasend);
      
}


void SendData()
{
     LoRa.beginPacket();
     LoRa.print((char *)datasend);
     LoRa.endPacket();
     //Serial.println("Packet Sent");
//     Serial.write("sys get hweui\r\n"); delay(1000); 
//      while(Serial.available()) Serial.write(Serial.read());
}    
    


void loop()
{
    new_time=millis();
    if (new_time - old_time >= 1000 || old_time == 0)
    {
      old_time = new_time;
//      Serial.print("###########    ");
//      Serial.print("COUNT=");
//      Serial.print(count);
//      Serial.println("    ###########");
      count++;
      //dhtTem();
      dhtWrite();
      if (distF() <= 50){
        SendData();
        
      }
      else Serial.print("");
      LoRa.receive();
    }
}

void onReceive(int packetSize) {
 
  // received a packet
  Serial.print("Received packet : ");

  // read packet
  for (int i = 0; i < packetSize; i++) {
      Serial.print((char)LoRa.read());
  }
  Serial.print("\n\r");  
}
