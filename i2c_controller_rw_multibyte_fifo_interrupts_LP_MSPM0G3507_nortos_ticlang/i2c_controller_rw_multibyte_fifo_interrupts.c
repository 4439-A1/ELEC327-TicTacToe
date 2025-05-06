/*
 * Copyright (c) 2021, Texas Instruments Incorporated
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 *
 * *  Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 *
 * *  Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 *
 * *  Neither the name of Texas Instruments Incorporated nor the names of
 *    its contributors may be used to endorse or promote products derived
 *    from this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO,
 * THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR
 * PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR
 * CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
 * EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
 * PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS;
 * OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY,
 * WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR
 * OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE,
 * EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

#include "ti_msp_dl_config.h"
#include "ti/driverlib/dl_gpio.h"
#include "ti/driverlib/m0p/dl_core.h"
#include "ti/driverlib/m0p/dl_sysctl.h"
#include "core_cm0plus.h"


#define BUZZER_IO BUZZER_IO_PIN_0_PIN
#define MOTOR     MOTOR_MOTOR_PIN_PIN
#define sysclk 32768000
#define NOTE_DURATION_MS 300

#define I2C_INST I2C1
#define I2C_INST_INT_IRQn I2C1_INT_IRQn

/* Maximum size of TX packet */
#define I2C_TX_MAX_PACKET_SIZE (16)

/* Number of bytes to send to target device */
#define I2C_TX_PACKET_SIZE (16)

/* Maximum size of RX packet */
#define I2C_RX_MAX_PACKET_SIZE (16)

/* Number of bytes to received from target */
#define I2C_RX_PACKET_SIZE (16)

/* I2C Target address */
#define I2C_TARGET_ADDRESS (0x2E)

void i2c_send(uint8_t *data, uint32_t len);
void I2C1_IRQHandler(void);

/* Data sent to the Target */
//uint8_t gTxPacket[I2C_TX_MAX_PACKET_SIZE] = {0x00, 0x01, 0x02, 0x03, 0x04,
//    0x05, 0x06, 0x07, 0x08, 0x09, 0x0A, 0x0B, 0x0C, 0x0D, 0x0E, 0x0F};

// Enable NeoPixel output on pin 0 (mandatory before setting color)

//uint8_t gTxPacket[] = {
//    0x0E,  // NeoPixel base register
//    0x00,  // Subcommand: Enable NeoPixels
//    0x10,  // Number of pixels
//    0x01   // Type: RGB
//};
//uint8_t setPixel[] = {
//    0x0E,       // NeoPixel base register
//    0x01,       // Subcommand: Set pixel color
//    0x05,       // Pixel index 0
//    0x00, 0xFF, 0x00  // GRB: green
//};
//
//uint8_t show[] = {
//    0x0E,  // NeoPixel base register
//    0x02   // Subcommand: Show pixels
//};

uint8_t enable_interrupt_output[] = { 0x00, 0x02, 0x04 };

uint8_t setBufLen[] = {
    0x0E, 0x04,   // SEESAW_NEOPIXEL_BASE, SEESAW_NEOPIXEL_BUF_LENGTH
    0x00, 0x30    // 0x0030 = 48 bytes = 16 pixels × 3 bytes
};

uint8_t setPin[] = {
    0x0E, 0x01,  // SEESAW_NEOPIXEL_BASE, SEESAW_NEOPIXEL_PIN
    0x00         // Use pin 0 (NeoTrellis default)
};

uint8_t setSpeed[] = {
    0x0E, 0x0B,   // SEESAW_NEOPIXEL_BASE, SEESAW_NEOPIXEL_SPEED
    0x01          // 0x01 = 800kHz (default NeoPixel)
};

uint8_t enableNeopixels[] = {
    0x0E, 0x00,   // SEESAW_NEOPIXEL_BASE, SEESAW_NEOPIXEL_ENABLE
    0x10,         // 16 pixels
    0x01          // GRB
};

uint8_t setPixel[] = {
    0x0E, 0x03,   // SEESAW_NEOPIXEL_BASE, SEESAW_NEOPIXEL_BUF
    0x00, 0x00,   // offset = 0 (pixel 0 * 3)
    0x00, 0xFF, 0x00  // RGB = red=0, green=255, blue=0 (GRB order)
};

uint8_t show[] = {
    0x0E, 0x02  // SEESAW_NEOPIXEL_BASE, SEESAW_NEOPIXEL_SHOW
};

uint8_t *gTxBuf;

/* Counters for TX length and bytes sent */
uint32_t gTxLen, gTxCount;

