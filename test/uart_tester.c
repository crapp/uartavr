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

#include <avr/io.h>
#include <avr/sleep.h>

#include <util/delay.h>

#include <string.h>

#include "uart.h"

/*
 * You can use this example to echo back data that was send to the
 * microcontroller.
 * This example makes use of the idle sleep mode
 */

volatile uint8_t wakeup;

void rx_cb(void)
{
    if (cb.rx_buff.items == 5) {
        wakeup = 1;
    }
}

int main(void)
{
    struct UARTcfg cfg;
    memset(&cfg, 0, sizeof(struct UARTcfg));

    init_uart_cfg(&cfg);
    init_UART(&cfg);
    cb.rx_buff.rx_callback = rx_cb;

    wakeup = 0;

    set_sleep_mode(SLEEP_MODE_IDLE);
    sleep_enable();

    sei();
    while (1) {
        while (!wakeup) {
            sleep_mode();
        }
        char s[6];
        /* echo the string back */
        gets_UART(&s[0]);
        puts_UART(&s[0]);
        wakeup = 0;
    };

    return 0;
}
