#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <MQTTClient.h>

#include "mqtt.h"
#include "cJSON.h"

#define ADDRESS "tcp://mqtt.eclipseprojects.io:1883"
#define CLIENTID "biel-ivan"

MQTTClient client;
volatile MQTTClient_deliveryToken deliveredtoken;

struct mqtt_client mqtt_device[5];
extern int controll;
int count = 0;
extern int screen_controler;
int dispositivos_para_registrar = 0;
int count_dispositivos = 0;
int cadastrar_dispositivo = 0;
int control_publish_message = 0;

int msgarrvd(void *context, char *topicName, int topicLen, MQTTClient_message *message)
{
    char* payload = message->payload;
    // while(1) printf("Message arrived! \n\rTopic: %s Message: %s\n", topicName, payload);

    char temp[1024];
    char delim[2] = "/";
    strcpy(temp, topicName);
    char *ptr = strtok(temp, delim); // fse
    ptr = strtok(NULL, delim); // matricula
    ptr = strtok(NULL, delim); // dispositivo || comodo
    char teste [100];
    strcpy(teste,ptr);
    printError(teste);
    
    if(ptr == NULL){
        //coco
        printf("error\n");
    }else if(!strcmp(ptr, "dispositivos")){
       
        ptr = strtok(NULL, delim); // mac

        //if(dispositivos_para_registrar == 5) return 1;
        cJSON * json = cJSON_Parse(message->payload);
        cJSON * id = cJSON_GetObjectItem(json, "id");
        
        char * rendered = cJSON_Print(json);

        if(id)
        {
            strcpy(mqtt_device[count_dispositivos].id, ptr);
            dispositivos_para_registrar++;
            cadastrar_dispositivo = 1;
            screen_controler = 1;
        }
        
    } else{
        int result = number_for_key(ptr);
        char test[100];
        printError(test);
        sprintf(test,"%d",result);
        cJSON * json = cJSON_Parse(message->payload);
        mqtt_device[result].in_state = cJSON_GetObjectItemCaseSensitive(json, "entrada")->valueint;
        if(cJSON_GetObjectItemCaseSensitive(json, "saida")!=NULL){
            mqtt_device[result].out_state = cJSON_GetObjectItemCaseSensitive(json, "saida")->valueint;
            mqtt_device[result].temp = cJSON_GetObjectItemCaseSensitive(json, "temperatura")->valueint;
             mqtt_device[result].hmd = cJSON_GetObjectItemCaseSensitive(json, "umidade")->valueint;
        }
        
    }
    
    
    MQTTClient_freeMessage(&message);
    MQTTClient_free(topicName);

    return 1;
}

void mqtt_configuration()
{
    MQTTClient_connectOptions conn_opts = MQTTClient_connectOptions_initializer;
    int rc;

    
    MQTTClient_create(&client, ADDRESS, CLIENTID, MQTTCLIENT_PERSISTENCE_NONE, NULL);

    conn_opts.keepAliveInterval = 20;
    conn_opts.cleansession = 1;
    
    MQTTClient_setCallbacks(client, NULL, NULL, msgarrvd, NULL);

    rc = MQTTClient_connect(client, &conn_opts);

    if (rc != MQTTCLIENT_SUCCESS)
    {
        printf("\n\rFalha na conexao ao broker MQTT. Erro: %d\n", rc);
        exit(-1);
    }

    MQTTClient_subscribe(client, "fse2020/170013278/dispositivos/#", 1);
}

void mqtt_publish(char* topic, char* payload)
{
    MQTTClient_message pubmsg = MQTTClient_message_initializer;
    MQTTClient_deliveryToken token;

    pubmsg.payload = payload;
    pubmsg.payloadlen = strlen(pubmsg.payload);
    pubmsg.qos = 1;
    pubmsg.retained = 0;
    MQTTClient_publishMessage(client, topic, &pubmsg, &token);
    MQTTClient_waitForCompletion(client, token, 1000L);
}

void mqtt_subscribe(char * topic){
     MQTTClient_subscribe(client, topic, 1);
}

int number_for_key(char *key)
{
    printError("entrei no number for key");
    int i = 0;
    printError(mapa[i].str);
    char test1[500];
    sprintf(test1, "%d", mapa[i].n);
    printError(test1);
    char *name = mapa[i].str;
    if(name == NULL){
        printError("NOME TA NULL");
    }
    while (name) {
        if (strcmp(name, key) == 0){
            printError(name);
            printError(key);
            return mapa[i].n;
        }
        name = mapa[++i].str;
    }
    return 0;
}