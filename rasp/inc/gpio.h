#ifndef GPIO_H
#define GPIO_H

void get_state(int lamp[], int sp[], int so[]);
void get_sensor_state(int sp[], int so[]);
void set_lamp_state(int value, int opt);
void configure_pins();

#endif