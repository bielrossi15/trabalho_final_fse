#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#include "helper.h"

FILE *file;

void file_write(int type, int number, int on_off)
{
    char date[DATE_SIZE];
    char hour[HOUR_SIZE];

    
    format_time(date, hour);

    file = fopen("./data.csv", "a+");

    if(type == 0)
    {
        if(on_off == 1)
            fprintf(file, "%s %s -> turned on lamp %d\n", date, hour, number);
        else
            fprintf(file, "%s %s -> turned off lamp %d\n", date, hour, number);
    }

    else
    {
        fprintf(file, "%s %s -> alarm triggered\n", date, hour);
    }
    
    fclose(file);
}

void format_time(char *date_string, char *hour_string) 
{
    time_t rawtime;
    struct tm * tm_data;

    time(&rawtime);
    tm_data = localtime(&rawtime);

    sprintf(hour_string, "%02d:%02d:%02d", tm_data->tm_hour, tm_data->tm_min, tm_data->tm_sec);

    sprintf(date_string, "%02d-%02d-%04d", tm_data->tm_mday, tm_data->tm_mon+1, 1900+tm_data->tm_year);
}