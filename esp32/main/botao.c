#include"botao.h"

xQueueHandle filaDeInterrupcao;

int clickBotao=0;
extern char topicoComodo[100];
static void IRAM_ATTR gpio_isr_handler(void *args)
{
    int pino = (int)args;
    xQueueSendFromISR(filaDeInterrupcao, &pino, NULL);
}

void initializaBotao()
{
    filaDeInterrupcao = xQueueCreate(10, sizeof(int));
    // Configuração do pino do Botão
    gpio_pad_select_gpio(BOTAO);
    // Configura o pino do Botão como Entrada
    gpio_set_direction(BOTAO, GPIO_MODE_INPUT);
    // Configura o resistor de Pulldown para o botão (por padrão a entrada estará em Zero)
    gpio_pulldown_en(BOTAO);
    // Desabilita o resistor de Pull-up por segurança.
    gpio_pullup_dis(BOTAO);
    gpio_set_intr_type(BOTAO, GPIO_INTR_POSEDGE);

    gpio_install_isr_service(0);
    gpio_isr_handler_add(BOTAO, gpio_isr_handler, (void *)BOTAO);
   
}

void mandaMensagemPush(){
    cJSON *botaoPush = cJSON_CreateObject();
    while (botaoPush == NULL)
    {
        cJSON_Delete(botaoPush);
        ESP_LOGE("JSON", "erro ao criar o objeto json, tentando novamente em 3 segundos\n");
        vTaskDelay(3000 / portTICK_PERIOD_MS);
        botaoPush = cJSON_CreateObject();
    }
    cJSON *entrada = NULL;
    while (criaJson(botaoPush, entrada, "entrada", clickBotao));
    char *info = cJSON_Print(botaoPush);
    printf("String de info = %s\n", info);
    mqtt_envia_mensagem(topicoComodo, info);
    cJSON_Delete(botaoPush);
}

void trataInterrupcaoBotao(void *params)
{
    int pino;
    int contador = 0;

    while (true)
    {
        if (xQueueReceive(filaDeInterrupcao, &pino, portMAX_DELAY))
        {
            // De-bouncing
            int estado = gpio_get_level(pino);
            if (estado == 1)
            {
                gpio_isr_handler_remove(pino);
                while (gpio_get_level(pino) == estado)
                {
                    vTaskDelay(50 / portTICK_PERIOD_MS);
                }

                clickBotao=!clickBotao;
                printf("%d\n",clickBotao);
                mandaMensagemPush();
                // Habilitar novamente a interrupção
                vTaskDelay(50 / portTICK_PERIOD_MS);
                gpio_isr_handler_add(pino, gpio_isr_handler, (void *)pino);
            }
        }
    }
}

