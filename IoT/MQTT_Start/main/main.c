/**********************************************************************************
* MQTT START PRACTICA No. 1 JESUS JOSHUA MUÑOZ PACHECO
*
* ESTA PRACTICA ESTABLECE LOS FUNDAMENTOS DE CONEXION MEDIANTE EL PROTOCOLO
* MQTT HACIA UN BROKER PRE-ESTABLECIDO
*
* LA PRACTICA SE PROBÓ MEDIANTE EL BROKER MOSQUITTO, UN BROKER GRATUITO PERSONAL
* DE CODIGO ABIERTO QUE ES FACIL DE UTILIZAR E INSTALAR.
**********************************************************************************/


//CABEZERAS DEL PROGRAMA 

//DEPENDENCIAS DE ELEMENTOS BASICOS DEL ESP32
#include <stdio.h>
#include <stdint.h>
#include <stddef.h>
#include <string.h>
#include "esp_wifi.h"
#include "esp_system.h"
#include "nvs_flash.h"
#include "esp_event.h"
#include "esp_netif.h"
//#include "protocol_examples_common.h"

//DEPENDENCIAS DEL RTOS
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/semphr.h"
#include "freertos/queue.h"

//DEPENDENCIAS DE RED
#include "lwip/sockets.h"
#include "lwip/dns.h"
#include "lwip/netdb.h"

//DEPENDENCIAS DE PROTOCOLO MQTT
#include "esp_log.h"
#include "mqtt_client.h"

//DEFINICIONES DEL NOMBRE DEL PROTOCOLO Y ACCESO A LA RED WIFI
static const char *TAG = "MQTT_START";
#define  EXAMPLE_ESP_WIFI_SSID "MP Family"
#define  EXAMPLE_ESP_WIFI_PASS "14071999"


uint32_t MQTT_CONNEECTED = 0;

static void mqtt_app_start(void);

//MANIPULADOR DE EVENTOS DE CONEXION E INICIO DE LA APLICACION MQTT
static esp_err_t event_handler(void *ctx, system_event_t *event)
{
    switch (event->event_id)
    {
    case SYSTEM_EVENT_STA_START:
        esp_wifi_connect();
        ESP_LOGI(TAG, "Trying to connect with Wi-Fi\n");
        break;

    case SYSTEM_EVENT_STA_CONNECTED:
        ESP_LOGI(TAG, "Wi-Fi connected\n");
        break;

    case SYSTEM_EVENT_STA_GOT_IP:
        ESP_LOGI(TAG, "got ip: startibg MQTT Client\n");
        mqtt_app_start();
        break;

    case SYSTEM_EVENT_STA_DISCONNECTED:
        ESP_LOGI(TAG, "disconnected: Retrying Wi-Fi\n");
        esp_wifi_connect();
        break;

    default:
        break;
    }
    return ESP_OK;
}

//FUNCION DE INICIO, CONFIGURACION Y CONEXION DEL MODULO WIFI A LA RED PREVIAMENTE CONFIGURADA
void wifi_init(void)
{
    //HERRAMIENTAS DE CHEQUEO DE ERRORES PROPORCIONADAS POR ESPRESIFF
    ESP_ERROR_CHECK(esp_netif_init());

    ESP_ERROR_CHECK(esp_event_loop_create_default());
    esp_netif_create_default_wifi_sta();

    wifi_init_config_t cfg = WIFI_INIT_CONFIG_DEFAULT(); //CARGA CONFIGURACION POR DEFECTO DEL MODULO WIFI DE LA ESP32
    ESP_ERROR_CHECK(esp_wifi_init(&cfg));//INICIA EL MODULO CON LA CONFIGURACION ANTERIORMENTE CARGADA

    ESP_ERROR_CHECK(esp_event_loop_init(event_handler, NULL)); //MUESTRA EVENTOS DESDE LA CONFIGURACION HASTA LA CONECCION Y/O DESCONEXION
                                                               //DEL MODULO WIFI DE LA ESP32.

    //MANEJO DE DATOS DE RED MEDIANTE ESTRUCTURAS PARA LA CONFIGURACION DE RED WIFI
    wifi_config_t wifi_config = {
        .sta = {
            .ssid = EXAMPLE_ESP_WIFI_SSID,
            .password = EXAMPLE_ESP_WIFI_PASS,
	     .threshold.authmode = WIFI_AUTH_WPA2_PSK,
        },
    };
    ESP_ERROR_CHECK(esp_wifi_set_mode(WIFI_MODE_STA) );
    ESP_ERROR_CHECK(esp_wifi_set_config(WIFI_IF_STA, &wifi_config) );
    ESP_ERROR_CHECK(esp_wifi_start()); //EMPIEZA LA CONEXION 
}

