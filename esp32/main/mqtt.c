#include <stdio.h>
#include <stdint.h>
#include <stddef.h>
#include <string.h>
#include "esp_system.h"
#include "esp_event.h"
#include "esp_netif.h"

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/semphr.h"
#include "freertos/queue.h"

#include "lwip/sockets.h"
#include "lwip/dns.h"
#include "lwip/netdb.h"

#include "esp_log.h"
#include "mqtt_client.h"

#include "mqtt.h"
#include "cJSON.h"

#define TAG "MQTT"

extern xSemaphoreHandle conexaoMQTTSemaphore;
esp_mqtt_client_handle_t client;
char *macAddress;
void  getMacAddress(){

    uint8_t *mac = (uint8_t *)malloc(sizeof(uint8_t) * 15);
    esp_efuse_mac_get_default(mac);
    int interm[20];
    macAddress = (char *) malloc(sizeof(char *)*100);
    for (int i = 0; i < 6; i++)
    {
        interm[i]=mac[i];
        sprintf(macAddress,"%X",interm[i]);
        macAddress+=2;
    }
    macAddress-=12;
    //free(mac);

}
   

static esp_err_t mqtt_event_handler_cb(esp_mqtt_event_handle_t event)
{
    client = event->client;
    int msg_id;
    
    switch (event->event_id) {
        case MQTT_EVENT_CONNECTED:
            ESP_LOGI(TAG, "MQTT_EVENT_CONNECTED");
            xSemaphoreGive(conexaoMQTTSemaphore);

            char topicoMqttConnected[100] = "fse2020/170013278/dispositivos/";
            getMacAddress();
            strcat(topicoMqttConnected,macAddress);
            mqtt_envia_mensagem(topicoMqttConnected, "Esp conectada");
            mqtt_inscricao(topicoMqttConnected);
            //free(macAddress);
            
            break;
        case MQTT_EVENT_DISCONNECTED:
            ESP_LOGI(TAG, "MQTT_EVENT_DISCONNECTED");
            break;

        case MQTT_EVENT_SUBSCRIBED:
            ESP_LOGI(TAG, "MQTT_EVENT_SUBSCRIBED, msg_id=%d", event->msg_id);
            break;
        case MQTT_EVENT_UNSUBSCRIBED:
            ESP_LOGI(TAG, "MQTT_EVENT_UNSUBSCRIBED, msg_id=%d", event->msg_id);
            break;
        case MQTT_EVENT_PUBLISHED:
            ESP_LOGI(TAG, "MQTT_EVENT_PUBLISHED, msg_id=%d", event->msg_id);
            break;
        case MQTT_EVENT_DATA:
            ESP_LOGI(TAG, "MQTT_EVENT_DATA");
            printf("TOPIC=%.*s\r\n", event->topic_len, event->topic);
            printf("DATA=%.*s\r\n", event->data_len, event->data);
            char topicoData[100] = "fse2020/170013278/";

            //const cJSON *name = NULL;
            char buffer[100];
         
            memcpy(buffer, event->data, event->data_len);

            buffer[event->data_len] = '\0';
            printf("%s\n", buffer);
            cJSON *jsonComodo = cJSON_Parse(buffer);
            const cJSON *name = NULL;
            //char *comodo = (char *)malloc(sizeof(char *)*20);
            char *comodo = (char *) malloc(sizeof(char *)*30);
            name = cJSON_GetObjectItemCaseSensitive(jsonComodo, "comodo");
            if (cJSON_IsString(name) && (name->valuestring != NULL))
            {
                comodo = name->valuestring;
                printf("Checking monitor \"%s\"\n", name->valuestring);
            }else{
                printf("não é string\n");
            }

            printf("%s\n",comodo);
            strcat(topicoData,comodo);
            //for(int i=0,j=0;j<strlen(comodo);j++,i++){
              //  printf("%c %d\n",comodo[j],j);
            //}
            printf("%s\n",topicoData);
            //free(comodo);
            mqtt_envia_mensagem(topicoData, "Mensagem do distribuido(esp)");
            cJSON_Delete(jsonComodo);
            break;
        case MQTT_EVENT_ERROR:
            ESP_LOGI(TAG, "MQTT_EVENT_ERROR");
            break;
        default:
            ESP_LOGI(TAG, "Other event id:%d", event->event_id);
            break;
    }
    return ESP_OK;
}

static void mqtt_event_handler(void *handler_args, esp_event_base_t base, int32_t event_id, void *event_data) {
    ESP_LOGD(TAG, "Event dispatched from event loop base=%s, event_id=%d", base, event_id);
    mqtt_event_handler_cb(event_data);
}

void mqtt_start()
{
    esp_mqtt_client_config_t mqtt_config = {
        .uri = "mqtt://mqtt.eclipseprojects.io",
    };
    client = esp_mqtt_client_init(&mqtt_config);
    esp_mqtt_client_register_event(client, ESP_EVENT_ANY_ID, mqtt_event_handler, client);
    esp_mqtt_client_start(client);
}

void mqtt_envia_mensagem(char * topico, char * mensagem)
{
    int message_id = esp_mqtt_client_publish(client, topico, mensagem, 0, 1, 0);
    ESP_LOGI(TAG, "Mesnagem enviada, ID: %d", message_id);
}

void mqtt_inscricao(char *topico)
{
    int message_id = esp_mqtt_client_subscribe(client, topico, 0);
    ESP_LOGI(TAG, "Inscrição enviada, ID: %d", message_id);
}