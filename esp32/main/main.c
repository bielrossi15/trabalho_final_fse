#include "main.h"

xSemaphoreHandle conexaoWifiSemaphore;
xSemaphoreHandle conexaoMQTTSemaphore;


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

void trataComunicacaoComServidor(void *params)
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

      

      mandaMensagem("temperatura",temperature);
      mandaMensagem("umidade", humidity);
      mandaMensagemEstado();
     
      vTaskDelay(10000 / portTICK_PERIOD_MS);
    }
  }
}

void app_main(void)
{
 
  // Inicializa o NVS
  esp_err_t ret = nvs_flash_init();
  DHT11_init(GPIO_NUM_4);
  initializaBotao();
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
  xTaskCreate(&trataInterrupcaoBotao, "Trata botão", 4096, NULL, 1, NULL);


}
