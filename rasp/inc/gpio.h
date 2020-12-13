#ifndef _GPIO_H_
#define _GPIO_H_

#include <stdio.h>
#include <signal.h>
#include <stdlib.h>

#define LAMPADA_01 RPI_V2_GPIO_P1_11
#define LAMPADA_02 RPI_V2_GPIO_P1_12


#define SENSOR_PRESENCA_SALA RPI_V2_GPIO_P1_22
#define SENSOR_PRESENCA_COZINHA RPI_V2_GPIO_P1_37
#define SENSOR_ABERTURA_PORTA_COZINHA RPI_V2_GPIO_P1_29
#define SENSOR_ABERTURA_JANELA_COZINHA RPI_V2_GPIO_P1_31
#define SENSOR_ABERTURA_PORTA_SALA RPI_V2_GPIO_P1_32
#define SENSOR_ABERTURA_JANELA_SALA RPI_V2_GPIO_P1_36

#define HIGH 1
#define LOW 0

int gpioLigaEquipamentos(int option);
int init_bcm();
void gpioSensoresPresenca();
void configura_pinos();
void trata_interrupcao_gpio(void);

extern volatile int restartClient;

struct device{
    int port;
    int state;
};

struct atualizacao{
    float temperatura,umidade;
	struct device machines[10];
	struct device sensors[10];
};

void updated_Values();

#endif