/* Data received from Target */
uint8_t gRxPacket[I2C_RX_MAX_PACKET_SIZE];
/* Counters for TX length and bytes sent */
uint32_t gRxLen, gRxCount;
/* Indicates status of I2C */
enum I2cControllerStatus {
    I2C_STATUS_IDLE = 0,
    I2C_STATUS_TX_STARTED,
    I2C_STATUS_TX_INPROGRESS,
    I2C_STATUS_TX_COMPLETE,
    I2C_STATUS_RX_STARTED,
    I2C_STATUS_RX_INPROGRESS,
    I2C_STATUS_RX_COMPLETE,
    I2C_STATUS_ERROR,
} gI2cControllerStatus;

//int main(void)
//{
//    SYSCFG_DL_init();
//
//    /* Set LED to indicate start of transfer */
//    DL_GPIO_setPins(GPIO_LEDS_PORT, GPIO_LEDS_USER_LED_1_PIN);
//
//    NVIC_EnableIRQ(I2C_INST_INT_IRQn);
//    DL_SYSCTL_disableSleepOnExit();
//
//    gI2cControllerStatus = I2C_STATUS_IDLE;
//
//    gTxLen = I2C_TX_PACKET_SIZE;
//    /*
//     * Fill the FIFO
//     *  The FIFO is 8-bytes deep, and this function will return number
//     *  of bytes written to FIFO */
//    gTxCount = DL_I2C_fillControllerTXFIFO(I2C_INST, &gTxPacket[0], gTxLen);
//    /* Enable TXFIFO trigger interrupt if there are more bytes to send */
//    if (gTxCount < gTxLen) {
//        DL_I2C_enableInterrupt(
//            I2C_INST, DL_I2C_INTERRUPT_CONTROLLER_TXFIFO_TRIGGER);
//    } else {
//        DL_I2C_disableInterrupt(
//            I2C_INST, DL_I2C_INTERRUPT_CONTROLLER_TXFIFO_TRIGGER);
//    }
//
//    /*
//     * Send the packet to the controller.
//     * This function will send Start + Stop automatically.
//     */
//    gI2cControllerStatus = I2C_STATUS_TX_STARTED;
//    while (!(
//        DL_I2C_getControllerStatus(I2C_INST) & DL_I2C_CONTROLLER_STATUS_IDLE))
//        ;
//    DL_I2C_startControllerTransfer(
//        I2C_INST, I2C_TARGET_ADDRESS, DL_I2C_CONTROLLER_DIRECTION_TX, gTxLen);
//
//    /* Wait until the Controller sends all bytes */
//    while ((gI2cControllerStatus != I2C_STATUS_TX_COMPLETE) &&
//           (gI2cControllerStatus != I2C_STATUS_ERROR)) {
//        __WFE();
//    }
//
//    while (DL_I2C_getControllerStatus(I2C_INST) &
//           DL_I2C_CONTROLLER_STATUS_BUSY_BUS)
//        ;
//
//    /* Trap if there was an error */
//    if (DL_I2C_getControllerStatus(I2C_INST) &
//        DL_I2C_CONTROLLER_STATUS_ERROR) {
//        /* LED will remain high if there is an error */
//        __BKPT(0);
//    }
//
//    while (!(
//        DL_I2C_getControllerStatus(I2C_INST) & DL_I2C_CONTROLLER_STATUS_IDLE))
//        ;
//
//    /* Add delay between transfers */
//    delay_cycles(1000);
//
//    /* Send a read request to Target */
//    gRxLen               = I2C_RX_PACKET_SIZE;
//    gRxCount             = 0;
//    gI2cControllerStatus = I2C_STATUS_RX_STARTED;
//    DL_I2C_startControllerTransfer(
//        I2C_INST, I2C_TARGET_ADDRESS, DL_I2C_CONTROLLER_DIRECTION_RX, gRxLen);
//
//    /* Wait for all bytes to be received in interrupt */
//    while (gI2cControllerStatus != I2C_STATUS_RX_COMPLETE) {
//        __WFE();
//    }
//
//    while (DL_I2C_getControllerStatus(I2C_INST) &
//           DL_I2C_CONTROLLER_STATUS_BUSY_BUS)
//        ;
//
//    /* If write and read were successful, toggle LED */
//    while (1) {
//        DL_GPIO_togglePins(GPIO_LEDS_PORT,
//            GPIO_LEDS_USER_LED_1_PIN | GPIO_LEDS_USER_TEST_PIN);
//        delay_cycles(16000000);
//    }
//}

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
        elapsed_us += half_period_us;
        motor_timer_us += half_period_us;

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
    SYSCFG_DL_init();

    DL_GPIO_setPins(GPIO_LEDS_PORT, GPIO_LEDS_USER_LED_1_PIN);

    NVIC_EnableIRQ(I2C_INST_INT_IRQn);
    DL_SYSCTL_disableSleepOnExit();

    gI2cControllerStatus = I2C_STATUS_IDLE;
    while (DL_I2C_getControllerStatus(I2C_INST) & DL_I2C_CONTROLLER_STATUS_BUSY_BUS);
