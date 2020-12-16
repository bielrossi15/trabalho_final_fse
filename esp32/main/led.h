#ifndef LED_H
#define LED_H


#include <stdio.h>
#include "sdkconfig.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "driver/gpio.h"

#define LED 2

void inicializaLed();
void ligaDesligaLed(int estado);

#endif