#include <MQTTClient.h>

#ifndef MQTT_H
#define MQTT_H

struct mqtt_client 
{
    char id[50];
    char room[50];
    char in[50];
    char out[50];
    char state;
    double temp;
    double hmd;
};

int mqtt_configuration();
void mqtt_subscribe();
void mqtt_publish(MQTTClient client, char* topic, char* payload);

#endif