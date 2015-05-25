/*
 * sensor_auth.c
 *
 *  Created on: May 22, 2015
 *      Author: l3nko
 */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "transceiver.h"
#include "msg.h"
//#include "sensor_atuh.h"

#define _TC_TYPE            TRANSCEIVER_NATIVE

int send(uint8_t *buffer, radio_packet_length_t lenght, uint16_t dst)
{
	if (transceiver_pid == KERNEL_PID_UNDEF) {
		puts("Transceiver not initialized");
		return 1;
	}

	radio_packet_t p;
	transceiver_command_t tcmd;
	tcmd.transceivers = _TC_TYPE;
	tcmd.data = &p;

	p.data = buffer;
	p.length = lenght;
	p.dst = dst;

	msg_t mesg;
	mesg.type = SND_PKT;
	mesg.content.ptr = (char *) &tcmd;

	uint8_t resp = (uint8_t)msg_send(&mesg, transceiver_pid);
	printf("sc_sensor auth: packet sent %d\n", resp);

	return 0;
}

int sa_init_handler(int argc, char **argv)
{
    if (argc < 2) {
        puts("Usage: sa_init DST");
        return 1;
    }
    
    uint8_t data = 0x00;	//SA_HELLO msg ID
    return send(&data, 1, atoi(argv[1]));
}

int sa_server_handler(int argc, char **argv)
{
    if (argc < 2) {
        puts("Usage: sa_server DST");
        return 1;
    }

//#define ID_SIZE			(20)
    uint8_t data[21];
    data[0] = 4;	//SA_RESP_ID
    memset(&data[1], 0, 20);
    return send(data, 21, atoi(argv[1]));
}
