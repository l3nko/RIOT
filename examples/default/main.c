/*
 * Copyright (C) 2008, 2009, 2010  Kaspar Schleiser <kaspar@schleiser.de>
 * Copyright (C) 2013 INRIA
 * Copyright (C) 2013 Ludwig Ortmann <ludwig.ortmann@fu-berlin.de>
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
 * @brief       Default application that shows a lot of functionality of RIOT
 *
 * @author      Kaspar Schleiser <kaspar@schleiser.de>
 * @author      Oliver Hahm <oliver.hahm@inria.fr>
 * @author      Ludwig Ortmann <ludwig.ortmann@fu-berlin.de>
 *
 * @}
 */

#include <stdio.h>
#include <string.h>

#include "thread.h"
#include "posix_io.h"
#include "shell.h"
#include "shell_commands.h"
#include "board_uart0.h"

//#define USE_RC5     //DEBUG!!!!
#include "crypto/rc5.h"
#include "uECC.h"

#if FEATURE_PERIPH_RTC
#include "periph/rtc.h"
#endif

#ifdef MODULE_LTC4150
#include "ltc4150.h"
#endif

#if MODULE_AT86RF231 || MODULE_CC2420 || MODULE_MC1322X
#include "ieee802154_frame.h"
#endif

#ifdef MODULE_TRANSCEIVER
#include "transceiver.h"
#endif

#define SND_BUFFER_SIZE     (100)
#define RCV_BUFFER_SIZE     (64)
#define RADIO_STACK_SIZE    (KERNEL_CONF_STACKSIZE_MAIN)

#ifdef MODULE_TRANSCEIVER

static char radio_stack_buffer[RADIO_STACK_SIZE];
static msg_t msg_q[RCV_BUFFER_SIZE];

void *radio(void *arg)
{
    (void) arg;

    msg_t m;

#if MODULE_AT86RF231 || MODULE_CC2420 || MODULE_MC1322X
    ieee802154_packet_t *p;
#else
    radio_packet_t *p;
    radio_packet_length_t i;
#endif

    msg_init_queue(msg_q, RCV_BUFFER_SIZE);

    #ifdef USE_RC5
    //init cipher
    uint8_t key[16] = {0,1,2,3,4,5,6,7,8,9,10,11,12,13,14,15};
    uint8_t dec_msg[255];
    cipher_context_t context;
    block_cipher_interface_t cipher = rc5_interface;
    printf("Initializing cipher: %s whit size %d\n", cipher.name, sizeof(context.context));
    int enc_res = cipher.BlockCipher_init(&context, cipher.BlockCipherInfo_getPreferredBlockSize(), 16, key);
    printf("RC5_INIT: %d\n", enc_res);
    #endif

    while (1) {
        msg_receive(&m);

        if (m.type == PKT_PENDING) {


#if MODULE_AT86RF231 || MODULE_CC2420 || MODULE_MC1322X
            p = (ieee802154_packet_t*) m.content.ptr;
            printf("Got radio packet:\n");
            printf("\tLength:\t%u\n", p->length);
            printf("\tSrc:\t%u\n", (p->frame.src_addr[0])|(p->frame.src_addr[1]<<8));
            printf("\tDst:\t%u\n", (p->frame.dest_addr[0])|(p->frame.dest_addr[1]<<8));
            printf("\tLQI:\t%u\n", p->lqi);
            printf("\tRSSI:\t%u\n", p->rssi);

            printf("Payload Length:%u\n", p->frame.payload_len);
            printf("Payload:%s\n", p->frame.payload);

            p->processing--;
#else
            p = (radio_packet_t *) m.content.ptr;

            printf("Got radio packet:\n");
            printf("\tLength:\t%u\n", p->length);
            printf("\tSrc:\t%u\n", p->src);
            printf("\tDst:\t%u\n", p->dst);
            printf("\tLQI:\t%u\n", p->lqi);
            printf("\tRSSI:\t%u\n", p->rssi);

            #ifdef USE_RC5
            printf("Decrypting...");
            enc_res = cipher.BlockCipher_decrypt( &context, p->data, dec_msg);
            printf("%d\n", enc_res);
            if(enc_res>0) 
            {
                p->length = strlen((char*)dec_msg);
                memcpy(p->data, dec_msg, p->length);   

                //printf("%s\n", dec_msg);
            }
            #endif

            for (i = 0; i < p->length; i++) {
                printf("%02X ", p->data[i]);
            }

            p->processing--;
            puts("\n");
#endif

        }
        else if (m.type == ENOBUFFER) {
            puts("Transceiver buffer full");
        }
        else {
            puts("Unknown message received");
        }
    }
}

