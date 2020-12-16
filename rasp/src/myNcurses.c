#include "myNcurses.h"

WINDOW *windowImprimeDados,*windowEntradaUsuario,*windowImprimeErros;

char * connection;

extern struct atualizacao updateValues;
int tocaAlarme = 0;
extern struct mqtt_client mqtt_device[5];
int screen_controler= 0;
extern int count_dispositivos;
extern int cadastrar_dispositivo;

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

    mvwprintw(windowEntradaUsuario, 1, 1, "Escolha um comando de 0 para controlar as lampadas, 1 para controlar as esps e 2 para alarme");
    mvwprintw(windowEntradaUsuario, 4, 1, "Comando:");
    clearThenBox(0);
    
    int validation;
    while(mvwscanw(windowEntradaUsuario, 4, 9, "%d ", &validation)){

        if(cadastrar_dispositivo == 1)
        {
            enviaNomeComodo();
        }

        else
        {
            if(validation==0){
                clearThenBox(1);
                enviaComandoAparelhos();
            }
            else if(validation==1){
                clearThenBox(1);
                ligaDesligaLed();
            } 
            else 
            {
                tocaAlarme = 1;
            } 
        }
        mvwprintw(windowEntradaUsuario, 1, 1, "Escolha um comando de 0 para controlar as lampadas, 1 para controlar as esps e 2 para alarme");
        mvwprintw(windowEntradaUsuario, 4, 1, "Comando:");
        clearThenBox(1);
    }

    return NULL;
}

void ligaDesligaLed(){
    int validation;
    mvwprintw(windowEntradaUsuario, 1, 1, "Escolha o número do cômodo%*c",65,' ');
    mvwscanw(windowEntradaUsuario, 4, 9, "%d ", &validation);
    clearThenBox(1);
    validation--;
    
    char topic[512];
    char data[512];

    sprintf(topic, "fse2020/170013278/dispositivos/%s", mqtt_device[validation].id); 
    mqtt_device[validation].out_state=!mqtt_device[validation].out_state;
    sprintf(data, "{ \"saida\": %d}",mqtt_device[validation].out_state);
    mqtt_publish(topic, data);
   
}

void enviaComandoAparelhos()
{   
    int validation;
    mvwprintw(windowEntradaUsuario, 1, 1, "Escolha a lâmpada%*c",74,' ');
    mvwscanw(windowEntradaUsuario, 4, 9, "%d ", &validation);
    clearThenBox(1);

        while(validation<0|| validation> 2)
        {
            mvwprintw(windowEntradaUsuario, 1, 1, "Comando Incorreto. Escolha um comando de 0 ou 1 para as lampadas ou 2 para alarme");
            clearThenBox(0);
            
            mvwscanw(windowEntradaUsuario, 4, 9, "%d",&validation);
            clearThenBox(1);
        }
        
        if(validation < 2)
        {
            gpioLigaEquipamentos(validation);
        }

       
            
       
}
void enviaNomeComodo()
{
    int yMax, xMax;
    getmaxyx(stdscr, yMax, xMax);
    char topic[512];
    char data[512];

    clearThenBox(1);

    sprintf(topic, "%s%s", "fse2020/170013278/dispositivos/", mqtt_device[count_dispositivos].id);
    wrefresh(windowEntradaUsuario);
    mvwprintw(windowEntradaUsuario, 1, 1, "Escolha um nome para o novo comodo%*c", 60, ' ');
    mvwprintw(windowEntradaUsuario, 4, 1, "Comodo: ");
    mvwscanw(windowEntradaUsuario, 4, 9, "%s", &mqtt_device[count_dispositivos].room);

    sprintf(data, "{ \"comodo\": \"%s\"}", mqtt_device[count_dispositivos].room);
    mqtt_publish(topic, data);
    clearThenBox(1);
    char inscreveComodo[512];
    sprintf(inscreveComodo,"fse2020/170013278/%s/#",mqtt_device[count_dispositivos].room);
    mapa[count_dispositivos].str = mqtt_device[count_dispositivos].room;
    mapa[count_dispositivos].n = count_dispositivos;
    mqtt_subscribe(inscreveComodo);


    cadastrar_dispositivo = 0;
    count_dispositivos++;
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

        if(count_dispositivos > 0)
        {
            for(int j = 4,i=0; i < count_dispositivos; j++,i++)
            {
                mvwprintw(windowImprimeDados, (j - 4) + 6, xMax/(xMax - 5), "%d. %s -> T %d H %d IN %d OUT %d\n\n",i+1, mqtt_device[i].room, mqtt_device[i].temp, mqtt_device[i].hmd, mqtt_device[i].in_state, mqtt_device[i].out_state);
                // mvwprintw(windowImprimeDados, j+3, xMax/(xMax - 5), "%s", mqtt_device[j - 4].room);
            }
        }

        for(int j=2;j<6;j++){
          
            mvwprintw(windowImprimeDados, j + 3, xMax/10+39, "%s = % d ", sensorsName[j], updateValues.sensors[j].state);
            if(updateValues.sensors[j].state){
                fprintf(fp,"nenhum, %s , %s", sensorsName[j], asctime(tm));
            }
        }

        if(screen_controler == 1 && cadastrar_dispositivo == 1)
        {
            mvwprintw(windowImprimeDados, 7 + 3, xMax/10+40, "Aperte ENTER para cadastrar quarto");
            //screen_controler = 0;
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

    wprintw(windowImprimeErros, "ERRO -> %s\n ",erro);
	box(windowImprimeErros, 0, 0);
	wrefresh(windowImprimeErros);

}



