/*
 * Copyright (C) 2015 Martine Lenders <mlenders@inf.fu-berlin.de>
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @defgroup    net_ng_rpl  New RPL
 * @ingroup     net
 * @{
 *
 * @file
 * @brief   TODO
 *
 * @author  Martine Lenders <mlenders@inf.fu-berlin.de>
 */
#ifndef NG_RPL_H_
#define NG_RPL_H_

//#include "net/ng_sixlowpan.h"
//#include "net/ng_netbase.h"
#include "net/ng_icmpv6.h"
#include "net/ng_rpl/ng_rpl_types.h"

#ifdef __cplusplus
extern "C" {
#endif

#define RPL_DIS_CODE					0x00
#define RPL_DIO_CODE					0x01
#define RPL_DAO_CODE					0x02
#define RPL_DAO_ACK_CODE				0x03

#define RPL_SECURE_DIS_CODE				0x80
#define RPL_SECURE_DIO_CODE				0x81
#define RPL_SECURE_DAO_CODE				0x82
#define RPL_SECURE_DAO_ACK_CODE			0x83
#define RPL_SECURE_CONS_CHECK_CODE		0x8A


///**
// * @brief   Default stack size to use for the RPL thread.
// */
//#ifndef NG_RPL_STACK_SIZE
//#define NG_RPL_STACK_SIZE  (THREAD_STACKSIZE_DEFAULT)
//#endif
//
///**
// * @brief   Default priority for the RPL thread.
// */
//#ifndef NG_RPL_PRIO
//#define NG_RPL_PRIO   (THREAD_PRIORITY_MAIN - 4)
//#endif
//
///**
// * @brief   Default message queue size to use for the 6LoWPAN thread.
// */
//#ifndef NG_RPL_MSG_QUEUE_SIZE
//#define NG_RPL_MSG_QUEUE_SIZE (8U)
//#endif

/**
 * @brief Initialization of RPL-root.
 *
 * This function initializes all RPL resources to act as a root node.
 *
 * @param[in] rpl_opts          RPL root node configurations
 *
 */
void ng_rpl_init_root(ng_rpl_options_t *rpl_opts);

/**
 * @brief   Handles received RPL message.
 *
 * @param[in] iface         The receiving interface.
 * @param[in] hdr           The icmpv6 header of received packet.
 * @param[in] icmpv6        The ICMPv6 data in @p pkt.
 * @param[in] icmpv6_size   The overall size of the message.
 */
void ng_rpl_handle(kernel_pid_t iface, ng_ipv6_hdr_t *ipv6_hdr,
					ng_icmpv6_hdr_t *hdr, uint8_t *data, size_t data_size);

/**
 * @brief Adds routing entry to routing table
 *
 * @deprecated This function is obsolete and will be removed shortly. This will be replaced with a
 * common routing information base.
 *
 * @param[in] addr                  Destination address
 * @param[in] next_hop              Next hop address
 * @param[in] lifetime              Lifetime of the entry
 *
 * */
void ng_rpl_add_routing_entry(ng_ipv6_addr_t *addr, ng_ipv6_addr_t *next_hop, uint16_t lifetime);

#ifdef __cplusplus
}
#endif

#endif /* NG_RPL_H_ */
/** @} */
