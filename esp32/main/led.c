#include "led.h"

int estadoLed=0;

void inicializaLed(){
    gpio_pad_select_gpio(LED);
    gpio_set_direction(LED, GPIO_MODE_OUTPUT);
}


void ligaDesligaLed(int estado){
    gpio_set_level(LED, estado);
    estadoLed=estado;
}
