#include <MQTTClient.h>

#ifndef MQTT_H
#define MQTT_H
#include "myNcurses.h"
struct mqtt_client 
{
    char id[50];
    char room[50];
    char in[50];
    char out[50];
    int in_state;
    int out_state;
    int temp;
    int hmd;
};

struct entry {
    char *str;
    char macAdress[6][20];
};
struct entry mapa[10];

int number_for_key(char *key);
void mqtt_configuration();
void mqtt_publish(char* topic, char* payload);
void mqtt_subscribe(char * topic);
#endif