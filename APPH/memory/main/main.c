#include <stdio.h>
// LIBRERIA FREERTOS PARA EL DELAY, NO ES NECESARIO HACER UNA TAREA DE RTOS PARA EL DELAY
#include <freertos/FreeRTOS.h>
#include <freertos/task.h>

void app_main(void)
{

/*SE ENCIENDE EL LED CON LA MASCARA DE BITS Y DEFERENCIA DIRECTA A MEMORIA*/
(*((volatile uint32_t *)(0x3FF44020))) |= 0x10;       //HABILITA LA SALIDA DEL PIN                       
(*((volatile uint32_t *)(0x3FF44004))) |= 0x10;       //SELECCIONA EL PIN DE SALIDA                         
(*((volatile uint32_t *)(0x3FF44014))) |= 0x10;       //SACA UN 1 POR EL PIN DE SALIDA                      

vTaskDelay(1000/portTICK_PERIOD_MS);

/*SE APAGA EL LED CON LA MASCARA DE BITS Y DEFERENCIA DIRECTA A MEMORIA*/
(*((volatile uint32_t *)(0x3FF44004))) &= ~0x10;                                
(*((volatile uint32_t *)(0x3FF44014))) &= ~0x10;


}
