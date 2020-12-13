#include <linux/i2c-dev.h>
#include <sys/ioctl.h>

/******************************************************************************/
/*!                         System header files                               */
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <fcntl.h>
#include <time.h>

/******************************************************************************/
/*!                         Own header files                                  */
#include "bme280.h"

/******************************************************************************/
/*!                               Structures                                  */

/* Structure that contains identifier details used in example */
struct identifier
{
    /* Variable to hold device address */
    uint8_t dev_addr;

    /* Variable that contains file descriptor */
    int8_t fd;
};



void user_delay_us(uint32_t period, void *intf_ptr);

void print_sensor_data(struct bme280_data *comp_data);

int8_t user_i2c_read(uint8_t reg_addr, uint8_t *data, uint32_t len, void *intf_ptr);

int8_t user_i2c_write(uint8_t reg_addr, const uint8_t *data, uint32_t len, void *intf_ptr);

void stream_sensor_data_forced_mode(struct bme280_dev *dev);


struct bme280_dev dev;
struct identifier id;
char * i2cDisp;
float *resultado;

int initI2C(){
    
    i2cDisp = "/dev/i2c-1"; 
    if ((id.fd = open(i2cDisp, O_RDWR)) < 0)
    {
        //fprintf(stderr, "Failed to open the i2c bus %s\n", i2cDisp);
        return -1;
    }

     /* Make sure to select BME280_I2C_ADDR_PRIM or BME280_I2C_ADDR_SEC as needed */
    id.dev_addr = BME280_I2C_ADDR_PRIM;

    dev.intf = BME280_I2C_INTF;
    dev.read = user_i2c_read;
    dev.write = user_i2c_write;
    dev.delay_us = user_delay_us;

    /* Update interface pointer with the structure that contains both device address and file descriptor */
    dev.intf_ptr = &id;

    if (ioctl(id.fd, I2C_SLAVE, id.dev_addr) < 0)
    {
        //fprintf(stderr, "Failed to acquire bus access and/or talk to slave.\n");
        return -1;
    }

     /* Variable to define the result */
    int8_t rslt = BME280_OK;
   
    /* Initialize the bme280 */
    rslt = bme280_init(&dev);
    if (rslt != BME280_OK)
    {
        //fprintf(stderr, "Failed to initialize the device (code %+d).\n", rslt);
        return -1;
    }
    return 0;
}

float * temperature_humidity()
{
    resultado = malloc(sizeof(float)*5);
    resultado[0]=-1;
    resultado[1]=-1;
    stream_sensor_data_forced_mode(&dev);
    return resultado;
}

/*!
 * @brief This function reading the sensor's registers through I2C bus.
 */
int8_t user_i2c_read(uint8_t reg_addr, uint8_t *data, uint32_t len, void *intf_ptr)
{
    struct identifier id;

    id = *((struct identifier *)intf_ptr);

    write(id.fd, &reg_addr, 1);
    read(id.fd, data, len);

    return 0;
}

/*!
 * @brief This function provides the delay for required time (Microseconds) as per the input provided in some of the
 * APIs
 */
void user_delay_us(uint32_t period, void *intf_ptr)
{
    usleep(period);
}

/*!
 * @brief This function for writing the sensor's registers through I2C bus.
 */
int8_t user_i2c_write(uint8_t reg_addr, const uint8_t *data, uint32_t len, void *intf_ptr)
{
    uint8_t *buf;
    struct identifier id;

    id = *((struct identifier *)intf_ptr);

    buf = malloc(len + 1);
    buf[0] = reg_addr;
    memcpy(buf + 1, data, len);
    if (write(id.fd, buf, len + 1) < (uint16_t)len)
    {
        return BME280_E_COMM_FAIL;
    }

    free(buf);

    return BME280_OK;
}

/*!
 * @brief This API used to print the sensor temperature, pressure and humidity data.
 */
void print_sensor_data(struct bme280_data *comp_data)
{
    float temp, press, hum;

#ifdef BME280_FLOAT_ENABLE
    temp = comp_data->temperature;
    press = 0.01 * comp_data->pressure;
    hum = comp_data->humidity;
#else
#ifdef BME280_64BIT_ENABLE
    temp = 0.01f * comp_data->temperature;
    press = 0.0001f * comp_data->pressure;
    hum = 1.0f / 1024.0f * comp_data->humidity;
#else
    temp = 0.01f * comp_data->temperature;
    press = 0.01f * comp_data->pressure;
    hum = 1.0f / 1024.0f * comp_data->humidity;
#endif
#endif
    printf("%0.2lf deg C, %0.2lf hPa, %0.2lf%%\n", temp, press, hum);
}

/*!
 * @brief This API reads the sensor temperature, pressure and humidity data in forced mode.
 */
void stream_sensor_data_forced_mode(struct bme280_dev *dev)
{
    /* Variable to define the result */
    int8_t rslt = BME280_OK;

    /* Variable to define the selecting sensors */
    uint8_t settings_sel = 0;

    /* Variable to store minimum wait time between consecutive measurement in force mode */

    /* Structure to get the pressure, temperature and humidity values */

    /* Recommended mode of operation: Indoor navigation */
    dev->settings.osr_h = BME280_OVERSAMPLING_1X;
    dev->settings.osr_p = BME280_OVERSAMPLING_16X;
    dev->settings.osr_t = BME280_OVERSAMPLING_2X;
    dev->settings.filter = BME280_FILTER_COEFF_16;

    settings_sel = BME280_OSR_PRESS_SEL | BME280_OSR_TEMP_SEL | BME280_OSR_HUM_SEL | BME280_FILTER_SEL;

    /* Set the sensor settings */
    rslt = bme280_set_sensor_settings(settings_sel, dev);
    if (rslt != BME280_OK)
    {
        //fprintf(stderr, "Failed to set sensor settings (code %+d).", rslt);

        return;
    }

  
    struct bme280_data save_Data;

    /* Set the sensor to forced mode */
    rslt = bme280_set_sensor_mode(BME280_FORCED_MODE, dev);
    if (rslt != BME280_OK)
    {
        //fprintf(stderr, "Failed to set sensor mode (code %+d).", rslt);
        return ;
    }

    /* Wait for the measurement to complete and print data */
    usleep(100000);
    rslt = bme280_get_sensor_data(BME280_ALL, &save_Data, dev);
    if (rslt != BME280_OK)
    {
        //fprintf(stderr, "Failed to get sensor data (code %+d).", rslt);
        return ;
    }
    
    resultado[0] = save_Data.temperature;
    resultado[1] = save_Data.humidity;
   
}

void trata_interrupcao_I2C(){
    if(close(id.fd)==0){
        fprintf(stderr,"fechou o arquivo I2C\n");
    }
    else{
        fprintf(stderr,"arquivo i2c já estava fechado\n");
    }
}