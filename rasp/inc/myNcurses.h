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


void initNcurs();
void  createEntradaUsuarioWindow();
void createImprimeDadosWindow();
void createErrosWindow();
void * EntradaUsuario1(void* parameters);
void EntradaUsuario2(char val[]);
void * ImprimeDados();
void * Erros(void* parameters);
void clearThenBox(int option);
void printError(char erro[500]);
void printClientConection();
void setClientConection(char msg[500]);
FILE * fp;



#endif