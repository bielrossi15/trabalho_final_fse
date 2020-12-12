#include "main.h"

xSemaphoreHandle conexaoWifiSemaphore;
xSemaphoreHandle conexaoMQTTSemaphore;

extern char topicoComodo[100];

void conectadoWifi(void *params)
{
  while(true)
  {
    if(xSemaphoreTake(conexaoWifiSemaphore, portMAX_DELAY))
    {
      // Processamento Internet
      mqtt_start();
    }
  }
}

void trataComunicacaoComServidor(void * params)
{
  char mensagem[50];
  if(xSemaphoreTake(conexaoMQTTSemaphore, portMAX_DELAY))
  {
    while(true)
    {
      //printf("Temperature is %d \n", DHT11_read().temperature);
      //printf("Humidity is %d\n", DHT11_read().humidity);
      //printf("Status code is %d\n", DHT11_read().status);

      cJSON *espInfo = cJSON_CreateObject();
      if (espInfo == NULL)
      {
        printf("erro1\n");
        return;
      }

      cJSON *temperatura = NULL;
      temperatura = cJSON_CreateNumber(DHT11_read().temperature);
      if (temperatura == NULL)
      {
        printf("erro2\n");
        return;
      }

      cJSON_AddItemToObject(espInfo, "temperatura", temperatura);

      cJSON *umidade = NULL;
      umidade = cJSON_CreateNumber(DHT11_read().humidity);
      if (umidade == NULL)
      {
        printf("erro3\n");
        return;
      }

      cJSON_AddItemToObject(espInfo, "umidade", umidade);

      char *teste = cJSON_Print(espInfo);
      printf("String de teste = %s\n",teste);
      mqtt_envia_mensagem(topicoComodo, teste);
      vTaskDelay(30000 / portTICK_PERIOD_MS);
      cJSON_Delete(espInfo);
    }
  }
}

void app_main(void)
{
 
  // Inicializa o NVS
  esp_err_t ret = nvs_flash_init();
  DHT11_init(GPIO_NUM_4);
  if (ret == ESP_ERR_NVS_NO_FREE_PAGES || ret == ESP_ERR_NVS_NEW_VERSION_FOUND)
  {
    ESP_ERROR_CHECK(nvs_flash_erase());
    ret = nvs_flash_init();
    }
    ESP_ERROR_CHECK(ret);
    
    conexaoWifiSemaphore = xSemaphoreCreateBinary();
    conexaoMQTTSemaphore = xSemaphoreCreateBinary();
    wifi_start();

    xTaskCreate(&conectadoWifi,  "Conexão ao MQTT", 4096, NULL, 1, NULL);
    xTaskCreate(&trataComunicacaoComServidor, "Comunicação com Broker", 4096, NULL, 1, NULL);
}
