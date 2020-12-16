#ifndef _MYNCURSES_H_
#define _MYNCURSES_H_

#include <stdio.h>
#include <stdlib.h>
#include <ncurses.h>
#include <curses.h>
#include <unistd.h>
#include <pthread.h>
#include <string.h>
#include "gpio.h"
#include "mqtt.h"

void initNcurs();

void * EntradaUsuario(void* parameters);
void * ImprimeDados();
void * Erros(void* parameters);

void  createEntradaUsuarioWindow();
void createImprimeDadosWindow();
void createErrosWindow();
void clearThenBox(int option);
void printError(char erro[500]);
void printClientConection();
void setClientConection(char msg[500]);

void enviaNomeComodo();
void enviaComandoAparelhos(int validation);
FILE * fp;



#endif