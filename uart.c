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

void cb_init(void)
{
    struct DirBuff *dbuffs[] = {&(cb.rx_buff), &(cb.tx_buff)};
    for (uint8_t i = 0; i < 2; i++) {
        dbuffs[i]->start_ptr = &dbuffs[i]->buff[0];
        dbuffs[i]->end_ptr = &dbuffs[i]->buff[BUFFSIZE];
        dbuffs[i]->inpos_ptr = &dbuffs[i]->buff[0];
        dbuffs[i]->outpos_ptr = &dbuffs[i]->buff[0];
        dbuffs[i]->items = 0;
        dbuffs[i]->full = 0;
        dbuffs[i]->callback = NULL;
    }
}

void get_direction_buffer(enum DIR_BUFFS dir, struct DirBuff **dbuff)
{
    switch (dir) {
    case RX_BUFF:
        *dbuff = &(cb.rx_buff);
        break;
    case TX_BUFF:
        *dbuff = &(cb.tx_buff);
    default:
        break;
    }
}

uint8_t cb_pop(char *c, enum DIR_BUFFS dir)
{
    struct DirBuff *dbuff = NULL;

    get_direction_buffer(dir, &dbuff);

    if (!dbuff)
        return 2;

#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wunused-variable"
    ATOMIC_BLOCK(ATOMIC_RESTORESTATE)
#pragma clang diagnostic pop
    {
        if (dbuff->inpos_ptr == dbuff->outpos_ptr && dbuff->full != 1) {
            return 1;
        }
    }

    *c = *(dbuff->outpos_ptr++);

    if (dbuff->outpos_ptr == dbuff->end_ptr)
        dbuff->outpos_ptr = dbuff->start_ptr;

#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wunused-variable"
    ATOMIC_BLOCK(ATOMIC_RESTORESTATE)
#pragma clang diagnostic pop
    {
        dbuff->items--;

        if (dbuff->full)
            dbuff->full = 0;
    }

    return 0;
}

uint8_t cb_push(char c, enum DIR_BUFFS dir)
{
    struct DirBuff *dbuff = NULL;

    get_direction_buffer(dir, &dbuff);

    if (!dbuff)
        return 2;

#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wunused-variable"
    ATOMIC_BLOCK(ATOMIC_RESTORESTATE)
#pragma clang diagnostic pop
    {
        if (dbuff->inpos_ptr == dbuff->outpos_ptr && dbuff->full == 1) {
            return 1;
        }
    }

    *(dbuff->inpos_ptr++) = c;

    if (dbuff->inpos_ptr == dbuff->end_ptr)
        dbuff->inpos_ptr = dbuff->start_ptr;

#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wunused-variable"
    ATOMIC_BLOCK(ATOMIC_RESTORESTATE)
#pragma clang diagnostic pop
    {
        dbuff->items++;

        if (dbuff->inpos_ptr == dbuff->outpos_ptr)
            dbuff->full = 1;
    }

    return 0;
}

void init_uart_cfg(struct UARTcfg *cfg)
{
    /* init config with default values */
    if (cfg) {
        cfg->tx = _BV(TXEN0);
        cfg->rx = _BV(RXEN0);
        cfg->tx_callback = NULL;
        cfg->rx_callback = NULL;
    }
};

void init_UART(const struct UARTcfg *cfg)
{
    cb_init();

    UBRR0H = UBRRH_VALUE; /* set baud rate */
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
    /* activate send, receive plus receive interrupt */
    UCSR0B |= cfg->tx | cfg->rx | _BV(RXCIE0);
}

#ifdef LIB_DEBUG

void put_noi_UART(char c)
{
    /* Stay here until data buffer is empty */
    while (!(UCSR0A & _BV(UDRE0)))
        ;
    /* write data */
    UDR0 = c;
}
void puts_noi_UART(const char *s)
{
    while (*s) {
        put_noi_UART(*s);
        s++;
    }
    char *cr_ptr = CR;
    while (*cr_ptr) {
        put_noi_UART(*cr_ptr);
        cr_ptr++;
    }
}

#endif /* LIB_DEBUG */

void put_UART(const char c)
{
    if (cb_push(c, TX_BUFF) == 0)
        UCSR0B |= _BV(UDRIE0); /* activate buffer empty interrupt */
}

void puts_UART(const char *s)
{
    while (*s) {
        put_UART(*s);
        s++;
    }
    char *cr_ptr = CR;
    while (*cr_ptr) {
        put_UART(*cr_ptr);
        cr_ptr++;
    }
}

uint8_t get_UART(char *s)
{
    char *s_ptr = s;
    if (cb_pop(s_ptr, RX_BUFF) == 0)
        return 0;
    return 1;
}

uint8_t gets_UART(char *s)
{
    char *s_ptr = s;
    if (cb.rx_buff.items > 0) {
        while ((cb_pop(s_ptr, RX_BUFF) == 0)) {
            s_ptr++;
        }
        s_ptr++;
        *s_ptr = '\0';
        return 0;
    } else {
        return 1;
    }
}

ISR(USART_RX_vect)
{
    cb_push(UDR0, RX_BUFF);
    if (cb.rx_buff.callback)
        cb.rx_buff.callback();
}

ISR(USART_UDRE_vect)
{
    char c = 0;
    if (cb_pop(&c, TX_BUFF) != 0) {
        UCSR0B &= ~(_BV(UDRIE0));
    } else {
        UDR0 = c;
    }
}
