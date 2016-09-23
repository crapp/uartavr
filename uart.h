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
#include <stdlib.h>
#ifdef PRINTF
#include <stdio.h>
#endif

#include <avr/io.h>
#include <util/atomic.h>
#include <util/setbaud.h>

/**
 * @file uart.h
 *
 * @author Christian Rapp
 * @date 2016
 * @copyright BSD-3-Clause
 * @pre This library was written and tested fo the Atmege328P microcontroller. You
 * may need to change the registers if you want to port this to a different AVR.
 *
 * @details
 *
 * uartavr is a simple interrupt driven UART implementation for Atmel AVRs. This
 * library uses a circular buffer data structure to store data that should be send
 * or was received. In general the user does not have to interact directly with
 * this buffer as there are some convenience methods already available.
 *
 * In order to use this implementation you have to use [sei](http://www.nongnu.org/avr-libc/user-manual/group__avr__interrupts.html#gaad5ebd34cb344c26ac87594f79b06b73)
 * which enables interrupts by setting the global interrupt mask.
 *
 */

/**
 * @brief Version major
 */
#define UARTAVR_VERSION_MAJOR 0
/**
 * @brief Version minor
 */
#define UARTAVR_VERSION_MINOR 2
/**
 * @brief Version patch
 */
#define UARTAVR_VERSION_PATCH 0

/**
 * @brief Define the line ending you need. This one will work with minicom on linux
 */
#define CR "\n\r"

/**
 * @brief The buffer size for the RX and TX buffers
 */
#define BUFFSIZE 64

/**
 * @brief Presenting a circular buffer
 *
 * @details
 * The buffer is used by this UART implementation to store data that will either
 * be send or was received. This way we can use UART interrupts and the write or
 * read functions are not blocking.
 */
struct DirBuff {
    char buff[BUFFSIZE];       /**< The buffer holding the data that should be
                                 send or was received */
    char *start_ptr;           /**< A pointer to the start of the buffer */
    char *end_ptr;             /**< A pointer to theend of the buffer */
    char *inpos_ptr;           /**< The write pointer position */
    char *outpos_ptr;          /**< The read pointer position */
    size_t items;              /**< Number of items in the buffer */
    uint8_t full;              /**< Indicates if the buffer is full, meaning BUFFSIZE
                                 number of items are stored */
    void (*rx_callback)(void); /**< A callback function you can use to get
                                 notified if a byte was received */
    void (*tx_callback)(void); /**< Callback when a byte was sent */
    void (*buff_empty)(void);  /**< Callback when buff is empty */
};

/**
 * @brief Identifier for direction buffer
 */
enum DIR_BUFFS {
    RX_BUFF, /**< RX Buffer identifier */
    TX_BUFF  /**< TX Buffer identifier */
};

/**
 * @brief This holds the circular buffers
 */
struct CBuffer {
    struct DirBuff rx_buff; /**< RX Buffer */
    struct DirBuff tx_buff; /**< TX Buffer */
} cb;                       /**< Global instance of the circular buffer */

/* TODO: I am not sure if this instance of CBuffer needs to be volatile. But if I
*  do this I get lots of compiler warnings.
*/

/**
 * @brief A struct to configure the UART
 *
 * @sa init_uart_cfg
 */
struct UARTcfg {
    uint8_t tx; /**< Activate TX, use the appropriate Byte for your platform */
    uint8_t rx; /**< Activate RX, use the appropriate Byte for your platform */
    void (*rx_callback)(void); /**< Callback function that will be called from
                                 RX ISR */
    void (*tx_callback)(void); /**< Callback function that will be called from
                                 TX ISR */
    void (*buff_empty)(void);  /**< Callback function that will be called from
                                 RX ISR */
};

/**
 * @brief Initializes the circular buffer structure.
 *
 * @warning Do not call this function yourself. The init_UART() function takes
 * care of this.
 */
void cb_init(void);

/**
 * @brief Get a direction buffer struct from CBuffer
 *
 * @param dir RX or TX
 * @param dbuff ** to a DirBuff struct
 */
void get_direction_buffer(enum DIR_BUFFS dir, struct DirBuff **dbuff);

/**
 * @brief Get one byte from the circular buffer
 *
 * @param c Pointer to char variable
 * @param dir Get the byte from the TX or RX buffer
 *
 * @return 0 If the byte has been retrieved, 1 if the buffer is empty
 */
uint8_t cb_pop(char *c, enum DIR_BUFFS dir);
/**
 * @brief Put one byte in the circular buffer
 *
 * @param c A char variable
 * @param dir Put the byte on the TX or RX buffer
 *
 * @return 0 If the byte has written successfully, 1 if the buffer is full
 */
uint8_t cp_push(char c, enum DIR_BUFFS dir);

/**
 * @brief Init a cfg struct with the default values
 *
 * @param cfg
 */
void init_uart_cfg(struct UARTcfg *cfg);

/**
 * @brief Initialize the UART on the microcontroller
 *
 * @param cfg Pointer to a UARTcfg struct with the configuration for the UART
 *
 * @details
 * Some configuration options are currently fixed. The UART will always be setup
 * in 8N1 mode. This may be changed in the future.
 *
 * This method also initializes a circular buffer. So there is no need to call
 * cb_init() yourself.
 *
 */
void init_UART(const struct UARTcfg *cfg);

#ifdef LIB_DEBUG
void put_noi_UART(char c);
void puts_noi_UART(const char *s);
#endif /* LIB_DEBUG */

/**
 * @brief Send a single character
 *
 * @param c The character to send
 */
void put_UART(const char c);

/**
 * @brief Write a string to the UART buffer
 *
 * @param s The string you want to send
 * @details
 *
 * The macro CR will automatically appended to your string so you do not have to
 * worry about this.
 */
void puts_UART(const char *s);

/**
 * @brief Retrieve one char from the buffer
 *
 * @param s Pointer to a char variable
 *
 * @return 0 If character was retrieved, 1 otherwise
 */
uint8_t get_UART(char *s);
/**
 * @brief Get all data from the circular buffer
 *
 * @param s Pointer an array which this function uses to store the data from cb
 * @return 0 on success or 1 if something went wrong
 *
 * @warning
 * Make sure your char array has enough room for the data in the circular buffer
 * plus `\0` as termination character. You can get the numbers of items in the
 * buffer with DirBuff#items
 */
uint8_t gets_UART(char *s);

#ifdef PRINTF
int puts_printf_UART(char c, FILE *stream);
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wunused-variable"
static FILE uartavr_stdout =
    FDEV_SETUP_STREAM(puts_printf_UART, NULL, _FDEV_SETUP_WRITE);
#pragma GCC diagnostic pop
#endif

#endif /* ifndef UART_H */
