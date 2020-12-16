#include "main.h"

xSemaphoreHandle conexaoWifiSemaphore;
xSemaphoreHandle conexaoMQTTSemaphore;

extern char topicoComodo[100];
extern int estadoLed;
extern int clickBotao;
extern char *macAddress;

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

int criaJsonStr(cJSON *espInfo, cJSON *titulo)
{
  titulo = cJSON_CreateString(macAddress);
  if (titulo == NULL)
  {
    ESP_LOGE("JSON", "erro ao criar o json, tentando novamente em 3 segundos\n");
    vTaskDelay(3000 / portTICK_PERIOD_MS);
    return 1;
  }
  cJSON_AddItemToObject(espInfo, "id", titulo);
  return 0;
}

void mandaMensagem(char *topico, int info)
{
  cJSON *json = cJSON_CreateObject();
  while (json == NULL)
  {
    cJSON_Delete(json);
    ESP_LOGE("JSON", "erro ao criar o objeto json, tentando novamente em 3 segundos\n");
    vTaskDelay(3000 / portTICK_PERIOD_MS);
    json = cJSON_CreateObject();
  }

  cJSON *mensagem = NULL;
  while (criaJsonStr(json, mensagem));
  while (criaJson(json, mensagem, topico, info));
  
  char *info2 = cJSON_Print(json);
  char enviaEstado[200];
  sprintf(enviaEstado, "%s/%s", topicoComodo,topico);
  mqtt_envia_mensagem(enviaEstado, info2);
  cJSON_Delete(json);
}

void mandaMensagemEstado(){
  cJSON *json = cJSON_CreateObject();
  while (json == NULL)
  {
    cJSON_Delete(json);
    ESP_LOGE("JSON", "erro ao criar o objeto json, tentando novamente em 3 segundos\n");
    vTaskDelay(3000 / portTICK_PERIOD_MS);
    json = cJSON_CreateObject();
  }

  cJSON *id = NULL;
  while (criaJsonStr(json, id));
  
  cJSON *saida = NULL;
  while (criaJson(json, saida, "saida", estadoLed));

  cJSON *entrada = NULL;
  while (criaJson(json, entrada, "entrada", clickBotao));
  
  char *info = cJSON_Print(json);
  char enviaEstado[200];
  sprintf(enviaEstado,"%s/estado",topicoComodo);
  mqtt_envia_mensagem(enviaEstado, info);
  cJSON_Delete(json);
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
