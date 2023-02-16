#include <stdio.h>
// FUNCION RTOS PARA RETRAZO, NO ES NECESARIO TENER UNA TAREA DE RTOS
#include <freertos/FreeRTOS.h>
#include <freertos/task.h>

#define HWREG32(x)        (*((volatile uint32_t *)(x)))
/*\\\\\\\\\\\\\\\\\MACROFUNCIONES PARA ACCESO A REGISTROS\\\\\\\\\\*/
#define GPIO_OUT  (HWREG32(0x3FF44004))
#define GPIO_ENABLE (HWREG32(0x3FF44020))
#define GPIO_OUT_WITS (HWREG32(0x3FF44014))
/*\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/
void app_main(void)
{


    GPIO_ENABLE |= 0x10;     //HABILITACION DEL PIN PARA SALIDA
    GPIO_OUT |= 0x10;       //SELECCION DEL PIN DE SALIDA
    GPIO_OUT_WITS |= 0x10;  //SACA UN 1 POR EL PIN SELECCIONADO

    //APAGADO DE LED 
    vTaskDelay(5000 / portTICK_PERIOD_MS);  //RETRAZO DE 5 SEGUNDOS
    GPIO_OUT &= ~0x10;
    GPIO_OUT_WITS &= ~0x10;

    //ENCENDIDO DE LED
    vTaskDelay(5000 / portTICK_PERIOD_MS); //RETRAZO DE 5 SEGUNDOS
    GPIO_OUT |= 0x10;
    GPIO_OUT_WITS |= 0x10;

}
