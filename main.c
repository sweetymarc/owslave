/*
 *  Copyright © 2010-2015, Matthias Urlichs <matthias@urlichs.de>
 *
 *  This program is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation, either version 3 of the License, or
 *  (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License (included; see the file LICENSE)
 *  for more details.
 */

/* Based on work published at http://www.mikrocontroller.net/topic/44100 */

#include <avr/io.h>
#include <avr/interrupt.h>

#define MAIN
#include "features.h"
#include "onewire.h"
#include "uart.h"
#include "dev_data.h"
#include "debug.h"
#include "moat.h"
#include "jmp.h"

// Initialise the hardware
static inline void
init_mcu(void)
{
#ifdef __AVR_ATtiny13__
       CLKPR = 0x80;    // Prepare to ...
       CLKPR = 0x00;    // ... set to 9.6 MHz

#elif defined(__AVR_ATtiny25__)
       CLKPR = 0x80;    // Prepare to ...
       CLKPR = 0x00;    // ... set to 8.0 MHz

#elif defined(__AVR_ATtiny84__)
       CLKPR = 0x80;    // Prepare to ...
       CLKPR = 0x00;    // ... set to 8.0 MHz

#elif defined (__AVR_ATmega8__)
       // Clock is set via fuse
#elif defined (__AVR_ATmega168__) || defined (__AVR_ATmega88__)
       // Clock is set via fuse

#else
#error Basic config for your CPU undefined
#endif
}
 
static inline void
init_all(void)
{
	uart_init(UART_BAUD_SELECT(BAUDRATE,F_CPU));
	onewire_init();
	init_state();
}

inline void
poll_all(void)
{
    uart_poll();
    onewire_poll();
}

// Main program
int
main(void)
{

#ifdef HAVE_DBG_PIN
        DBGPINPORT &= ~(1 << DBGPIN);
        DBGPINDDR |= (1 << DBGPIN);
#endif
#ifdef HAVE_DBG_PORT
	DBGPORT = 0;
	DBGDDR = 0xFF;
#endif
#ifdef HAVE_ONEWIRE
	OWDDR &= ~(1<<ONEWIREPIN);
	OWPORT &= ~(1<<ONEWIREPIN);
#endif

	init_mcu();
	init_all();

	// now go
        DBG(0x01);
	sei();
        DBG(0x03);

	DBGS_P("\nInit done!\n");
        DBG(0x02);
        setjmp_q(_go_out);
        DBG(0x22);
        /* clobbered variables (and constants) beyond this point */
	while(1) {
            poll_all();
            mainloop();
	}
}

