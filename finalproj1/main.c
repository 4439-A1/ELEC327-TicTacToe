#include "ti_msp_dl_config.h"
#include <ti/devices/msp/msp.h>
#include <ti/driverlib/dl_gpio.h>
#include <ti/driverlib/dl_i2c.h>
#include <stdio.h>

#define BUZZER_IO BUZZER_IO_PIN_0_PIN
#define MOTOR     MOTOR_MOTOR_PIN_PIN

#define sysclk 32768000


#define NOTE_DURATION_MS 300

void delayMs(uint32_t ms) {
    uint32_t cycles = (ms * (sysclk / 1000));
    for (volatile uint32_t i = 0; i < cycles / 10; i++);
}

void playToneAndVibrate(uint32_t freqHz, uint32_t duration_ms) {
    uint32_t period_us = 1000000 / freqHz;
    uint32_t half_period_us = period_us / 2;

    uint32_t elapsed_us = 0;
    uint32_t motor_toggle_interval_us = 20000; // motor toggles every 20ms
    uint32_t motor_timer_us = 0;
    uint8_t motor_state = 0;

    while (elapsed_us < duration_ms * 1000) {
        // Turn on buzzer
        BUZZER_IO_PORT->DOUTSET31_0 = BUZZER_IO;

        // Check if we need to toggle the motor NMOS' gate input
        if (motor_timer_us >= motor_toggle_interval_us) {
            motor_timer_us = 0;
            motor_state = !motor_state;
            if (motor_state) {
                MOTOR_PORT->DOUTSET31_0 = MOTOR;
            } else {
                MOTOR_PORT->DOUTCLR31_0 = MOTOR;
            }
        }

        delayMs(half_period_us / 1000);
        elapsed_us += half_period_us; motor_timer_us += half_period_us;

        // Buzzer OFF
        BUZZER_IO_PORT->DOUTCLR31_0 = BUZZER_IO;

        delayMs(half_period_us / 1000);
        elapsed_us += half_period_us;
        motor_timer_us += half_period_us;
    }

    // Ensure everything is OFF
    BUZZER_IO_PORT->DOUTCLR31_0 = BUZZER_IO;
    MOTOR_PORT->DOUTCLR31_0 = MOTOR;
}


int main(void)
{
    SYSCFG_DL_init();  // initializes GPIOs as configured in SysConfig

    while (1)
    {
        // Play a C4 note (~262 Hz)
        playToneAndVibrate(262, NOTE_DURATION_MS);

        delayMs(1000);  // pause before repeating
    }
}


