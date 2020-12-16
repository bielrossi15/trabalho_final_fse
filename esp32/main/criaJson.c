#include "criaJson.h"

extern char topicoComodo[100];
extern int estadoLed;
extern int clickBotao;
extern char *macAddress;


int criaJson(cJSON *espInfo, cJSON *titulo, char nome[], int info)
{

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
    while (criaJsonStr(json, mensagem))
        ;
    while (criaJson(json, mensagem, topico, info))
        ;

    char *info2 = cJSON_Print(json);
    char enviaEstado[200];
    sprintf(enviaEstado, "%s/%s", topicoComodo, topico);
    mqtt_envia_mensagem(enviaEstado, info2);
    cJSON_Delete(json);
}

void mandaMensagemEstado()
{
    cJSON *json = cJSON_CreateObject();
    while (json == NULL)
    {
        cJSON_Delete(json);
        ESP_LOGE("JSON", "erro ao criar o objeto json, tentando novamente em 3 segundos\n");
        vTaskDelay(3000 / portTICK_PERIOD_MS);
        json = cJSON_CreateObject();
    }

    cJSON *id = NULL;
    while (criaJsonStr(json, id))
        ;

    cJSON *saida = NULL;
    while (criaJson(json, saida, "saida", estadoLed))
        ;

    cJSON *entrada = NULL;
    while (criaJson(json, entrada, "entrada", clickBotao))
        ;

    char *info = cJSON_Print(json);
    char enviaEstado[200];
    sprintf(enviaEstado, "%s/estado", topicoComodo);
    mqtt_envia_mensagem(enviaEstado, info);
    cJSON_Delete(json);
}