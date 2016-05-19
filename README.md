# AMQP Adapter for MQTT

This is an AMQP adapter for MQTT. It is designed internally to connect to the ClearBlade MQTT Broker.  
__NOTE:__ This adapter is still in beta and only supports basic functionality of the AMQP protocol. It only supports AMQP v0.9.1 and it is only tested with the RabbitMQ Java Client.  

### Installation  
- Clone this repository  
- You will need to have Eclipse Paho installed on your system. Please follow instructions given at https://eclipse.org/paho/clients/c/  
- After installing paho, run ```make clean``` and then ```make```  
- After successful compilation, you can start the adapter by running ```./main```  
- The adapter listens for incoming connections on port __5672__ 

### Usage  

AMQP clients can connect to this adapter and the adapter will convert and forward the AMQP packets to the ClearBlade MQTT Broker. Make sure you have the RabbitMQ Java Client installed. It can be found at https://www.rabbitmq.com/api-guide.html. 
You must use the exact syntax of the RabbitMQ Java client as described below:

#### Connecting to the Adapter  
```java
String username = "CLEARBLADE_AUTH-TOKEN";
String systemKey = "CLEARBLADE-SYSTEM-KEY";
String mqttClientID = "CLIENT-ID";
String messagingURL = "CLEARBLADE-MESSAGING-URL";
String mqttPort = "CLEARBLADE-MQTT-PORT";
String adapterIP = "ADAPTER-IP";

String password = systemKey + "-" + mqttClientID + "-" + messagingURL + "-" + mqttPort;

ConnectionFactory factory = new ConnectionFactory(); 
factory.setUri("amqp://" + username + ":" + password + "@" + adapterIP + ":5672");

Connection conn = factory.newConnection();
Channel channel = conn.createChannel();
```  
Replace the CLEARBLADE_AUTH-TOKEN, CLEARBLADE-SYSTEM-KEY, CLIENT-ID, CLEARBLADE-MESSAGING-URL, CLEARBLADE-MQTT-PORT and ADAPTER-IP with your own. An example of the CLEARBLADE-MESSAGING-URL can be platform.clearblade.com and that of the CLEARBLADE-MQTT-PORT would be 1883. 

