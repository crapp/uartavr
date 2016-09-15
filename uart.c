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

#include "uart.h"

void init_uart_cfg(struct uart_cfg *cfg)
{
    if (cfg) {
        cfg->tx = _BV(TXEN0);
        cfg->tx_int = _BV(TXCIE0);
        cfg->rx = _BV(RXEN0);
        cfg->rx_int = _BV(RXCIE0);
    }
};

void init_UART(const struct uart_cfg *cfg)
{
    UBRR0H = UBRRH_VALUE;  // set baud rate
    UBRR0L = UBRRL_VALUE;

#if USE_2X
    /* U2X-Modus is necessary */
    UCSR0A |= _BV(U2X0);
#else
    /* U2X-Modus not necessary */
    UCSR0A &= ~(_BV(U2X0));
#endif

    /* 8-bit */
    UCSR0C = _BV(UCSZ01) | _BV(UCSZ00);
    /* as we did not touch UPMn0 UPMn1 and USBSn we are now using 8N1 */
    /* activate send and receive */
    UCSR0B |= cfg->tx | cfg->rx;
}

void put_UART(unsigned char c)
{
    /*Stay here until data buffer is empty*/
    while (!(UCSR0A & _BV(UDRE0)))
        ;
    /* write data */
    UDR0 = c;
}

void puts_UART(const char *s)
{
    while (*s) {
        put_UART(*s);
        s++;
    }
}