//MANIPULADOR DE EVENTOS DE MQTT
static void mqtt_event_handler(void *handler_args, esp_event_base_t base, int32_t event_id, void *event_data)
{
    ESP_LOGD(TAG, "Event dispatched from event loop base=%s, event_id=%d", base, event_id);
    esp_mqtt_event_handle_t event = event_data;
    esp_mqtt_client_handle_t client = event->client;
    int msg_id;
    switch ((esp_mqtt_event_id_t)event_id)
    {
    case MQTT_EVENT_CONNECTED:
        ESP_LOGI(TAG, "MQTT_EVENT_CONNECTED");
        MQTT_CONNEECTED=1; //ESTA VARIABLE AFECTA EN SI HAY PUBLICACION O NO EN EL BROKER EN AL TAREA PRINCIPAL DEL RTOS
        
        msg_id = esp_mqtt_client_subscribe(client, "/topic/test1", 0);
        ESP_LOGI(TAG, "sent subscribe successful, msg_id=%d", msg_id);

        msg_id = esp_mqtt_client_subscribe(client, "/topic/test2", 1);
        ESP_LOGI(TAG, "sent subscribe successful, msg_id=%d", msg_id);
        break;
    case MQTT_EVENT_DISCONNECTED:
        ESP_LOGI(TAG, "MQTT_EVENT_DISCONNECTED");
        MQTT_CONNEECTED=0;//ESTA VARIABLE AFECTA EN SI HAY PUBLICACION O NO EN EL BROKER EN AL TAREA PRINCIPAL DEL RTOS
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
        break;
    case MQTT_EVENT_ERROR:
        ESP_LOGI(TAG, "MQTT_EVENT_ERROR");
        break;
    default:
        ESP_LOGI(TAG, "Other event id:%d", event->event_id);
        break;
    }
}

esp_mqtt_client_handle_t client = NULL;

//APLICACION MQTT, ESTA ES INICIADA UNA VEZ LA CONEXION WIFI ES EXITOSA, SE DEBE DE CONFIGURAR LA DIRECCION MQTT DEL BROKER
static void mqtt_app_start(void)
{
    ESP_LOGI(TAG, "STARTING MQTT");
    esp_mqtt_client_config_t mqttConfig = {
        .uri = "mqtt://192.168.1.4:1883"};
    
    client = esp_mqtt_client_init(&mqttConfig); //INICIA EL CLIENTE MQTT CON LA CONFIGURACION QUE SE LE ASIGNO
    esp_mqtt_client_register_event(client, ESP_EVENT_ANY_ID, mqtt_event_handler, client);
    esp_mqtt_client_start(client);
}
//TAREA PRINCIPAL DEL RTOS 
void Publisher_Task(void *params)
{
  while (true)
  {
    if(MQTT_CONNEECTED)
    {
        //PUBLICA EN EL BROKER EL TEXTO "HELLO WORLD" EN LA SIGUIENTE DIRECCION DEL BROKER
        esp_mqtt_client_publish(client, "/topic/test3", "Helllo World", 0, 0, 0);//LA DIRECCION Y MENSAJE PUEDEN SER CAMBIADOS.
        vTaskDelay(15000 / portTICK_PERIOD_MS);//RETRAZO DE TIEMPO ENTRE PUBLICACION
    }
  }
}

//EL PROGRAMA COMIENZA AQUI 

void app_main(void)
{
    //COMPRUEBA ERRORES SI LOS HAY AL MOMENTO DE ESCRIBIR EN MEMORIA
    esp_err_t ret = nvs_flash_init();
    if (ret == ESP_ERR_NVS_NO_FREE_PAGES || ret == ESP_ERR_NVS_NEW_VERSION_FOUND) {
      ESP_ERROR_CHECK(nvs_flash_erase());
      ret = nvs_flash_init();
    }
    ESP_ERROR_CHECK(ret);
    //INICIA EL WIFI Y ACCEDE A LA RED QUE SE DEFINIO ANTERIORMENTE
    wifi_init();
    //CREA UNA TAREA PARA EL RTOS DONDE SE ARA LA PUBLICACION AL BROKER
    xTaskCreate(Publisher_Task, "Publisher_Task", 1024 * 5, NULL, 5, NULL);
}