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
* FILENAME :        printf_example.c
*
* DESCRIPTION :
*       How to use printf with uartavr
*
* NOTES :
*       This is an example for the uartavr library. This shows how to use
*       the printf function with this library. In order to do this the Makefile
*       must link to the math library and printf_min or printf_flt.
*       In order to show some useful data this example activates the internal
*       temperature sensor that some Atmel avrs have. This sensor is not very
*       accurate. You should consider a one or two point calibration and store
*       the calibration data in the EEPROM. The values used in this example worked
*       quite good with an ATmega328P.
*       Have a look at AVR122: Calibration of the AVR's Internal Temperature Reference
*
*       Be careful not to provide an external reference voltage to AREF with this
*       example as the adc is set to use an internal reference (1.1). If you do
*       this they will be shorted to the external voltage.
*
* AUTHOR :    Christian Rapp
*
*H*/
#include <avr/io.h>
#include <stdio.h>

#include <string.h>

#include "uart.h"

volatile uint8_t adc_ready; /* signal main loop adc conversion result is ready */
volatile uint8_t overflow_cnt; /* an additional counter for Timer0 */

int main(void)
{
    /* set up uart with default config */
    struct UARTcfg cfg;
    memset(&cfg, 0, sizeof(struct UARTcfg));

    init_uart_cfg(&cfg);
    init_UART(&cfg);

    adc_ready = 0;

    /* use internal reference (1.1V) and activate adc8 */
    ADMUX |= _BV(REFS1) | _BV(REFS0) | _BV(MUX3);
    /* activate interrupts and 1024 prescaler */
    ADCSRA |= _BV(ADIE) | _BV(ADPS2) | _BV(ADPS1) | _BV(ADPS0);
    /* enable the adc */
    ADCSRA |= _BV(ADEN);

    overflow_cnt = 0;
    TCNT0 = 0;
    /* set prescaler for Timer0 to 1024 */
    TCCR0B |= _BV(CS02) | _BV(CS00);
    /* activate timer overflow interrupt */
    TIMSK0 |= _BV(TOIE0);

    sei();

    while (1) {
        while (!adc_ready) {
        }

        /* read the data from the adc data registers */
        adc_ready = 0;
        uint16_t adc_data = ADCL;
        adc_data |= (ADCH << 8);

        /* Conversion formula: */
        /* T = { [(ADCH << 8) | ADCL] - TOS } / k */
        float result = (adc_data - 328) / 1.22;
        /* send the result via uart */
        printf("We measured: %.2f\n", result);
    }

    return 0;
}

ISR(ADC_vect) { adc_ready = 1; }

ISR(TIMER0_OVF_vect)
{
    /* we are using an additional counter here. With a frequency of 16MHz this
     * will activate the main loop approximately every second */
    ++overflow_cnt;
    if (overflow_cnt == 61) {
        overflow_cnt = 0;
        /* measure the temperature */
        ADCSRA |= _BV(ADSC);
    }
}
