# AMQP Adapter for MQTT

This is an AMQP adapter for MQTT. It is designed internally to connect to the ClearBlade MQTT Broker.  
__NOTE:__ This adapter is still in beta and only supports basic functionality of the AMQP protocol. It only supports AMQP v0.9.1 and it is only tested with the RabbitMQ Java Client.  

### Installation  
- Clone this repository  
- You will need to have Eclipse Paho installed on your system. Please follow instructions given at https://eclipse.org/paho/clients/c/  
- After installing paho, run ```make clean``` and then ```make```  
- After successful compilation, you can start the adapter by running ```./main```  
- The adapter listens for incoming connections on port __5672___  