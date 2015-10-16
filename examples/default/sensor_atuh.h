///*
// * sensor_atuh.h
// *
// *  Created on: May 21, 2015
// *      Author: l3nko
// */
//
//#ifndef SENSOR_ATUH_H_
//#define SENSOR_ATUH_H_
//
//#include <stdint.h>
//#include <stdlib.h>
//
//#ifdef __cplusplus
//extern "C" {
//#endif
//
//#define MAX_SA_PKT_LENGTH		35	/* max bytes received: HASH_SIZE+ CHALLENGE_SIZE + 1 (pkt id) */
//
//typedef enum {
//	SA_HELLO 				= 0,
//	SA_HELLO_RESP			= 1,
//	SA_HASH_ID				= 2,
//	SA_GET_ID				= 3,
//	SA_RESP_ID				= 4,
//	SA_CHALLENGE_ID			= 5,
//	SA_RESP_CHALLENGE		= 6,
//	SA_GW_KEY				= 7,
//	SA_SENSOR_KEY			= 8,
//	SA_NUM_OF_PKT_TYPES 	= 9,	/* number of valid types */
//	SA_UNKNOWN_TYPE			= 10
//} sa_pkt_type_t;
//
///* SA packet structure */
//typedef struct {
//	sa_pkt_type_t id;						/* message id */
//    uint8_t data[MAX_SA_PKT_LENGTH];                     /* pointer to the data of the packet */
//    size_t size;                    /* the length of the data in byte */
//} sa_pkt_t;
//
///*
// *	Parse packet from buffer
// * output: 0 OK else FAIL (packet size difference)
// */
//int8_t get_packet_fromBuffer(uint8_t *buffer, size_t length, sa_pkt_t *pkt);
//
/////*
//// * Start authentication procedure
//// * output: 0 OK else FAIL
//// */
////uint8_t start_authentication();
//
///*
// * Manage a packet received for authentication procedure
// * output: 0 OK else FAIL
// */
//uint8_t sa_manager(sa_pkt_t *pkt, uint16_t src);
//
//
//#ifdef __cplusplus
//}
//#endif
//
//#endif /* SENSOR_ATUH_H_ */
