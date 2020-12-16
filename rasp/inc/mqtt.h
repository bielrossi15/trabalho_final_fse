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
};
struct entry mapa[10];

int findByid(char *id);
void mqtt_configuration();
void mqtt_publish(char* topic, char* payload);
void mqtt_subscribe(char * topic);
#endif