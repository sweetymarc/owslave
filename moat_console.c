/*
 *  Copyright © 2014-2015, Matthias Urlichs <matthias@urlichs.de>
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

/* This code implements reading the console buffer via 1wire.
 */

#include "moat_internal.h"
#include "dev_data.h"
#include "debug.h"
#include "onewire.h"
#include "console.h"

#ifdef N_CONSOLE

static uint8_t sent;
uint8_t read_console_len(uint8_t chan)
{
	uint8_t len;
	len = console_buf_len();
	if(chan == 1) {
		if (len>MAXBUF)
			len=MAXBUF;
	} else if (!chan) {
		if (len) len = 2;
		else len = 0;
	} else
		next_idle('W');
	sent = len;
	return len;
	
}
void read_console(uint8_t chan, uint8_t *buf)
{
	if (chan) {
		if (chan != 1)
			next_idle('W');
		console_buf_read(buf,sent);
	} else { // list of channels and their length
		uint8_t len = console_buf_len();
		buf[0] = 1;
		buf[1] = len;
	}
}

void read_console_done(uint8_t chan)
{
	if (chan)
		console_buf_done(sent);
}

char alert_console_check()
{
	return console_alert();
}

void alert_console_fill(uint8_t *buf)
{
	if(console_alert())
		buf[0]=1;
}

#ifdef CONSOLE_WRITE
void write_console(uint16_t crc)
{
	uint8_t chan;
	uint8_t len, pos=0;
	uint8_t buf[MAXBUF+1];
	chan = recv_byte_in();
	if (chan != 1)
		next_idle('w');
	recv_byte();
	len = recv_byte_in();
	recv_byte();
	if ((len==0) || (len>MAXBUF))
		next_idle('u');
	crc = crc16(crc,chan);
	crc = crc16(crc,len);
	while(1) {
		uint8_t b = recv_byte_in();
		buf[pos++] = b;
		if (pos == len) {
			crc = crc16(crc,b);
			break;
		}
		recv_byte();
		crc = crc16(crc,b);
	}
	end_transmission(crc);
	buf[pos] = 0;
	console_puts((char *)buf);
}
#endif // console_write

#endif