//    DL_I2C_startControllerTransfer(I2C_INST, 0x2E, DL_I2C_CONTROLLER_DIRECTION_TX, 0);
    uint8_t probe = 0x00;  // STATUS register on NeoTrellis
    i2c_send(&probe, 1);
// Wait for TX to complete
    while ((gI2cControllerStatus != I2C_STATUS_TX_COMPLETE) &&
           (gI2cControllerStatus != I2C_STATUS_ERROR)) {
        __WFE();
    }
//    __BKPT(0);
    if (gI2cControllerStatus == I2C_STATUS_ERROR) {
        // Device NACK'd or not responding
    } else {
        DL_GPIO_setPins(GPIO_LEDS_PORT, GPIO_LEDS_USER_TEST_PIN);
    }

    DL_GPIO_setPins(GPIO_LEDS_PORT, GPIO_LEDS_USER_TEST_PIN);
    while (!(DL_I2C_getControllerStatus(I2C_INST) & DL_I2C_CONTROLLER_STATUS_IDLE));

    if (DL_I2C_getControllerStatus(I2C_INST) & DL_I2C_CONTROLLER_STATUS_ERROR) {
        // NACK received — device not responding
//        __BKPT(0);
    }

//    i2c_send(gTxPacket, sizeof(gTxPacket));  // Enable NeoPixel
//    delay_cycles(500000);
//
//    i2c_send(setPixel, sizeof(setPixel));    // Set pixel 0 to red
////    __BKPT(0); // reaches this breakpoint but the pixel doesn't turn red
//    delay_cycles(500000);
//
//    i2c_send(show, sizeof(show));            // Show pixel
////    __BKPT(0); // also reaches this breakpoint
//    delay_cycles(500000);

    i2c_send(setPin, sizeof(setPin));
//    __BKPT(0);
    delay_cycles(1600000);

    i2c_send(setBufLen, sizeof(setBufLen));
//    __BKPT(0);
    delay_cycles(1600000);

    i2c_send(setSpeed, sizeof(setSpeed));
//    __BKPT(0);
    delay_cycles(1600000);

    i2c_send(enableNeopixels, sizeof(enableNeopixels));
//    __BKPT(0);
    delay_cycles(1600000);

    i2c_send(setPixel, sizeof(setPixel));
//    __BKPT(0);
    delay_cycles(1600000);

    i2c_send(show, sizeof(show));
//    __BKPT(0);
    delay_cycles(1600000);


    // Optionally toggle onboard LED forever
    DL_GPIO_setPins(GPIO_LEDS_PORT, GPIO_LEDS_USER_TEST_PIN);  // leave ON for visual feedback

//    while (1) {
//        DL_GPIO_togglePins(GPIO_LEDS_PORT,
//            GPIO_LEDS_USER_LED_1_PIN | GPIO_LEDS_USER_TEST_PIN);
//        delay_cycles(16000000);
//    }

    // Enable GPIO interrupt in NVIC
    NVIC_EnableIRQ(GPIOA_INT_IRQn);  // Enables GPIOA interrupt group

    // Optional: clear any lingering interrupts from before
    DL_GPIO_clearInterruptStatus(GPIOA, DL_GPIO_PIN_28);

    // Optional: show startup state (e.g., LED OFF)
    DL_GPIO_clearPins(GPIO_LEDS_PORT, GPIO_LEDS_USER_TEST_PIN);

    __enable_irq();

    while (1) {
        __WFI();  // Wait For Interrupt: saves power
    }

    //
    // Program compiles and reaches this while loop when debugging, but there may be an issue with how the data bits are being sent
    //


}

