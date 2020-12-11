#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <signal.h>
#include <time.h>
#include <unistd.h>
#include <pthread.h>
#include <semaphore.h>

// including my own files
#include "i2c.h"
#include "gpio.h"


// functions to handle signals
void sig_handler(int signal);
void alarm_handler(int signal);

// functions to execute on threads
void * i2c();
void * get_sensor_values();
void * client_handler();
void * server_handler();
void * sensor_update();
void * temperature_handler();

double T = 0.0,
      H = 0.0,
      AC_TEMP = 100.0;

int lamp[4],
    sp[2],
    so[4],
    sp_counter[2],
    count = 0;

pthread_t t0, t1, t2, t3, t4, t5;
FILE *file;
sem_t sem;


int main(int argc, const char * argv[])
{
    sp_counter[0] = 0;
    sp_counter[1] = 0;

    configure_pins();

    // all handled signals
    signal(SIGINT, sig_handler);
    signal(SIGTERM, sig_handler);
    signal(SIGTSTP, sig_handler);
    signal(SIGALRM, alarm_handler);
    alarm(1);

    while(1)
    {
        menu();
    }

    return 0;
}

void sig_handler(int signal)
{
    printf("\nReceived signal %d, terminating program...\n", signal);
    alarm(0);
    pthread_cancel(t0);
    close_sockets();
    close_socket();
    set_lamp_state(0, 1);
    set_lamp_state(0, 2);
    set_lamp_state(0, 3);
    set_lamp_state(0, 4);
    set_ac_state(0, 1);
    set_ac_state(0, 2);
    bcm2835_close();
    exit(0);
}

void alarm_handler(int signal)
{
    pthread_create(&t0, NULL, i2c, NULL);
    pthread_create(&t1, NULL, get_sensor_values, NULL);
    alarm(1);
}   

void * i2c()
{
    initialize_bme("/dev/i2c-1", &T, &H);
}

void * get_sensor_values()
{
    get_state(lamp, sp, so);
}


// void * sensor_status()
// {
//     for(int i = 0; i < 2; i++)
//     {
//         if(sp[i] != sp_old[i] && sp[i] == 1)
//         {
//             system("omxplayer ../4cd33cb95f308477d770210720e9d74d1ab0fa58.mp3 > /dev/null 2>&1");
//             file_write(3, 0, 0, 0.0);
//         }
//         sp_old[i] = sp[i];
//     }

//     for(int i = 0; i < 6; i++)
//     {
//         if(so[i] != so_old[i] && so[i] == 1)
//         {
//             system("omxplayer ../4cd33cb95f308477d770210720e9d74d1ab0fa58.mp3 > /dev/null 2>&1");
//             file_write(3, 0, 0, 0.0);
//         }

//         so_old[i] = so[i];
//     }

// }

void menu()
{
    
    short cs = 0;

    int l, on_off;
    double temp, h;

    clear_outputs();

    printf("========== INTERACTIVE MENU ==========\n  1 -> Print values\n  2 -> Turn on/off lamps\n ======================================\n");
    printf("-> ");
    scanf("%hd", &cs);
    printf("\n");
    switch (cs)
    {
        case 1:
            printf("T -> %lf\nH -> %lf\n", H, T);
            
            for(int i = 0; i < *(&lamp + 1) - lamp; i++)
                printf("LAMP_%d -> %d\n", i+1, lamp[i]);

            for(int i = 0; i < *(&sp + 1) - sp; i++)
                printf("SP_%d -> %d\n", i+1, sp[i]);
            
            for(int i = 0; i < *(&so + 1) - so; i++)
                printf("SO_%d -> %d\n", i+1, so[i]);

            char q;
            printf("PRESS 'q' TO RETURN TO MENU");
        
            while(1)
            {
                scanf("%c", &q);
                printf("\n");

                if(q == 'q')
                {
                    clear_outputs();
                    break;
                }
            }

        break;

        case 2:
            printf("Wich lamp you like to control? (1-2)\n");
            scanf("%d", &l);

            printf("0 - off 1 - On\n");
            scanf("%d", &on_off);
            
            set_lamp_state(on_off, l);
            char j;
            printf("PRESS 'q' TO RETURN TO MENU");

            while(1)
            {
                scanf("%c", &j);
                printf("\n");

                if(j == 'q')
                {
                    clear_outputs();
                    break;
                }
            }
            file_write(0, l, on_off);
        break;

        default:
        break;
        }
}

void clear_outputs() 
{
    #if defined _WIN32
        system("cls");
    #elif defined (__LINUX__) || defined(__gnu_linux__) || defined(__linux__)
        system("clear");
    #elif defined (__APPLE__)
        system("clear");
    #endif
}