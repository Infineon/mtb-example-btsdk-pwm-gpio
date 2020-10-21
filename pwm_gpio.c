/******************************************************************************
* File Name:   pwm_gpio.c
*
* Version:     2.0.0
*
* Description: This is the source code for the pwm_gpio Example
*              for ModusToolbox.
*
* Related Document: See README.md
*
*
*******************************************************************************
* (c) 2020, Cypress Semiconductor Corporation. All rights reserved.
*******************************************************************************
* This software, including source code, documentation and related materials
* ("Software"), is owned by Cypress Semiconductor Corporation or one of its
* subsidiaries ("Cypress") and is protected by and subject to worldwide patent
* protection (United States and foreign), United States copyright laws and
* international treaty provisions. Therefore, you may use this Software only
* as provided in the license agreement accompanying the software package from
* which you obtained this Software ("EULA").
*
* If no EULA applies, Cypress hereby grants you a personal, non-exclusive,
* non-transferable license to copy, modify, and compile the Software source
* code solely for use in connection with Cypress's integrated circuit products.
* Any reproduction, modification, translation, compilation, or representation
* of this Software except as specified above is prohibited without the express
* written permission of Cypress.
*
* Disclaimer: THIS SOFTWARE IS PROVIDED AS-IS, WITH NO WARRANTY OF ANY KIND,
* EXPRESS OR IMPLIED, INCLUDING, BUT NOT LIMITED TO, NONINFRINGEMENT, IMPLIED
* WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE. Cypress
* reserves the right to make changes to the Software without notice. Cypress
* does not assume any liability arising out of the application or use of the
* Software or any product or circuit described in the Software. Cypress does
* not authorize its products for use in any products where a malfunction or
* failure of the Cypress product may reasonably be expected to result in
* significant property damage, injury or death ("High Risk Product"). By
* including Cypress's product in a High Risk Product, the manufacturer of such
* system or application assumes all risk of such use and in doing so agrees to
* indemnify Cypress against all liability.
*******************************************************************************/

#include "wiced.h"
#include "wiced_platform.h"
#include "sparcommon.h"
#include "wiced_bt_stack.h"
#include "wiced_rtos.h"
#include "wiced_hal_puart.h"
#include "wiced_bt_trace.h"
#include "wiced_hal_pwm.h"
#include "wiced_hal_aclk.h"
#include "GeneratedSource/cycfg_pins.h"

/*****************************    Constants   *****************************/
/* Thread will delay for 5ms */
#define THREAD_DELAY_IN_MS          (5)

/* Useful macros for thread priorities */
#define PRIORITY_HIGH               (3)
#define PRIORITY_MEDIUM             (5)
#define PRIORITY_LOW                (7)

/* Sensible stack size for most threads */
#define THREAD_STACK_MIN_SIZE       (500)
#define INVERT_LED                  (1)

/*Macros for clocks*/
#define PWM_INP_CLK_IN_HZ           (32*1000)
#define PWM_FREQ_IN_HZ              (5000)
#define DUTY_STEP_SIZE              (1)

/*Enable/Disable PMU Clock. 1-Enable, 0-Disable*/
#define ENABLE_PMU_CLK              (0)

/*****************************    Variables   *****************************/
wiced_thread_t     *led_thread;
uint8_t             pwm_flag = 0;

/*****************************    Function Prototypes   *******************/
static wiced_result_t bt_cback(wiced_bt_management_evt_t event, 
                               wiced_bt_management_evt_data_t *p_event_data );
static void PWM_control(uint32_t arg);
static void button_cb(void * data , uint8_t port_pin);

/*****************************    Functions   *****************************/
/*
 Function name:
 application_start

 Function Description:
 @brief    Starting point of your application. Entry point to the application.
           Set device configuration and start BT stack initialization.
           The actual application initialization will happen when stack reports
           that BT device is ready.

 @param void

 @return void
 */
void application_start(void)
{
    wiced_result_t result = WICED_BT_SUCCESS;

    wiced_set_debug_uart(WICED_ROUTE_DEBUG_TO_PUART);
    WICED_BT_TRACE( "\n--------------------------------------------------------- \n"
                     "              PWM GPIO Application \n"
                     "---------------------------------------------------------\n");

    /* Register BT stack callback*/
    result = wiced_bt_stack_init(bt_cback, NULL, NULL);

    if(WICED_BT_SUCCESS != result)
    {
        WICED_BT_TRACE("Stack Initialization Failed!!\n\r");
    }
}

/*
 Function Name:
 bt_cback

 Function Description:
 @brief  Callback function that will be invoked by application_start()

 @param  event           Bluetooth management event type
 @param  p_event_data    Pointer to the the bluetooth management event data

 @return        status of the callback function
 */
