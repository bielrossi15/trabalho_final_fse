// #include <stdio.h>
// #include <string.h>
// #include <stdlib.h>
// #include <MQTTClient.h>

// #include "mqtt.h"

// #define ADDRESS "tcp://mqtt.eclipseprojects.io:1883"
// #define CLIENTID "biel-ivan"

// MQTTClient client, client2;
// volatile MQTTClient_deliveryToken deliveredtoken;

// extern struct mqtt_client m_client[5];
// extern int controll;
// int count = 0;


// int msgarrvd(void *context, char *topicName, int topicLen, MQTTClient_message *message)
// {
//     clear();
//     char* payload = message->payload;
//     printf("Message arrived! \n\rTopic: %s Message: %s\n", topicName, payload);

//     char json[50] = "{\n\"comodo\":";

//     if(count == 0)
//     {
//         printf("Novo quarto para cadastrar!\n");
//         printf("Escolha um nome para cadastrar o novo quarto -> ");
//         scanf("%s", &m_client[count].room);
//         printf("\n");

//         char json_mid[20] = "\"170013278\/";
//         char json_end[10] = "\"\n}";
//         strcat(json, json_mid);
//         strcat(json, m_client[count].room);
//         strcat(json, json_end);

//         printf("%s\n\n", json);
//         count++;
//         mqtt_publish(client, topicName, json);
//     }   
    
//     MQTTClient_freeMessage(&message);
//     MQTTClient_free(topicName);

//     clear();
//     return 1;
// }

// void connlost(void *context, char *cause)
// {
//     printf("\nConnection lost\n");
//     printf("     cause: %s\n", cause);
// }

// void delivered(void *context, MQTTClient_deliveryToken dt)
// {
//     printf("Message with token value %d delivery confirmed\n", dt);
//     deliveredtoken = dt;
// }

// int mqtt_configuration()
// {
//     MQTTClient_connectOptions conn_opts = MQTTClient_connectOptions_initializer;
//     int rc;

    
//     MQTTClient_create(&client, ADDRESS, CLIENTID, MQTTCLIENT_PERSISTENCE_NONE, NULL);

//     conn_opts.keepAliveInterval = 20;
//     conn_opts.cleansession = 1;
    
//     MQTTClient_setCallbacks(client, NULL, NULL, msgarrvd, NULL);

//     rc = MQTTClient_connect(client, &conn_opts);

//     if (rc != MQTTCLIENT_SUCCESS)
//     {
//         printf("\n\rFalha na conexao ao broker MQTT. Erro: %d\n", rc);
//         exit(-1);
//     }

//     MQTTClient_subscribe(client, "fse2020/170013278/dispositivos/#", 1);

//     int ch;

//     while(1)
//     {

//     }

//     disconnect();
// }

// void mqtt_publish(MQTTClient client, char* topic, char* payload)
// {
//     MQTTClient_message pubmsg = MQTTClient_message_initializer;
//     MQTTClient_deliveryToken token;

//     pubmsg.payload = payload;
//     pubmsg.payloadlen = strlen(pubmsg.payload);
//     pubmsg.qos = 1;
//     pubmsg.retained = 0;
//     MQTTClient_publishMessage(client, topic, &pubmsg, &token);
//     MQTTClient_waitForCompletion(client, token, 1000L);
// }

// void mqtt_subscribe()
// {
//     MQTTClient_connectOptions conn_opts = MQTTClient_connectOptions_initializer;
//     int rc;

    
//     MQTTClient_create(&client2, ADDRESS, CLIENTID, MQTTCLIENT_PERSISTENCE_NONE, NULL);

//     conn_opts.keepAliveInterval = 20;
//     conn_opts.cleansession = 1;
    
//     MQTTClient_setCallbacks(client2, NULL, NULL, msgarrvd, NULL);

//     rc = MQTTClient_connect(client2, &conn_opts);

//     if (rc != MQTTCLIENT_SUCCESS)
//     {
//         printf("\n\rFalha na conexao ao broker MQTT. Erro: %d\n", rc);
//         exit(-1);
//     }

//     char topic[25] = "fse2020/170013278/";
//     // for(int i = 0; i < count; i++)
//     // {
//     char new_topic[70];
//     strcat(topic, m_client[count].room);
//     strcat(new_topic, '#');
//     printf("%s\n", new_topic);
//     MQTTClient_subscribe(client2, new_topic, 1);
//     // }
// }

// void disconnect()
// {
//     MQTTClient_disconnect(client, 10000);
//     MQTTClient_destroy(&client);
// }
