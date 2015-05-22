/*
 * sensor_auth.c
 *
 *  Created on: May 21, 2015
 *      Author: l3nko
 */
#include "sensor_atuh.h"
#include "relic.h"

#define ENABLE_DEBUG    (0)
#include "debug.h"

#define ID_SIZE			(20)
#define HASH_SIZE		(32)
#define CHALLENGE_SIZE	(1)
#define EPHEMERAL_SIZE	(1)

static uint8_t thisID[ID_SIZE];		/* current id */
static uint8_t sensorID[ID_SIZE];	/* destination id (if current is the GW) */

static uint8_t thisChallengeNum[CHALLENGE_SIZE];	/* random number used to challenge (this) */
//static uint8_t otherChallengeNum[CHALLENGE_SIZE];	/* random number used to challenge (other) */

static uint8_t challengeSuccess;	/* 1 if challenge SUCCESS other FAIL */

static uint8_t thisEphemeralKey[EPHEMERAL_SIZE];
static uint8_t otherEphemeralKey[EPHEMERAL_SIZE];

/* PRIVATE FUNCTIONS */
static void generate_ephemeral_key();
static void generate_challenge_number();

static uint8_t send_packet(sa_pkt_t *pkt);
static uint8_t send_packet_Server(sa_pkt_t *pkt);
/* ***************** */

uint8_t get_packet_fromBuffer(uint8_t *buffer, size_t length, sa_pkt_t *pkt)
{
	uint8_t id = buffer[0];

	if(pkt == NULL) {
		DEBUG("sensor_auth: get pkt from buffer to null pointer")
		return -MAX_SA_PKT_LENGTH;
	}

	pkt->id = buffer[0];
	if(pkt->id >= SA_NUM_OF_PKT_TYPES) {
		pkt->id = SA_UNKNOWN_TYPE;
		pkt->size = 0;
	}
	else {
		pkt->size = length-1 < MAX_SA_PKT_LENGTH ? length-1 : MAX_SA_PKT_LENGTH;	//min(length-1, MAX_SA_PKT_LENGTH);
		memcpy(pkt->data, &buffer[1], pkt->size);
	}

	return pkt->size - (length-1);
}

static uint8_t send_packet(sa_pkt_t *pkt)
{
	return 0;
}

static uint8_t send_packet_Server(sa_pkt_t *pkt)
{
	return 0;
}

uint8_t start_authentication()
{
	sa_pkt_t req;
	req.id = SA_HELLO;
	req.size = 0;

	challengeSuccess = 0;	//reset challenge value
	return send_packet(&req);
}

uint8_t sa_manager(sa_pkt_t *pkt) {
	DEBUG("sensor_auth: packet received with type: %0x02X", pkt->id);
	if(pkt->id == SA_HELLO) {
		sa_pkt_t resp;
		resp.id = SA_HELLO_RESP;
		resp.size = 0;
		send_packet(&resp);
		challengeSuccess = false;	//reset challenge value
	}
	else if(pkt->id == SA_HELLO_RESP){
		sa_pkt_t req;
		req.id = SA_HASH_ID;
		req.size = HASH_SIZE; /* SHA2 - 256 bit -> 32 byte */
		md_map_sh256(req.data, thisID, ID_SIZE);	/* calculate SHA-256 hash */
		send_packet(&req);
	}
	else if(pkt->id == SA_HASH_ID) {
		sa_pkt_t req;
		req.id = SA_GET_ID;
		req.size = 0;
		send_packet_Server(&req);	//OSS: send packet to OMA-DM server
	}
	else if (pkt->id == SA_RESP_ID) {
		memcpy(sensorID, pkt->data, ID_SIZE);	/* store other ID: received from OMA-DM server */
		generate_challenge_number();
		/* start challenge */
		sa_pkt_t req;
		req.id = SA_CHALLENGE_ID;
		req.size = HASH_SIZE + 1;	/* SHA-256 + 1 byte random */

		//compute hash with ID || challenge num
		uint8_t hashInput[ID_SIZE+CHALLENGE_SIZE];
		memcpy(hashInput, sensorID, ID_SIZE);
		memcpy(&hashInput[ID_SIZE], thisChallengeNum, CHALLENGE_SIZE);
		md_map_sh256(req.data, hashInput, ID_SIZE+1);
		//concatenate challenge
		memcpy(&req.data[HASH_SIZE], thisChallengeNum, CHALLENGE_SIZE);
		send_packet(&req);
	}
	else if(pkt->id == SA_CHALLENGE_ID) {
		/* check HASH with this ID and challenge number received*/
		uint8_t hashCalc[HASH_SIZE];
		uint8_t hashInput[ID_SIZE+CHALLENGE_SIZE];
		memcpy(hashInput, thisID, ID_SIZE);
		memcpy(&hashInput[ID_SIZE], &pkt->data[HASH_SIZE], CHALLENGE_SIZE);	//challenge number received
		md_map_sh256(hashCalc, hashInput, ID_SIZE+CHALLENGE_SIZE);

		sa_pkt_t resp;
		resp.id = SA_RESP_CHALLENGE;
		resp.size = 1;
		if(memcmp(hashCalc, pkt->data, HASH_SIZE) == 0) {
			DEBUG("sensor_auth: ID challenge SUCCESS")
			challengeSuccess = true;
		}
		else {
			DEBUG("sensor_auth: ID challenge FAIL !!!")
			challengeSuccess = false;
		}
		resp.data[0] = challengeSuccess;
		send_packet(&resp);
	}
	else if(pkt->id == SA_RESP_CHALLENGE) {
		challengeSuccess = pkt->data[0];
		if(challengeSuccess == true) {	/* challenge success */
			generate_ephemeral_key();
			//send generated ephemeral key
			sa_pkt_t req;
			req.id = SA_GW_KEY;
			req.size = EPHEMERAL_SIZE;
			memcpy(req.data, thisEphemeralKey, EPHEMERAL_SIZE);
			send_packet(&req);
		}
		else {
			DEBUG("sensor_auth: challenge response NO !!");
		}
	}
	else if(pkt->id == SA_GW_KEY) {
		memcpy(otherEphemeralKey, pkt->data, EPHEMERAL_SIZE);	/* store other ephemeral key */
		//send generated ephemeral key
		generate_ephemeral_key();
		sa_pkt_t req;
		req.id = SA_GW_KEY;
		req.size = EPHEMERAL_SIZE;
		memcpy(req.data, thisEphemeralKey, EPHEMERAL_SIZE);
		send_packet(&req);
	}
	else {
		/* unknown type */
		DEBUG("sensor_auth: packet type unknown: %0x02X", pkt->id);
		return 1;
	}
	return 0;
}

static void generate_ephemeral_key() {
	//thisEphemeralKey = //TODO: random buffer!!!!!!
	memset(thisEphemeralKey, 0, EPHEMERAL_SIZE);
}

static void generate_challenge_number() {
	//thisEphemeralKey = //TODO: random buffer!!!!!!
	memset(thisChallengeNum, 0, CHALLENGE_SIZE);
}
