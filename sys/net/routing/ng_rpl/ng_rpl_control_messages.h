/*
 * rpl_control_messages.h
 *
 *  Created on: 27/lug/2015
 *      Author: l3nko
 */

#ifndef NG_RPL_CONTROL_MESSAGES_H_
#define NG_RPL_CONTROL_MESSAGES_H_

#include "net/ng_rpl/ng_rpl_types.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief Receives a DIS message
 *
 * This function handles receiving a DIS message.
 *
 */
void ng_rpl_recv_DIS(ng_rpl_dis_t* dis, size_t data_size);

/**
 * @brief Receives a DIO message
 *
 * This function handles receiving a DIO message.
 *
 */
void ng_rpl_recv_DIO(ng_rpl_dio_t* dio, size_t data_size, ng_ipv6_hdr_t *ipv6_hdr);

/**
 * @brief Receives a DAO message
 *
 * This function handles receiving a DAO message.
 *
 */
void ng_rpl_recv_DAO(ng_rpl_dao_t* dao, size_t data_size, ng_ipv6_hdr_t* ipv6_hdr);

/**
 * @brief Receives a DAO_ACK message
 *
 * This function handles receiving a DAO_ACK message.
 *
 */
void ng_rpl_recv_DAO_ACK(ng_rpl_dao_ack_t* dao_ack, size_t data_size);

#ifdef __cplusplus
}
#endif

#endif /* NG_RPL_CONTROL_MESSAGES_H_ */
