#ifndef BOTAO_H_
#define BOTAO_H_

#include <stdio.h>
#include <string.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/queue.h"
#include "esp_log.h"
#include "driver/gpio.h"
#include "main.h"
#include "mqtt.h"
#define BOTAO 0

void initializaBotao();
void trataInterrupcaoBotao(void *params);


#endif