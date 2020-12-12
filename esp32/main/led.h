#ifndef LED_H
#define LED_H

void conectadoWifi(void *params);
void trataComunicacaoComServidor(void *params);

#include <stdio.h>
#include "sdkconfig.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "driver/gpio.h"

#define LED 2

void inicializaLed();
void ligaDesligaLed();

#endif