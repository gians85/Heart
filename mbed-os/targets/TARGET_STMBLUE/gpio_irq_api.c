#include "gpio_irq_api.h"
#include "cmsis.h"
#include "core_cm0.h"
#include "pin_device.h"

typedef enum {
    IRQ_ON_FALLING_EDGE = 0,
    IRQ_ON_RISING_EDGE,
    IRQ_ON_BOTH_EDGE
} ButtonIrqType;


int gpio_irq_init(gpio_irq_t *obj, PinName pin, gpio_irq_handler handler, uint32_t id){
    
    if (pin == NC) return -1;
    
    obj->pin = pin;
    obj->irq_id = id;
    obj->irq_type_n = GPIO_IRQn ;
    
    /* Enable the GPIO Clock */
    SysCtrl_PeripheralClockCmd(CLOCK_PERIPH_GPIO, ENABLE);
    
    
    /* Configure the push buttons */
    obj->GPIO_InitStructure.GPIO_Pin = getGpioPin(pin); //Get_ButtonGpioPin(BUTTON_1);
    obj->GPIO_InitStructure.GPIO_Mode = GPIO_Input;
    obj->GPIO_InitStructure.GPIO_Pull = DISABLE;
    obj->GPIO_InitStructure.GPIO_HighPwr = DISABLE;
    GPIO_Init(&obj->GPIO_InitStructure);

    
    /* Set the GPIO interrupt priority and enable it */
    obj->NVIC_InitStructure.NVIC_IRQChannel = GPIO_IRQn;
    obj->NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = LOW_PRIORITY;
    obj->NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
    NVIC_Init(&obj->NVIC_InitStructure);

    
    /* Configures EXTI line for BUTTON_1 */
    obj->GPIO_EXTIStructure.GPIO_Pin = getGpioPin(pin); //Get_ButtonGpioPin(BUTTON_1);
    obj->GPIO_EXTIStructure.GPIO_IrqSense = GPIO_IrqSense_Edge;
    obj->GPIO_EXTIStructure.GPIO_Event = IRQ_ON_BOTH_EDGE;
    GPIO_EXTIConfig(&obj->GPIO_EXTIStructure);


    /* Clear pending interrupt */
    GPIO_ClearITPendingBit( getGpioPin(pin));//Get_ButtonGpioPin(BUTTON_1) | Get_ButtonGpioPin(BUTTON_2));
    
    /* Enable the interrupt */
    GPIO_EXTICmd(getGpioPin(pin), ENABLE); //Get_ButtonGpioPin(BUTTON_1) | Get_ButtonGpioPin(BUTTON_2), ENABLE);
    
    return 0;
}




void gpio_irq_free(gpio_irq_t *obj){
}


void gpio_irq_set(gpio_irq_t *obj, gpio_irq_event event, uint32_t enable)
{
    /*  Enable / Disable Edge triggered interrupt and store event */
    if (event == IRQ_RISE) {
        if (enable) {
            obj->GPIO_EXTIStructure.GPIO_IrqSense = GPIO_IrqSense_Edge;
            obj->GPIO_EXTIStructure.GPIO_Event = IRQ_ON_RISING_EDGE;
            GPIO_EXTIConfig(&obj->GPIO_EXTIStructure);
            obj->event |= IRQ_RISE;
        } else {
            obj->NVIC_InitStructure.NVIC_IRQChannelCmd = DISABLE;
            GPIO_EXTIConfig(&obj->GPIO_EXTIStructure);
            obj->event &= ~IRQ_RISE;
        }
    }
    if (event == IRQ_FALL) {
        if (enable) {
            obj->GPIO_EXTIStructure.GPIO_IrqSense = GPIO_IrqSense_Edge;
            obj->GPIO_EXTIStructure.GPIO_Event = IRQ_ON_FALLING_EDGE;
            GPIO_EXTIConfig(&obj->GPIO_EXTIStructure);
            obj->event |= IRQ_RISE;
        } else {
            obj->NVIC_InitStructure.NVIC_IRQChannelCmd = DISABLE;
            GPIO_EXTIConfig(&obj->GPIO_EXTIStructure);
            obj->event &= ~IRQ_RISE;
        }
    }
}

void GPIO_Handler(void){
}

