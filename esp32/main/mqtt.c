#include "mqtt.h"


#define TAG "MQTT"

extern xSemaphoreHandle conexaoMQTTSemaphore;
esp_mqtt_client_handle_t client;
char *macAddress;
char topicoComodo[100] = "fse2020/170013278/";
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
   

void conexaoEsp(){
    char topicoMqttConnected[100] = "fse2020/170013278/dispositivos/";
    getMacAddress();
    strcat(topicoMqttConnected, macAddress);
    mqtt_envia_mensagem(topicoMqttConnected, "Esp conectada");
    mqtt_inscricao(topicoMqttConnected);
    free(macAddress);
}

void pega_Comodo_MQTT_DATA(char buffer[]){
    
   
    printf("%s\n", buffer);
    cJSON *jsonComodo = cJSON_Parse(buffer);
    const cJSON *name = NULL;
    name = cJSON_GetObjectItemCaseSensitive(jsonComodo, "comodo");
    if (cJSON_IsString(name) && (name->valuestring != NULL))
    {
        //printf("Checking monitor \"%s\"\n", name->valuestring);
        strcpy(topicoComodo,"fse2020/170013278/");
        strcat(topicoComodo, name->valuestring);
        printf("%s\n", topicoComodo);
        mqtt_envia_mensagem(topicoComodo, "Mensagem do distribuido(esp)");
        cJSON_Delete(jsonComodo);
    }
    else
    {
        printf("não é string\n");
    }
    xSemaphoreGive(conexaoMQTTSemaphore);
}

static esp_err_t mqtt_event_handler_cb(esp_mqtt_event_handle_t event)
{
    client = event->client;
    int msg_id;
    
    switch (event->event_id) {
        case MQTT_EVENT_CONNECTED:
            ESP_LOGI(TAG, "MQTT_EVENT_CONNECTED");
           
            conexaoEsp();
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
            char buffer[100];
            memcpy(buffer, event->data, event->data_len);
            buffer[event->data_len] = '\0';
            pega_Comodo_MQTT_DATA(buffer);
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