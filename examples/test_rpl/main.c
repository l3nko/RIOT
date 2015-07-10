/*
 * Copyright (C) 2014 Freie Universität Berlin
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     examples
 * @{
 *
 * @file
 * @brief       Hello World application
 *
 * @author      Kaspar Schleiser <kaspar@schleiser.de>
 * @author      Ludwig Ortmann <ludwig.ortmann@fu-berlin.de>
 *
 * @}
 */

#include <stdio.h>
#include <string.h>

#include "posix_io.h"
#include "shell.h"
// #include "shell_commands.h"
#include "board_uart0.h"

//inlcudes for transceiver
#include "thread.h"
// #include "transceiver.h"

//includes for RPL
//#include "net_if.h"
//#include "sixlowpan.h"
// #include "rpl.h"
//#include "rpl/rpl_dodag.h"

// ps command
#include "ps.h"


#define ENABLE_DEBUG    (1)
#include "debug.h"

// #ifndef UNASSIGNED_CHANNEL
// 	#define UNASSIGNED_CHANNEL INT_MIN
// #endif

// #define TRANSCEIVER TRANSCEIVER_DEFAULT
// #define RADIO_CHANNEL   (10)

// #define MONITOR_STACK_SIZE  (THREAD_STACKSIZE_MAIN) //or THREAD_STACKSIZE_DEFAULT
// #define RCV_BUFFER_SIZE     (16)//(32)

 /** @brief Char array for IP address printing */
//static char addr_str[IPV6_MAX_ADDR_STR_LEN];

// static msg_t msg_q[RCV_BUFFER_SIZE];
// void *rpl_udp_monitor(void *arg)
// {
// 	DEBUGF("rpl_udp_monitor starting...\n");

//     (void) arg;

//     msg_t m;
//     radio_packet_t *p;

//     msg_init_queue(msg_q, RCV_BUFFER_SIZE);
//     DEBUGF("msg queue initialized\n");

//     while (1) {
//     	DEBUGF("rpl_udp_monitor loop 0\n");
//         msg_receive(&m);

//         if (m.type == PKT_PENDING) {
//             p = (radio_packet_t *) m.content.ptr;

//             DEBUGF("Received packet from ID %u\n", p->src);
//             DEBUG("\tLength:\t%u\n", p->length);
//             DEBUG("\tSrc:\t%u\n", p->src);
//             DEBUG("\tDst:\t%u\n", p->dst);
//             DEBUG("\tLQI:\t%u\n", p->lqi);
//             DEBUG("\tRSSI:\t%i\n", (int8_t) p->rssi);

//             for (uint8_t i = 0; i < p->length; i++) {
//                 DEBUG("%02X ", p->data[i]);
//             }

//             p->processing--;
//             DEBUG("\n");
//         }
//          else if (m.type == IPV6_PACKET_RECEIVED) {
// 			uint8_t icmp_type, icmp_code;
//     		ipv6_hdr_t *ipv6_buf;
//     		icmpv6_hdr_t *icmpv6_buf = NULL;

//             ipv6_buf = (ipv6_hdr_t *) m.content.ptr;
//             printf("IPv6 datagram received (next header: %02X)", ipv6_buf->nextheader);
//             printf(" from %s ", ipv6_addr_to_str(addr_str, IPV6_MAX_ADDR_STR_LEN,
//                                                  &ipv6_buf->srcaddr));

//             if (ipv6_buf->nextheader == IPV6_PROTO_NUM_ICMPV6) {
//                 icmpv6_buf = (icmpv6_hdr_t *) &ipv6_buf[(LL_HDR_LEN + IPV6_HDR_LEN) + ipv6_ext_hdr_len];
//                 icmp_type = icmpv6_buf->type;
//                 icmp_code = icmpv6_buf->code;
//             }

//             if (ipv6_buf->nextheader == IPV6_PROTO_NUM_ICMPV6) {
//                 DEBUG("\t ICMP type: %02X ", icmp_type);
//                 DEBUG("\t ICMP code: %02X ", icmp_code);
//                 (void) icmp_type;
//                 (void) icmp_code;
//             }

