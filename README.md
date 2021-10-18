# LoRaControl

*MQTT Broker commands:
1.Start Mosquitto MQTT Broker : mosquitto
2.Subscribe to a topic: mosquitto_sub -t provider/topic name 
  #Ex: mosquitto_sub -t dragino-1ed4d0/#
3.Publish to a topic: mosquitto_pub -h host -t provider/channel -m DATAMESSAGE(text/ data) 
  #Ex: mosquitto_pub -h localhost -t dragino-1ed4d0/889988 -m DATA
