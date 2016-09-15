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

#ifndef UART_H
#define UART_H

/*
 * Make sure we have defined F_CPU before we include anything. This is very
 * important and you must change this value according to your needs. The easiest
 * way to do this is to use this in a Makefile
 * -DF_CPU=1000000
 */
#ifndef F_CPU
#define F_CPU 16000000UL /* 16MHz µc */
#endif                   /* ifndef F_CPU */
#ifndef BAUD
#define BAUD 9600L
#endif /* ifndef BAUD */
/*
 * Some basic includes
 */
#include <inttypes.h>

#include <avr/io.h>
#include <util/setbaud.h>

struct uart_cfg {
    uint8_t tx;
    uint8_t tx_int;
    uint8_t rx;
    uint8_t rx_int;
};

void init_uart_cfg(struct uart_cfg *cfg);

/**
 * @brief
 */
void init_UART(const struct uart_cfg *cfg);
/**
 * @brief Send a single character via UART
 *
 * @param c The character to send
 */
void put_UART(unsigned char c);

/**
 * @brief Write a string to the UART buffer
 *
 * @param s The string you want to send
 */
void puts_UART(const char *s);

#endif /* ifndef UART_H */