void init_transceiver(void)
{
    kernel_pid_t radio_pid = thread_create(
                        radio_stack_buffer,
                        sizeof(radio_stack_buffer),
                        PRIORITY_MAIN - 2,
                        CREATE_STACKTEST,
                        radio,
                        NULL,
                        "radio");

    uint16_t transceivers = TRANSCEIVER_DEFAULT;

    transceiver_init(transceivers);
    (void) transceiver_start();
    transceiver_register(transceivers, radio_pid);
}
#endif /* MODULE_TRANSCEIVER */

static int shell_readc(void)
{
    char c = 0;
    (void) posix_read(uart0_handler_pid, &c, 1);
    return c;
}

static void shell_putchar(int c)
{
    (void) putchar(c);
}

int main(void)
{
    shell_t shell;
    (void) posix_open(uart0_handler_pid, 0);

#ifdef MODULE_LTC4150
    ltc4150_start();
#endif

#ifdef MODULE_TRANSCEIVER
    init_transceiver();
#endif

#ifdef FEATURE_PERIPH_RTC
    rtc_init();
#endif

    (void) puts("Welcome to RIOT!");

    shell_init(&shell, NULL, UART0_BUFSIZE, shell_readc, shell_putchar);

    //init uECC
	random_init();	//need for uECC
	printf("Initializing uECC...");

	//public-private key
#define CPU0
#ifdef CPU0
	uint8_t privateKey[uECC_BYTES] = {0x05, 0xB8, 0x30, 0x6A, 0x86, 0xEE, 0x26, 0x48, 0xEC, 0x63, 0x12, 0x9C, 0xFC, 0xBD, 0xF3, 0x42, 0x10, 0xF2, 0x6C, 0x12};
	uint8_t publicKey[uECC_BYTES+1] = {0x03, 0x41, 0x9A, 0xC3, 0xED, 0xD4, 0xB6, 0x51, 0x75, 0xA1, 0xE3, 0x59, 0xA7, 0xF4, 0x31, 0xF6, 0x11, 0x89, 0xC2, 0x22, 0xC2};
#else
	uint8_t privateKey[uECC_BYTES] = {0xED, 0x64, 0xC4, 0x32, 0x36, 0x64, 0xE5, 0xD6, 0xBD, 0xBF, 0xB6, 0x25, 0x5F, 0xA1, 0xEA, 0x4D, 0x4D, 0xFB, 0x33, 0x3E};
	uint8_t publicKey[uECC_BYTES+1] = {0x03, 0x97, 0xAC, 0x7B, 0x03, 0x1A, 0x88, 0x76, 0x8D, 0x1C, 0x07, 0x62, 0xF7, 0xEC, 0x29, 0x92, 0x45, 0x17, 0x1B, 0x01, 0xB5};
#endif
	uint8_t decPubliKey[uECC_BYTES*2];
	uECC_decompress(publicKey, decPubliKey);

	//int resECC0 = uECC_make_key(decPubliKey, privateKey);
	//uECC_compress(decPubliKey, publicKey);

	//sahred key
	uint8_t sharedKey[uECC_BYTES];
	int resECC = uECC_shared_secret(decPubliKey, privateKey, sharedKey);

	printf("%d\n", resECC);
	if(resECC>0)
	{
		printf("My private key is:");
		for(int i=0; i<uECC_BYTES; i++)
			printf(" 0x%02X,", privateKey[i]);
		printf("\n");

		printf("My COMPRESSED-public key is:");
		for(int i=0; i<uECC_BYTES+1; i++)
			printf(" 0x%02X,", publicKey[i]);
		printf("\n");

		printf("My shared key is:");
		for(int i=0; i<uECC_BYTES; i++)
			printf(" 0x%02X,", sharedKey[i]);
		printf("\n");
	}

    shell_run(&shell);
    return 0;
}