//             printf("\n");
//         } 
//         else if (m.type == ENOBUFFER) {
//             puts("Transceiver buffer full");
//         }
//         else {
//             printf("Unknown packet received, type %04X\n", m.type);
//         }
//     }
// }

// static char monitor_stack_buffer[MONITOR_STACK_SIZE];	//stack buffer of monitor thread
void init_transceiver(void)
{
    // DEBUGF("Creating thread monitor\n");
    // kernel_pid_t monitor_pid = thread_create(monitor_stack_buffer,
    //                                          sizeof(monitor_stack_buffer),
    //                                          THREAD_PRIORITY_MAIN - 2,
    //                                          CREATE_SLEEPING, //CREATE_STACKTEST,
    //                                          rpl_udp_monitor,
    //                                          NULL,
    //                                          "rpl-monitor");

    // DEBUGF("Register at transceiver %02X\n", TRANSCEIVER);
    // transceiver_register(TRANSCEIVER, monitor_pid);

    // DEBUGF("Start monitor PID: %" PRIkernel_pid "\n", monitor_pid);
    // thread_wakeup(monitor_pid);

    // ipv6_register_packet_handler(monitor_pid);
}


// static radio_address_t id;
int myShellcommand(int argc, char **argv)
{
	(void) argc;
    (void) argv;

	// printf("Initializing RPL...\n");

	// transceiver_command_t tcmd;
 //    msg_t m;
 //    int32_t chan = UNASSIGNED_CHANNEL;

 //    //set address
 //    //net_if_set_hardware_address(0, id);

 //    // get channel
 //    DEBUGF("getting channel...\n");
 //    tcmd.transceivers = TRANSCEIVER;
 //    tcmd.data = &chan;
 //    m.type = GET_CHANNEL;
 //    m.content.ptr = (void *) &tcmd;
 //    msg_send_receive(&m, &m, transceiver_pid);
 //    if( chan == UNASSIGNED_CHANNEL ) {
 //        DEBUGF("The channel has not been set yet.\n");
 //        /* try to set the channel to 10 (RADIO_CHANNEL) */
 //        chan = RADIO_CHANNEL;
 //    }

 //    //set channel
 //    DEBUGF("setting channel...\n");
 //    m.type = SET_CHANNEL;
 //    msg_send_receive(&m, &m, transceiver_pid);
 //    if( chan == UNASSIGNED_CHANNEL ) {
 //        puts("ERROR: channel NOT set! Aborting initialization.");
 //        return 1;
 //    }
 //    printf("Channel set to %" PRIi32 "\n", chan);


    /* global address */
    /*
    ipv6_addr_t global_addr, global_prefix;
    ipv6_addr_init(&global_prefix, 0xabcd, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0);
    ipv6_addr_set_by_eui64(&global_addr, 0, &global_prefix);

    puts("6LoWPAN initialized.");
    */

    // init_transceiver();

    printf("RPL initialized !!!\n");
	return 0;
}

int my_ps_handler(int argc, char **argv)
{
    (void) argc; (void) argv;

    ps();
    return 0;
}

int my_reboot_handler(int argc, char **argv)
{
    (void) argc;
    (void) argv;

    (void) reboot(RB_AUTOBOOT);

    return 0;
}

static const shell_command_t myshell_commands[] = {
    // {"cmd", "my shell command", myShellcommand},
    {"ps", "Prints information about running threads", my_ps_handler},
    {"reboot", "Reboot the node", my_reboot_handler},
    {NULL, NULL, NULL}
};

int main(void)
{
	shell_t shell;
    posix_open(uart0_handler_pid, 0);

    //net_if_set_src_address_mode(0, NET_IF_TRANS_ADDR_M_SHORT);
    //id = net_if_get_hardware_address(0);

    // printf("Welcome RIOT!\n");
    // printf("You are running RIOT on a(n) %s board.\n", RIOT_BOARD);
    // printf("This board features a(n) %s MCU.\n", RIOT_MCU);

    shell_init(&shell, myshell_commands, UART0_BUFSIZE, uart0_readc, uart0_putc);
    shell_run(&shell);

    return 0;
}
