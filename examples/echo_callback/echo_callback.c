/*
 * uartavr interrupt driven serial communication for 8bit avrs
 * Copyright © 2016 Christian Rapp
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *
 * 1. Redistributions of source code must retain the above copyright
 * notice, this list of conditions and the following disclaimer.
 *
 * 2. Redistributions in binary form must reproduce the above copyright
 * notice, this list of conditions and the following disclaimer in the
 * documentation and/or other materials provided with the distribution.
 *
 * 3. Neither the name of the organization nor the
 * names of its contributors may be used to endorse or promote products
 * derived from this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY ''AS IS'' AND ANY  EXPRESS OR IMPLIED
 * WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
 * WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
 * DISCLAIMED. IN NO EVENT SHALL yourname BE LIABLE FOR ANY
 * DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
 * (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
 * LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
 * ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
 * SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

/*H**********************************************************************
* FILENAME :        echo_callback.c
*
* DESCRIPTION :
*       Echo example for uartavr
*
* NOTES :
*       You can use this example to echo back data that was send to the
*       microcontroller.
*       This example makes use of the idle sleep mode.
*
*       This example was written for the ATmega328P
*
* AUTHOR :    Christian Rapp
*/

#include <avr/io.h>
#include <avr/sleep.h>

#include <string.h>

#include "uart.h"

volatile uint8_t wakeup; /* wake up signal for the main loop */

/**
 * The callback function that will be called from USART_RX_vect
 * I use it here to check the number of bytes currently in the buffer. If we have
 * 5 bytes we signal the main loop to wake up. You need a byte array with size 6
 * to fetch the data as gets_UART automatically appends a \0 terminating
 * character
 */
void rx_cb(void)
{
    if (cb.rx_buff.items == 5) {
        wakeup = 1;
    }
}

int main(void)
{
    /* set up uart with default values */
    struct UARTcfg cfg;
    memset(&cfg, 0, sizeof(struct UARTcfg));

    init_uart_cfg(&cfg);
    init_UART(&cfg);
    cb.rx_buff.rx_callback = rx_cb;

    wakeup = 0;

    /* we want to use sleep mode idle because uart can wake up the controller
     * from this mode */
    set_sleep_mode(SLEEP_MODE_IDLE);
    sleep_enable();

    sei();
    while (1) {
        /* sending data will wake the microntroller up but we don't want to
         * continue yet so go to sleep once more */
        while (!wakeup) {
            sleep_mode();
        }
        /* size 6 is important */
        char s[6];
        /* echo the string back */
        gets_UART(&s[0]);
        puts_UART(&s[0]);
        wakeup = 0;
    };

    return 0;
}
