#ifndef MQTT_H
#define MQTT_H

#include <stdio.h>
#include <stdint.h>
#include <stddef.h>
#include <string.h>
#include "esp_system.h"
#include "esp_event.h"
#include "esp_netif.h"

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/semphr.h"
#include "freertos/queue.h"

#include "lwip/sockets.h"
#include "lwip/dns.h"
#include "lwip/netdb.h"

#include "esp_log.h"
#include "mqtt_client.h"
#include "led.h"
#include "cJSON.h"

void mqtt_start();

void mqtt_envia_mensagem(char * topico, char * mensagem);
void mqtt_inscricao(char *topico);
void conexaoEsp();
void getMacAddress();
void pega_Comodo_MQTT_DATA(char buffer[]);

#endif