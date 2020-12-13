#include "main.h"

xSemaphoreHandle conexaoWifiSemaphore;
xSemaphoreHandle conexaoMQTTSemaphore;

extern char topicoComodo[100];
extern int estadoLed;

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

int criaJson (cJSON *espInfo,cJSON *titulo,char nome[],int info){
  
  titulo = cJSON_CreateNumber(info);
  if (titulo == NULL)
  {
    ESP_LOGE("JSON", "erro ao criar o json, tentando novamente em 3 segundos\n");
    vTaskDelay(3000 / portTICK_PERIOD_MS);
    return 1;
  }

  cJSON_AddItemToObject(espInfo, nome, titulo);
  return 0;
}



void trataComunicacaoComServidor(void * params)
{
  char mensagem[50];
  if(xSemaphoreTake(conexaoMQTTSemaphore, portMAX_DELAY))
  {
    while(true)
    {
      int temperature,humidity;
      while(temperature = DHT11_read().temperature,temperature<0){
        ESP_LOGE("TEMPERATURA","erro ao ler a temperatura, tentando novamente em 3 segundos\n");
        vTaskDelay(3000 / portTICK_PERIOD_MS);
      };
      while (humidity = DHT11_read().humidity, humidity < 0){
        ESP_LOGE("UMIDADE", "erro ao ler a umidade, tentando novamente em 3 segundos\n");
        vTaskDelay(3000 / portTICK_PERIOD_MS);
      }

      

      cJSON *espInfo = cJSON_CreateObject();
      while(espInfo == NULL)
      {
        cJSON_Delete(espInfo);
        ESP_LOGE("JSON", "erro ao criar o objeto json, tentando novamente em 3 segundos\n");
        vTaskDelay(3000 / portTICK_PERIOD_MS);
        espInfo = cJSON_CreateObject();
      }

      cJSON *temperatura = NULL;
      while(criaJson(espInfo, temperatura, "temperatura", temperature));

      cJSON *umidade = NULL;
      while (criaJson(espInfo, umidade, "umidade", humidity));

     
      cJSON *saida = NULL;
      while (criaJson(espInfo, saida, "saida", estadoLed));

      char *info = cJSON_Print(espInfo);
    
      printf("String de info = %s\n",info);
      mqtt_envia_mensagem(topicoComodo, info);
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
  inicializaLed();
  
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
