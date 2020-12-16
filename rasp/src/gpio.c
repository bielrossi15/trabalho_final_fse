#include "gpio.h"
#include <bcm2835.h>


struct device sensors[] = {
    {SENSOR_PRESENCA_COZINHA,LOW}, 
    {SENSOR_PRESENCA_SALA,LOW}, 
    {SENSOR_ABERTURA_PORTA_COZINHA,LOW},
    {SENSOR_ABERTURA_JANELA_COZINHA,LOW}, 
    {SENSOR_ABERTURA_PORTA_SALA,LOW}, 
    {SENSOR_ABERTURA_JANELA_SALA,LOW}};

struct device machines[] = {
    {LAMPADA_01,LOW},
    {LAMPADA_02,LOW}};


volatile int restartClient;
int sensorsSize = 8,machinesSize=6;

extern struct atualizacao updateValues;
extern int tocaAlarme;

int gpioLigaEquipamentos(int option)
{
    bcm2835_gpio_write(sensors[0].port, 1);
    bcm2835_gpio_write(machines[option].port, !machines[option].state);
    machines[option].state = !machines[option].state;


    return 1;
}

void gpioSensoresPresenca(){
       
    struct atualizacao * rapidUpdate = malloc(sizeof(struct atualizacao));
    rapidUpdate->temperatura = -1.0;
    int cont=0;
    for(int i=0;i<sensorsSize;i++){
    
        sensors[i].state = bcm2835_gpio_lev(sensors[i].port);
        if(sensors[i].state){
            cont++;
            rapidUpdate->sensors[i].state = sensors[i].state;
        }
    }

    if(cont > 0 && tocaAlarme == 1){
        //system("omxplayer --no-keys example.mp3 > /dev/null 2>&1 & ");
        // printf("tocou\n");
    }
}

int init_bcm(){
    if (!bcm2835_init()){
        return 1;
    }
   // Configura pinos dos LEDs como saídas
    
    for(int i=0;i<machinesSize;i++){
        bcm2835_gpio_fsel(machines[i].port, BCM2835_GPIO_FSEL_OUTP);
        bcm2835_gpio_write(machines[i].port, LOW);

    }

    for(int i=0;i<sensorsSize;i++){
        bcm2835_gpio_fsel(sensors[i].port, BCM2835_GPIO_FSEL_OUTP);
        //bcm2835_gpio_write(sensors[i].port, LOW);
    }

    return 0;
}

void trata_interrupcao_gpio(void)
{
    for(int i=0;i<machinesSize;i++){
        bcm2835_gpio_write(machines[i].port, LOW);
    }
    
    
    fprintf(stderr,"fechei a gpio\n");
    bcm2835_close();
    
}

void updated_Values(){


    for (int i = 0; i < 2; i++)
    {
       updateValues.machines[i].state = machines[i].state;
       updateValues.machines[i].port = machines[i].port;
    }

    for (int i = 0; i < 6; i++)
    {
       updateValues.sensors[i].state = sensors[i].state;
       updateValues.sensors[i].port = sensors[i].port;
    }
}