#ifndef SALVA_NVS_H_
#define SALVA_NVS_H_

#include <stdio.h>
#include <string.h>
#include "esp_log.h"
#include "nvs.h"
#include "nvs_flash.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_system.h"

int le_valor_nvs();
void grava_valor_nvs();

#endif