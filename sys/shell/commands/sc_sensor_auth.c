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

#include "config.h"

config_t sysconfig;

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
    if (argc < 3) {
        puts("Usage: sa_init DST ID");
        return 1;
    }
    
    sysconfig.id = atoi(argv[2]);

    uint8_t data = 0x00;	//SA_HELLO msg ID
    return send(&data, 1, atoi(argv[1]));
}

int sa_server_handler(int argc, char **argv)
{
    if (argc < 3) {
        puts("Usage: sa_server DST ID");
        return 1;
    }
	puts("sc_sensor auth: simulating server response");

//#define ID_SIZE			(20)
    uint8_t data[21];
    data[0] = 4;	//SA_RESP_ID

    uint16_t id = atoi(argv[2]);
    memcpy(&data[1], &id, 2);
    memset(&data[3], 0x00, 18);

    //memset(&data[1], 0, 20);
    return send(data, 21, atoi(argv[1]));

//    radio_packet_t p;
//    p.data = data;
//    p.length = 21;
//
//	transceiver_command_t tcmd;
//	tcmd.transceivers = _TC_TYPE;
//	tcmd.data = &p;
//
//	msg_t mesg;
//	mesg.type = SND_PKT;
//	mesg.content.ptr = (char *) &tcmd;
//
//	int resp = msg_send_to_self(&mesg);
//	printf("sc_sensor auth: send msg to self %d\n", resp);
//
//    return 0;
}
