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
    if(ptr == NULL){
        printError("Null pointer");
    }else if(!strcmp(ptr, "dispositivos")){
       
        ptr = strtok(NULL, delim); // mac

        if(count_dispositivos == 5){
            printError("Número máximo de esps atingindo");
            return 1;   
        }
        
        cJSON * json = cJSON_Parse(message->payload);
        cJSON * id = cJSON_GetObjectItem(json, "id");
        
        char * rendered = cJSON_Print(json);

        if(id)
        {
            strcpy(mqtt_device[count_dispositivos].id, ptr);
            cadastrar_dispositivo = 1;
            screen_controler = 1;
        }
        
    } else{

        cJSON * json = cJSON_Parse(message->payload);
        char *id = cJSON_GetObjectItemCaseSensitive(json, "id")->valuestring;
        
        int result = findByid(id);

        ptr = strtok(NULL, delim);
        if(!strcmp(ptr, "temperatura")){
            if(cJSON_GetObjectItemCaseSensitive(json, "temperatura")!=NULL){
                mqtt_device[result].temp = cJSON_GetObjectItemCaseSensitive(json, "temperatura")->valueint;
            }
            else{
                printError("Envio incorreto");
            }
                
        }
        else if(!strcmp(ptr, "umidade")){
            if(cJSON_GetObjectItemCaseSensitive(json, "umidade")!=NULL){
                mqtt_device[result].hmd = cJSON_GetObjectItemCaseSensitive(json, "umidade")->valueint;
            }
            else{
                printError("Envio Incorreto");
            }

        }
        else if(!strcmp(ptr, "estado")){
            if(cJSON_GetObjectItemCaseSensitive(json, "entrada")!=NULL){
                mqtt_device[result].in_state = cJSON_GetObjectItemCaseSensitive(json, "entrada")->valueint;
            } else {
                printError("Envio Incorreto");
            }
            if(cJSON_GetObjectItemCaseSensitive(json, "saida")!=NULL){
                mqtt_device[result].out_state = cJSON_GetObjectItemCaseSensitive(json, "saida")->valueint;
            } else {
                printError("Envio Incorreto");
            }
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

int findByid(char *id)
{
    int result;
    for(int i=0;i<count_dispositivos;i++){
            if(!strcmp(mqtt_device[i].id,id)){
                result = i;
                return result;
                
            }
    }
    return -1;
}