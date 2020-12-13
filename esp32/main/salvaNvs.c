#include "salvaNvs.h"

extern char topicoComodo[100];

int le_valor_nvs()
{
    // Inicia o acesso à partição padrão nvs
    //ESP_ERROR_CHECK(nvs_flash_init());

    // Inicia o acesso à partição personalizada
    ESP_ERROR_CHECK(nvs_flash_init_partition("DadosNVS"));

    nvs_handle particao_padrao_handle;

    // Abre o acesso à partição nvs
    //esp_err_t res_nvs = nvs_open("armazenamento", NVS_READONLY, &particao_padrao_handle);

    // Abre o acesso à partição DadosNVS
    esp_err_t res_nvs = nvs_open_from_partition("DadosNVS", "armazenamento", NVS_READONLY, &particao_padrao_handle);

    if (res_nvs == ESP_ERR_NVS_NOT_FOUND)
    {
        ESP_LOGE("NVS", "Namespace: armazenamentos, não encontrado");
        return -1;
    }
    else
    {
        esp_err_t res;
       
        size_t required_size;
        res = nvs_get_str(particao_padrao_handle, "topicoComodo", NULL, &required_size);
        res = nvs_get_str(particao_padrao_handle, "topicoComodo", topicoComodo, &required_size);
       

        switch (res)
        {
        case ESP_OK:
            printf("TopicoComodo armazenado: %s\n", topicoComodo);
            break;

        case ESP_ERR_NOT_FOUND:
            ESP_LOGE("NVS", "isConnected não encontrado");
            return -1;
        default:
            ESP_LOGE("NVS", "Erro ao acessar o NVS (%s)", esp_err_to_name(res));
            return -1;
            break;
        }

        nvs_close(particao_padrao_handle);
    }
    return 0;
}

void grava_valor_nvs()
{
    //ESP_ERROR_CHECK(nvs_flash_init());
    ESP_ERROR_CHECK(nvs_flash_init_partition("DadosNVS"));

    nvs_handle particao_padrao_handle;

    //esp_err_t res_nvs = nvs_open("armazenamento", NVS_READWRITE, &particao_padrao_handle);
    esp_err_t res_nvs = nvs_open_from_partition("DadosNVS", "armazenamento", NVS_READWRITE, &particao_padrao_handle);

    if (res_nvs == ESP_ERR_NVS_NOT_FOUND)
    {
        ESP_LOGE("NVS", "Namespace: armazenamento, não encontrado");
    }
    esp_err_t res;
    
   
    res = nvs_set_str(particao_padrao_handle, "topicoComodo", topicoComodo);
    
    if (res != ESP_OK)
    {
        ESP_LOGE("NVS", "Não foi possível escrever no NVS (%s)", esp_err_to_name(res));
    }
    nvs_commit(particao_padrao_handle);
    nvs_close(particao_padrao_handle);
}
