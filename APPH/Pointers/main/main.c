#include <stdio.h>
// LIBRERIA FREERTOS PARA EL DELAY, NO ES NECESARIO HACER UNA TAREA DE RTOS PARA EL DELAY
#include <freertos/FreeRTOS.h>
#include <freertos/task.h>


//DECLARACION DE PUNTEROS A REGISTROS
/*||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||*/
uint32_t *gpio_out  = (uint32_t*)0x3FF44004;
uint32_t *gpio_enable = (uint32_t*)0x3FF44020;
uint32_t *gpio_out_wits = (uint32_t*)0x3FF44014;
/*\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

void app_main(void)
{

/*SE ENCIENDE EL LED CON LA MASCARA DE BITS*/
*gpio_enable |= 0x10;       //HABILITACION DE LA SALIDA
*gpio_out |= 0x10;          //PIN QUE QUEREMOS COMO SALIDA
*gpio_out_wits |= 0x10;     //SACAMOS UN 1 POR ESE PIN SELECCIONADO


vTaskDelay(1000/portTICK_PERIOD_MS);

/*SE APAGA EL LED CON LA MASCARA DE BITS*/
*gpio_out &= ~0x10;
*gpio_out_wits &= ~0x10;


}
