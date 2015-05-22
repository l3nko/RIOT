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

int sa_init_handler(int argc, char **argv)
{
    if (argc < 2) {
        puts("Usage: sa_init DST");
        return 1;
    }
    
    if (transceiver_pid == KERNEL_PID_UNDEF) {
        puts("Transceiver not initialized");
        return 1;
    }
    
    radio_packet_t p;
    transceiver_command_t tcmd;
    tcmd.transceivers = _TC_TYPE;
    tcmd.data = &p;
    
    uint8_t msg_payload = 0x00;     //SA_HELLO = 0
    
    p.data = &msg_payload;
    p.length = 1;
    p.dst = atoi(argv[1]);  //DEST arg
    
    msg_t mesg;
    mesg.type = SND_PKT;
    mesg.content.ptr = (char *) &tcmd;
    
    uint8_t resp = (uint8_t)msg_send(&mesg, transceiver_pid);
    printf("sc_sensor auth: packet sent %d\n", resp);
    
    return 0;
}