wiced_result_t bt_cback(wiced_bt_management_evt_t event,
                        wiced_bt_management_evt_data_t *p_event_data)
{
    wiced_result_t result = WICED_BT_SUCCESS;

    switch(event)
    {
    /* BlueTooth stack enabled */
    case BTM_ENABLED_EVT:
        /* Initaializing GPIO's */
        wiced_hal_gpio_configure_pin(CYBSP_D6, GPIO_OUTPUT_ENABLE, GPIO_PIN_OUTPUT_HIGH);
        wiced_hal_gpio_configure_pin(CYBSP_D7, GPIO_OUTPUT_ENABLE, GPIO_PIN_OUTPUT_LOW);

        /* Initializing PWM */
        wiced_hal_gpio_select_function(CYBSP_D6,WICED_PWM1);
        wiced_hal_gpio_select_function(CYBSP_D7,WICED_PWM1);
        wiced_hal_gpio_select_function(LED2,WICED_PWM1);

        /* Turn on LED1 */
        wiced_hal_gpio_set_pin_output(LED1, 0);

        /* Registering interrupt*/
        wiced_hal_gpio_register_pin_for_interrupt(WICED_GPIO_PIN_BUTTON_1,
                                                  button_cb,
                                                  NULL);
        wiced_hal_gpio_configure_pin(WICED_GPIO_PIN_BUTTON_1,
                                    (GPIO_INPUT_ENABLE|GPIO_PULL_UP|GPIO_EN_INT_FALLING_EDGE),
                                     GPIO_PIN_OUTPUT_HIGH);

        /* Start a thread to control LED blinking and Get memory for the thread handle */
        led_thread = wiced_rtos_create_thread();
        if (led_thread!= NULL)
        {
            wiced_rtos_init_thread( led_thread,                     // Thread handle
                                    PRIORITY_MEDIUM,                // Priority
                                    "PWM_thread",                   // Name
                                    PWM_control,                    // Function
                                    THREAD_STACK_MIN_SIZE,          // Stack
                                    NULL);                          // Function argument
        }
        else
        {
            WICED_BT_TRACE("failed to create thread!!\n\r");
            result = WICED_ERROR;
        }
        break;

    default:
        break;
    }
    return result;
}

/*
 Function Name:
 button_cb

 Function Description:
 @brief  Call back function to process the button interrupt, used for toggling
         the led and updating the pwm_flag which indicates PWM_thread to stop
         or start PWM.

 @param  data           unused
 @param  port_pin       pin number on which interrupt is received, it is set
                        internally

 @return  none
 */
void button_cb(void * data , uint8_t port_pin)
{
    uint32_t led;
    /*Toggle LED*/
    led = wiced_hal_gpio_get_pin_output(LED1);
    wiced_hal_gpio_set_pin_output(LED1, !led);

    pwm_flag = !pwm_flag;
    if(pwm_flag)
    {
        WICED_BT_TRACE("PWM Started... ");
    }
    wiced_hal_gpio_clear_pin_interrupt_status(port_pin);
}

/*
 Function Name:
 PWM_control

 Function Description:
 @brief  sensor thread responsible for controlling PWM

 @param  arg           unused

 @return  none
 */
void PWM_control(uint32_t arg)
{
    uint32_t led;
    uint32_t duty_cycle = 0;
    uint8_t  incr = TRUE;
    uint32_t current_duty_cycle = duty_cycle;

    PwmClockType clk = LHL_CLK;
    wiced_pwm_config_t config;

#if ENABLE_PMU_CLK
    clk=PMU_CLK;
    wiced_hal_aclk_enable(PWM_INP_CLK_IN_HZ, WICED_ACLK1, WICED_ACLK_FREQ_24_MHZ);
#endif

    wiced_hal_pwm_get_params(PWM_INP_CLK_IN_HZ, duty_cycle, PWM_FREQ_IN_HZ, &config);
    wiced_hal_pwm_start(PWM1,
                        clk,
                        config.toggle_count,
                        config.init_count,
                        INVERT_LED);

    WICED_BT_TRACE("\n<< PWM configurations >>\n"
                   "PWM Input clock source   : %s\n"
                   "PWM Input clock Frequency: %d Hz\n"
                   "PWM Output Frequency     : %d Hz\n",
                   clk==0? "LHL_CLK" : "PMU_CLK",
                   PWM_INP_CLK_IN_HZ,
                   PWM_FREQ_IN_HZ);
    WICED_BT_TRACE("\nPress User Button (SW3) to control PWM on LED2, and toggle LED1.\n\n");

    while(1)
    {
        if(pwm_flag==TRUE)
        {
            if(incr==TRUE)
            {
                duty_cycle += DUTY_STEP_SIZE;
                if (duty_cycle == 100)
                {
                    incr = FALSE;
                }
            }

            else
            {
                duty_cycle -= DUTY_STEP_SIZE;
                if(duty_cycle == 0)
                {
                    incr = TRUE;
                }
            }

            wiced_hal_pwm_get_params(PWM_INP_CLK_IN_HZ,
                                     duty_cycle,
                                     PWM_FREQ_IN_HZ,
                                     &config);

            wiced_hal_pwm_change_values(PWM1,
                                        config.toggle_count,
                                        config.init_count);

        }

        else if (current_duty_cycle != duty_cycle)
        {
            WICED_BT_TRACE("PWM Stopped at %d%% duty cycle.\n\r", current_duty_cycle=duty_cycle);
        }
        
        /* Send the thread to sleep for a period of time */
        wiced_rtos_delay_milliseconds(THREAD_DELAY_IN_MS, ALLOW_THREAD_TO_SLEEP);
    }

}