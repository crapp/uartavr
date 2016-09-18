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
#include <stdio.h>
#include <stdlib.h>

#include <avr/io.h>
#include <util/atomic.h>
#include <util/setbaud.h>

/** @file */

#define UARTAVR_VERSION_MAJOR 0
#define UARTAVR_VERSION_MINOR 1
#define UARTAVR_VERSION_PATCH 0

/**
 * @brief Define the line ending you need. This one will work with minicom on linux
 */
#define CR "\n\r"

/**
 * @brief The buffer size for the RX and TX buffers
 */
#define BUFFSIZE 128

#define LIB_DEBUG

//http://stackoverflow.com/a/14047028/1127601

/**
 * @brief Presenting a circular buffer
 */
struct DirBuff {
    char buff[BUFFSIZE];
    char *start_ptr;
    char *end_ptr;
    char *inpos_ptr;
    char *outpos_ptr;
    uint16_t items;
    uint8_t full;
    void (*callback)(void);
};

/**
 * @brief Identifier for direction buffer
 */
enum DIR_BUFFS { RX_BUFF, TX_BUFF };

/**
 * @brief This holds the circular buffers
 */
struct CBuffer {
    struct DirBuff rx_buff; /**< RX Buffer */
    struct DirBuff tx_buff; /**< TX Buffer */
} cb;

/**
 * @brief A struct to configure the UART
 *
 * @sa init_uart_cfg
 */
struct UARTcfg {
    uint8_t tx; /**< Activate TX, use the appropriate Byte for your platform */
    uint8_t rx; /**< Activate RX, use the appropriate Byte for your platform */
    void (*rx_callback)(
        void); /**< Callback function that will be called from RX ISR */
    void (*tx_callback)(
        void); /**< Callback function that will be called from TX ISR */
};

/**
 * @brief Initializes the circular buffer structure.
 *
 * @warning Do not call this function yourself. The init_UART() function takes
 * care of this.
 */
void cb_init(void);

void get_direction_buffer(enum DIR_BUFFS dir, struct DirBuff **dbuff);

uint8_t cb_pop(char *c, enum DIR_BUFFS dir);
uint8_t cp_push(char c, enum DIR_BUFFS dir);

/**
 * @brief Init a cfg struct with the default values
 *
 * @param cfg
 */
void init_uart_cfg(struct UARTcfg *cfg);

/**
 * @brief
 *
 * @detail
 * This method also initializes a circular buffer. The buffer is used by this
 * UART implementation to store data that will either be send or was received.
 * This way we can use UART interrupts and the write functions are not blocking.
 * The read functions don't have to poll the UART RX register for changes.
 */
void init_UART(const struct UARTcfg *cfg);

#ifdef LIB_DEBUG
void put_noi_UART(const char c);
void puts_noi_UART(const char *s);
#endif /* LIB_DEBUG */

/**
 * @brief Send a single character via UART
 *
 * @param c The character to send
 */
void put_UART(const unsigned char c);

/**
 * @brief Write a string to the UART buffer
 *
 * @param s The string you want to send
 */
void puts_UART(const char *s);

/**
 * @brief Retrieve one char from the buffer
 *
 * @param s Pointer to a char variable
 *
 * @return 0 if character was retrieved, 1 otherwise
 */
uint8_t get_UART(char *s);
/**
 * @brief Get all data from the circular buffer
 *
 * @param s Pointer an array which this function uses to store the data from cb
 * @return 0 on success or 1 if something went wrong
 */
uint8_t gets_UART(char *s);

/** @} */

#endif /* ifndef UART_H */
