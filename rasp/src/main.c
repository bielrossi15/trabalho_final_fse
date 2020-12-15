#include "main.h"
#include "mqtt.h"

#include <bcm2835.h>
#include <errno.h>

pthread_mutex_t lock1 = PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t lock2 = PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t lock3 = PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t lock4 = PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t lock5 = PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t lock6 = PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t lock9 = PTHREAD_MUTEX_INITIALIZER;

pthread_t t0, t1, t2,t3,t4,t5,t6,t7,t8;

int contador = 0,contador2=0 ,contador3=0;
volatile int restartClient=1;
float * tempHumidity;
int keepThreading = 1;
struct atualizacao *update;

volatile double userDefinedTemp = 100.5;
int tempControlledbyUser;
extern int cadastrar_dispositivo;

pthread_t t0,t1,t2,t3;

struct atualizacao updateValues;

struct mqtt_client mqtt_device[5];

int main(){
    update=  malloc(sizeof(struct atualizacao)); 

    tempHumidity =  (float*) calloc(5,sizeof(float));
    while(init_bcm());
    while(initI2C());

    signal(SIGINT, trataInterrupcao);
    signal(SIGALRM, trataSinalAlarme);

    initNcurs();
    mqtt_configuration();


    pthread_create(&t1, NULL, gpioSensores, NULL);
    pthread_create(&t2, NULL, i2c_TemperaturaUmidade, NULL);
    // pthread_create(&t3, NULL, sendUpdate, NULL);
    // pthread_create(&t4, NULL, connectClient, NULL);
    // pthread_create(&t5, NULL, regulateTemperature, NULL);
    pthread_create(&t6, NULL, ImprimeDados, NULL);
    pthread_create(&t7, NULL, EntradaUsuario1, NULL);
    pthread_create(&t8, NULL, Erros, NULL);
    // pthread_create(&t9, NULL, EntradaUsuario2, NULL);

    pthread_mutex_lock(&lock9);
    pthread_mutex_lock(&lock5);
    pthread_mutex_lock(&lock6);
    
    ualarm(100000, 100000);
   
    while(keepThreading)
    {
        while(contador3<20); 
        contador3=0;
    }


    return 0;
}


void trataInterrupcao(int sig)
{
    keepThreading=0;
   
    clear();
    endwin();
    fclose(fp);
    ualarm(0,0);
    keepThreading=0;
    
    pthread_cancel(t1);
    pthread_cancel(t2);
    pthread_mutex_unlock(&lock1),
    pthread_mutex_unlock(&lock2);
    pthread_mutex_unlock(&lock6);
    pthread_mutex_destroy(&lock1);
    pthread_mutex_destroy(&lock2);
    pthread_mutex_destroy(&lock6);
    
    pthread_join(t1,NULL);
    pthread_join(t2,NULL);

    
    //fprintf(stderr,"Passei das threads\n");
    trata_interrupcao_gpio();
    trata_interrupcao_I2C();
    free(update);
    free(tempHumidity);

    exit(0);
}

void *gpioSensores(void *arg)
{
    while (keepThreading)
    {
        pthread_mutex_lock(&lock1);
        gpioSensoresPresenca();
    }

    return NULL;
}

void trataSinalAlarme(int sinal)
{
    pthread_mutex_unlock(&lock1);
    if(contador==8){
        pthread_mutex_unlock(&lock2); 
        contador=0;
    }

    if(contador2==10){
        updated_Values();
        pthread_mutex_unlock(&lock6); 
        pthread_mutex_unlock(&lock5);
        contador2=0;
    }

    if(contador3==20){
        pthread_mutex_unlock(&lock4);  
        contador3=0;  
    }
    
   
    contador++;
    contador2++;
    contador3++;
}


void * i2c_TemperaturaUmidade(){
    //get external temperature
    float *temp = malloc(sizeof(float)*5);
    while(keepThreading){
        pthread_mutex_lock(&lock2);
        temp = temperature_humidity();
        if(temp[0]>0){
            updateValues.temperatura = temp[0]; 
        }
        if(temp[1]>0){
            updateValues.umidade = temp[1]; 
        }

    }
    return NULL;
}