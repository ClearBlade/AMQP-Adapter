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
import java.io.IOException;
import java.net.URISyntaxException;
import java.security.KeyManagementException;
import java.security.NoSuchAlgorithmException;
import java.util.concurrent.TimeoutException;

import com.rabbitmq.client.ConnectionFactory;
import com.rabbitmq.client.Connection;
import com.rabbitmq.client.Channel;

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
Replace the ```CLEARBLADE_AUTH-TOKEN```, ```CLEARBLADE-SYSTEM-KEY```, ```CLIENT-ID```, ```CLEARBLADE-MESSAGING-URL```, ```CLEARBLADE-MQTT-PORT``` and ```ADAPTER-IP``` with your own. An example of the ```CLEARBLADE-MESSAGING-URL``` can be __platform.clearblade.com__ and that of the ```CLEARBLADE-MQTT-PORT``` would be __1883__.   

#### Publishing Messages  
```java
String topic = "YOUR_TOPIC";
String message = "YOUR_MESSAGE";

channel.basicPublish("", topic, null, message.getBytes());
```

Replace ```YOUR_TOPIC``` and ```YOUR_MESSAGE``` with the topic and message of your choice.  

#### Subscribing to Topics  
```java
import com.rabbitmq.client.AMQP;
import com.rabbitmq.client.Consumer;
import com.rabbitmq.client.DefaultConsumer;
import com.rabbitmq.client.Envelope;

String topic = "YOUR_TOPIC";

Consumer consumer = new DefaultConsumer(channel) {
  @Override
	public void handleDelivery(String consumerTag, Envelope envelope, AMQP.BasicProperties properties, byte[] body)
	throws IOException {
	  String message = new String(body, "UTF-8");
		System.out.println(" [x] Received '" + message + "'");
		}
	};
	
channel.basicConsume(topic, true, consumer);
```
Replace ```YOUR_TOPIC``` with the topic of your choice.  

#### Disconnecting from the Adapter  
```java
channel.close();
conn.close();
```  