void i2c_send(uint8_t *data, uint32_t len) {
    gTxBuf = data;
    gTxLen = len;
    gTxCount = 0;
    gTxCount += DL_I2C_fillControllerTXFIFO(I2C_INST, gTxBuf, gTxLen);

    if (gTxCount < gTxLen) {
        DL_I2C_enableInterrupt(I2C_INST, DL_I2C_INTERRUPT_CONTROLLER_TXFIFO_TRIGGER);
    } else {
        DL_I2C_disableInterrupt(I2C_INST, DL_I2C_INTERRUPT_CONTROLLER_TXFIFO_TRIGGER);
    }

    gI2cControllerStatus = I2C_STATUS_TX_STARTED;
    while (!(DL_I2C_getControllerStatus(I2C_INST) & DL_I2C_CONTROLLER_STATUS_IDLE));
    DL_I2C_startControllerTransfer(I2C_INST, I2C_TARGET_ADDRESS, DL_I2C_CONTROLLER_DIRECTION_TX, gTxLen);

    while ((gI2cControllerStatus != I2C_STATUS_TX_COMPLETE) &&
           (gI2cControllerStatus != I2C_STATUS_ERROR)) {
        __WFE();
    }

    while (DL_I2C_getControllerStatus(I2C_INST) & DL_I2C_CONTROLLER_STATUS_BUSY_BUS);
}



void I2C_INST_IRQHandler(void)
{
    switch (DL_I2C_getPendingInterrupt(I2C_INST)) {
        case DL_I2C_IIDX_CONTROLLER_RX_DONE:
            gI2cControllerStatus = I2C_STATUS_RX_COMPLETE;
            break;
        case DL_I2C_IIDX_CONTROLLER_TX_DONE:
            DL_I2C_disableInterrupt(
                I2C_INST, DL_I2C_INTERRUPT_CONTROLLER_TXFIFO_TRIGGER);
            gI2cControllerStatus = I2C_STATUS_TX_COMPLETE;
            break;
        case DL_I2C_IIDX_CONTROLLER_RXFIFO_TRIGGER:
            gI2cControllerStatus = I2C_STATUS_RX_INPROGRESS;
            /* Receive all bytes from target */
            while (DL_I2C_isControllerRXFIFOEmpty(I2C_INST) != true) {
                if (gRxCount < gRxLen) {
                    gRxPacket[gRxCount++] =
                        DL_I2C_receiveControllerData(I2C_INST);
                } else {
                    /* Ignore and remove from FIFO if the buffer is full */
                    DL_I2C_receiveControllerData(I2C_INST);
                }
            }
            break;
        case DL_I2C_IIDX_CONTROLLER_TXFIFO_TRIGGER:
            gI2cControllerStatus = I2C_STATUS_TX_INPROGRESS;
            /* Fill TX FIFO with next bytes to send */
            if (gTxCount < gTxLen) {
                gTxCount += DL_I2C_fillControllerTXFIFO(
                    I2C_INST, &gTxBuf[gTxCount], gTxLen - gTxCount);
            }
            break;
            /* Not used for this example */
        case DL_I2C_IIDX_CONTROLLER_ARBITRATION_LOST:
        case DL_I2C_IIDX_CONTROLLER_NACK:
            if ((gI2cControllerStatus == I2C_STATUS_RX_STARTED) ||
                (gI2cControllerStatus == I2C_STATUS_TX_STARTED)) {
                /* NACK interrupt if I2C Target is disconnected */
                gI2cControllerStatus = I2C_STATUS_ERROR;
            }
            break;
        case DL_I2C_IIDX_CONTROLLER_RXFIFO_FULL:
        case DL_I2C_IIDX_CONTROLLER_TXFIFO_EMPTY:
        case DL_I2C_IIDX_CONTROLLER_START:
        case DL_I2C_IIDX_CONTROLLER_STOP:
        case DL_I2C_IIDX_CONTROLLER_EVENT1_DMA_DONE:
        case DL_I2C_IIDX_CONTROLLER_EVENT2_DMA_DONE:
        default:
            break;
    }
    DL_I2C_clearInterruptStatus(I2C_INST, DL_I2C_getPendingInterrupt(I2C_INST));
}

void I2C1_IRQHandler(void)
{
    I2C_INST_IRQHandler();
}

void GPIOA_IRQHandler(void)
{
    if (DL_GPIO_getPendingInterrupt(GPIOA) & DL_GPIO_PIN_28) {
        DL_GPIO_clearInterruptStatus(GPIOA, DL_GPIO_PIN_28);

        DL_GPIO_togglePins(GPIO_LEDS_PORT, GPIO_LEDS_USER_TEST_PIN);

        // Play tone and vibrate
//        playToneAndVibrate(262, NOTE_DURATION_MS);
    }
}

