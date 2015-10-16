///*
// * sensor_auth.c
// *
// *  Created on: May 21, 2015
// *      Author: l3nko
// */
//#include "sensor_atuh.h"
//#include "relic.h"
//
//#include "transceiver.h"
//#include "msg.h"
//
//#include "config.h"
//
//#include "periph/random.h"
//
//#define _TC_TYPE            TRANSCEIVER_NATIVE
//
//#define ENABLE_DEBUG    (1)
//#include "debug.h"
//
//#define ID_SIZE			(20)
//#define HASH_SIZE		(32)
//#define CHALLENGE_SIZE	(2)
//#define EPHEMERAL_SIZE	(1)
//
//static uint8_t thisID[ID_SIZE];		/* current id */
//static uint8_t sensorID[ID_SIZE];	/* destination id (if current is the GW) */
//
////static uint8_t thisChallengeNum[CHALLENGE_SIZE];	/* random number used to challenge (this) */
//static uint8_t challengeNumber[CHALLENGE_SIZE];	/* random number used to challenge (this) */
////static uint8_t otherChallengeNum[CHALLENGE_SIZE];	/* random number used to challenge (other) */
//
//static uint8_t challengeSuccess;	/* 1 if challenge SUCCESS other FAIL */
//
//static uint8_t thisEphemeralKey[EPHEMERAL_SIZE];
//static uint8_t otherEphemeralKey[EPHEMERAL_SIZE];
//
///* PRIVATE FUNCTIONS */
//static void generate_ephemeral_key(void);
//static void generate_challenge_number(void);
//
//static uint8_t send_packet(sa_pkt_t *pkt, uint16_t dst);
//static uint8_t send_packet_Server(sa_pkt_t *pkt);
///* ***************** */
//
//int8_t get_packet_fromBuffer(uint8_t *buffer, size_t length, sa_pkt_t *pkt)
//{
//	if(pkt == NULL) {
//        DEBUG("sensor_auth: get pkt from buffer to null pointer\n");
//		return -MAX_SA_PKT_LENGTH;
//	}
//
//	pkt->id = buffer[0];
//	if(pkt->id >= SA_NUM_OF_PKT_TYPES) {
//		pkt->id = SA_UNKNOWN_TYPE;
//		pkt->size = 0;
//	}
//	else {
//		pkt->size = length-1 < MAX_SA_PKT_LENGTH ? length-1 : MAX_SA_PKT_LENGTH;	//min(length-1, MAX_SA_PKT_LENGTH);
//		memcpy(pkt->data, &buffer[1], pkt->size);
//	}
//
//	return pkt->size - (length-1);
//}
//
//static uint8_t send_packet(sa_pkt_t *pkt, uint16_t dst)
//{
//    DEBUG("sensor_auth: sending pkt with id 0x%02X to %u\n", pkt->id, dst);
//
//    if (transceiver_pid == KERNEL_PID_UNDEF) {
//        puts("Transceiver not initialized");
//        return -pkt->size;
//    }
//
//    radio_packet_t p;
//    transceiver_command_t tcmd;
//    tcmd.transceivers = _TC_TYPE;
//    tcmd.data = &p;
//
//    // copy sa pkt id + data
//    uint8_t data[MAX_SA_PKT_LENGTH];
//    data[0] = pkt->id;
//
//    memcpy(&data[1], pkt->data, pkt->size);
//
//    p.data = data;
//    p.length = pkt->size+1;
//    p.dst = dst;
//
//    msg_t mesg;
//    mesg.type = SND_PKT;
//    mesg.content.ptr = (char *) &tcmd;
//
//    uint8_t resp = (uint8_t)msg_send(&mesg, transceiver_pid);
//    if(resp == 1)
//        DEBUG("sc_sensor auth: packet sent\n");
//    else
//        printf("sc_sensor auth: error packet not sent %d\n", resp);
//
//
//	return resp;
//}
//
//static uint8_t send_packet_Server(sa_pkt_t *pkt)
//{
//    DEBUG("sensor_auth: sending pkt to SERVER with id 0x%02X and HASH:\n", pkt->id);
//    (void)pkt;
//
//	#if ENABLE_DEBUG
//	for(int i=0; i<HASH_SIZE; i++)
//		DEBUG("0x%02X ", pkt->data[i]);
//	DEBUG("\n");
//	#endif
//
//	return 0;
//}
//
////uint8_t start_authentication()
////{
////	sa_pkt_t req;
////	req.id = SA_HELLO;
////	req.size = 0;
////
////	challengeSuccess = 0;	//reset challenge value
////	return send_packet(&req, 0);
////}
//
//uint8_t sa_manager(sa_pkt_t *pkt, uint16_t src) {
//	//DEBUG("sensor_auth: packet received with type 0x%02X and size %lu\n", pkt->id, pkt->size);
//	if(pkt->id == SA_HELLO) {
//		sa_pkt_t resp;
//		resp.id = SA_HELLO_RESP;
//		resp.size = 0;
//		send_packet(&resp, src);
//		challengeSuccess = false;	//reset challenge value
//	}
//	else if(pkt->id == SA_HELLO_RESP){
//		sa_pkt_t req;
//		req.id = SA_HASH_ID;
//		req.size = HASH_SIZE; /* SHA2 - 256 bit -> 32 byte */
//
//		memcpy(thisID, &sysconfig.id, 2);
//		memset(&thisID[2], 0x00, 18);
//
//		md_map_sh256(req.data, thisID, ID_SIZE);	/* calculate SHA-256 hash */
//		send_packet(&req, src);
//	}
//	else if(pkt->id == SA_HASH_ID) {
//		sa_pkt_t req;
//		req.id = SA_GET_ID;
//		req.size = HASH_SIZE;
//		memcpy(req.data, pkt->data, HASH_SIZE);
//		send_packet_Server(&req);	//OSS: send packet to OMA-DM server
//	}
//	else if (pkt->id == SA_RESP_ID) {
//		memcpy(sensorID, pkt->data, ID_SIZE);	/* store other ID: received from OMA-DM server */
//
//		sa_pkt_t req;
//		req.id = SA_CHALLENGE_ID;
//		req.size = HASH_SIZE + CHALLENGE_SIZE;	/* SHA-256 + CHALLENGE_SIZE bytes random */
//
//		/* start challenge */
//		generate_challenge_number();
//
//		//compute hash with ID || challenge num
//		uint8_t hashInput[ID_SIZE+CHALLENGE_SIZE];
//		memcpy(hashInput, sensorID, ID_SIZE);
//		memcpy(&hashInput[ID_SIZE], challengeNumber, CHALLENGE_SIZE);
//
//		md_map_sh256(req.data, hashInput, ID_SIZE+CHALLENGE_SIZE);
//		//concatenate challenge
//		memcpy(&req.data[HASH_SIZE], challengeNumber, CHALLENGE_SIZE);
//
//		send_packet(&req, src);
//	}
//	else if(pkt->id == SA_CHALLENGE_ID) {
//		/* check HASH with this ID and challenge number received*/
//		uint8_t hashCalc[HASH_SIZE];
//		uint8_t hashInput[ID_SIZE+CHALLENGE_SIZE];
//		memcpy(hashInput, thisID, ID_SIZE);
//		memcpy(&hashInput[ID_SIZE], &pkt->data[HASH_SIZE], CHALLENGE_SIZE);	//challenge number received
//
//		md_map_sh256(hashCalc, hashInput, ID_SIZE+CHALLENGE_SIZE);
//
//		sa_pkt_t resp;
//		resp.id = SA_RESP_CHALLENGE;
//		resp.size = 1;
//		if(memcmp(hashCalc, pkt->data, HASH_SIZE) == 0) {
//            DEBUG("sensor_auth: ID challenge SUCCESS\n");
//			challengeSuccess = 1;
//		}
//		else {
//            DEBUG("sensor_auth: ID challenge FAIL !!!\n");
//			challengeSuccess = 0;
//		}
//		resp.data[0] = challengeSuccess;
//		send_packet(&resp, src);
//	}
//	else if(pkt->id == SA_RESP_CHALLENGE) {
//		challengeSuccess = pkt->data[0];
//		if(challengeSuccess == 1) {	/* challenge success */
//			DEBUG("sensor_auth: received challenge SUCCESS\n");
//			generate_ephemeral_key();
//			//send generated ephemeral key
//			sa_pkt_t req;
//			req.id = SA_GW_KEY;
//			req.size = EPHEMERAL_SIZE;
//			memcpy(req.data, thisEphemeralKey, EPHEMERAL_SIZE);
//			send_packet(&req, src);
//		}
//		else {
//			DEBUG("sensor_auth: challenge response NO !!\n");
//		}
//	}
//	else if(pkt->id == SA_GW_KEY) {
//		memcpy(otherEphemeralKey, pkt->data, EPHEMERAL_SIZE);	/* store other ephemeral key */
//		//send generated ephemeral key
//		generate_ephemeral_key();
//		sa_pkt_t req;
//		req.id = SA_SENSOR_KEY;
//		req.size = EPHEMERAL_SIZE;
//		memcpy(req.data, thisEphemeralKey, EPHEMERAL_SIZE);
//		send_packet(&req, src);
//	}
//	else if(pkt->id == SA_SENSOR_KEY) {
//		DEBUG("sensor_auth: sensor authentication COMPLETED !!!\n");
//	}
//	else {
//		/* unknown type */
//		DEBUG("sensor_auth: packet type unknown: %0x02X\n", pkt->id);
//		return 1;
//	}
//	return 0;
//}
//
//static void generate_ephemeral_key(void) {
//	//memset(thisEphemeralKey, 0, EPHEMERAL_SIZE);
//	DEBUG("sensor_auth: generating ephemeral key... ");
//
//	/* power on rng */
//	random_poweron();
//	random_read((char*)thisEphemeralKey, EPHEMERAL_SIZE);
//	random_poweroff();
//
//	#if ENABLE_DEBUG
//	for(int i=0; i<EPHEMERAL_SIZE; i++)
//		DEBUG("0x%02X ", thisEphemeralKey[i]);
//	DEBUG("\n");
//	#endif
//}
//
//static void generate_challenge_number(void) {
//	//memset(thisChallengeNum, 0, CHALLENGE_SIZE);
//	DEBUG("sensor_auth: generating challenge number... ");
//
//	/* power on rng */
//	random_poweron();
//	random_read((char*)challengeNumber, CHALLENGE_SIZE);
//	random_poweroff();
//
//	#if ENABLE_DEBUG
//	for(int i=0; i<CHALLENGE_SIZE; i++)
//		DEBUG("0x%02X ", challengeNumber[i]);
//	DEBUG("\n");
//	#endif
//}
