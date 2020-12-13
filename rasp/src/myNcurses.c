#include "myNcurses.h"

WINDOW *windowImprimeDados,*windowEntradaUsuario,*windowImprimeErros;

char * connection;

extern struct atualizacao updateValues;
extern int tocaAlarme = 0;

void initNcurs()
{
    fp=fopen("relatorio.csv","w");
    fprintf(fp,"Comando, Alarme Ativado, Horário\n");
    fclose(fp);
    fp=fopen("relatorio.csv","a");
    
    initscr();
    cbreak();
    keypad(stdscr, TRUE);
    curs_set(0);

    createImprimeDadosWindow();
    createErrosWindow();
    createEntradaUsuarioWindow();
    
}

void  createEntradaUsuarioWindow(){
    int yMax, xMax;
    getmaxyx(stdscr, yMax, xMax);
    int tamXwindow = xMax/10 * 8, posX = 2;
    int tamanhoYwindowErro = 5;
    int tamanhoYwindowImprimeDados = yMax / 10 * 6;
    int tamanhoYwindowEntrada = 6;
    int posY =  tamanhoYwindowErro + tamanhoYwindowImprimeDados +2;
  

    windowEntradaUsuario = newwin(tamanhoYwindowEntrada, tamXwindow, posY, posX);
    mvwprintw(windowEntradaUsuario, 1, 1, "Escolha um comando de 0 ou 1 para as lampadas ou 2 para alarme\n");
    box(windowEntradaUsuario, 0, 0);
    wrefresh(windowEntradaUsuario);
}

void createImprimeDadosWindow(){
    int yMax, xMax;
    getmaxyx(stdscr, yMax, xMax);
    int tamXwindow = xMax/10 * 8, posX = 2;
    int tamanhoYwindow = yMax/10 * 6;

    windowImprimeDados = newwin(tamanhoYwindow, tamXwindow, 1, posX);
    
    box(windowImprimeDados, 0, 0);
    wrefresh(windowImprimeDados);
}

void createErrosWindow(){
    int yMax, xMax;
    getmaxyx(stdscr, yMax, xMax);
    int tamXwindow = xMax/10 * 8, posX = 2;
    int tamanhoYwindowErro = 5 ;
    int posY = yMax / 10 * 6 + 2;
   

    windowImprimeErros = newwin(tamanhoYwindowErro, tamXwindow, posY, posX);
    mvwprintw(windowImprimeErros, 1, 1, "Erros: ");
    box(windowImprimeErros, 0, 0);
    wrefresh(windowImprimeErros);
}


void * EntradaUsuario(void* parameters){

   
    mvwprintw(windowEntradaUsuario, 1, 1, "Escolha um comando de 0 ou 1 para as lampadas ou 2 para alarme");
    mvwprintw(windowEntradaUsuario, 4, 1, "Comando:");
    clearThenBox(0);
    
    int validation;
    while(mvwscanw(windowEntradaUsuario, 4, 9, "%d",&validation)){
       clearThenBox(1);
        
        while(validation<0|| validation> 2){
            mvwprintw(windowEntradaUsuario, 1, 1, "Comando Incorreto. Escolha um comando de 0 ou 1 para as lampadas ou 2 para alarme");
            clearThenBox(0);
            
            mvwscanw(windowEntradaUsuario, 4, 9, "%d",&validation);
            clearThenBox(1);
            
        }
        
        if(validation < 2)
        {
            gpioLigaEquipamentos(validation);
        }

        else 
        {
            tocaAlarme = 1;
        }

        mvwprintw(windowEntradaUsuario, 1, 1, "Escolha um comando de 0 ou 1 para as lampadas ou 2 para alarme");
        mvwprintw(windowEntradaUsuario, 4, 1, "Comando:");
        clearThenBox(1);
    }



    return NULL;
}

void * ImprimeDados(){

    char sensorsName [10][100] = {"SENSOR_PRESENCA_COZINHA",
                  "SENSOR_PRESENCA_SALA",
                  "SENSOR_ABERTURA_PORTA_COZINHA",
                  "SENSOR_ABERTURA_JANELA_COZINHA",
                  "SENSOR_ABERTURA_PORTA_SALA",
                  "SENSOR_ABERTURA_JANELA_SALA"};

    int yMax, xMax;
    getmaxyx(stdscr, yMax, xMax);

    

    while(1){
        time_t t = time(NULL);
        struct tm *tm = localtime(&t);
        
        wclear(windowImprimeDados);
        mvwprintw(windowImprimeDados, 1, 1, "Temperatura = %f     Umidade = %f     ",updateValues.temperatura,updateValues.umidade);
        
        for(int j=0;j<2;j++){
           
            mvwprintw(windowImprimeDados, j+3, xMax/10, "Estado Lâmpada %d = %d %*c %s = %d", j+1 , updateValues.machines[j].state,16,' ',sensorsName[j], updateValues.sensors[j].state);
            if(updateValues.sensors[j].state){
                fprintf(fp,"nenhum, %s , %s", sensorsName[j], asctime(tm));
            }
        }

        for(int j=2;j<6;j++){
          
            mvwprintw(windowImprimeDados, j + 3, xMax/10+39, "%s = % d ", sensorsName[j], updateValues.sensors[j].state);
            if(updateValues.sensors[j].state){
                fprintf(fp,"nenhum, %s , %s", sensorsName[j], asctime(tm));
            }
        }


        box(windowImprimeDados, 0, 0);
        wrefresh(windowImprimeDados);
        sleep(1);


    }

    return NULL;
}

void * Erros(void* parameters){

    while(1){
        wclear(windowImprimeErros);
        mvwprintw(windowImprimeErros, 1, 1, "Erros:");
        box(windowImprimeErros, 0, 0);
        wrefresh(windowImprimeErros);
        sleep(8);
    }

    return NULL;
}

void clearThenBox(int option){
    if(option){
        mvwprintw(windowEntradaUsuario, 4, 9, "%*c",20,' ');
    }
    box(windowEntradaUsuario, 0, 0);
    wrefresh(windowEntradaUsuario);

}

void printError(char erro[500]){

    wprintw(windowImprimeErros, "%s\n ",erro);
	box(windowImprimeErros, 0, 0);
	wrefresh(windowImprimeErros);

}


void setClientConection(char msg[500]){
    connection = msg;
}

void printClientConection(){
    int yMax,xMax;
    getmaxyx(windowImprimeDados, yMax, xMax);
    mvwprintw(windowImprimeDados, yMax-2,1, "Conexão do Cliente %s\n",connection);
}

