#include "led.h"

int estadoLed=0;

void inicializaLed(){
    gpio_pad_select_gpio(LED);
    gpio_set_direction(LED, GPIO_MODE_OUTPUT);
}


void ligaDesligaLed(){

    estadoLed=!estadoLed;
    gpio_set_level(LED, estadoLed);

}
