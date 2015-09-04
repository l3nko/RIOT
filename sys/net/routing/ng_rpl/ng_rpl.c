/*
 * ng_rpl.c
 *
 *  Created on: 24/lug/2015
 *      Author: l3nko
 */

#include "thread.h"
#include "net/ng_rpl.h"
#include "net/ng_rpl/ng_rpl_types.h"
#include "ng_rpl_control_messages.h"
#include "net/ng_rpl/ng_rpl_config.h"

#define ENABLE_DEBUG    (1)
#include "debug.h"

#if ENABLE_DEBUG
/* For PRIu16 etc. */
#include <inttypes.h>
#endif

#define NG_DATA_OFFSET			32//sizeof(ng_icmpv6_hdr_t)

//static kernel_pid_t _pid = KERNEL_PID_UNDEF;
//
//#if ENABLE_DEBUG
//static char _stack[NG_RPL_STACK_SIZE + THREAD_EXTRA_STACKSIZE_PRINTF];
//#else
//static char _stack[NG_RPL_STACK_SIZE];
//#endif

#if RPL_MAX_ROUTING_ENTRIES != 0
static ng_rpl_routing_entry_t rpl_routing_table[RPL_MAX_ROUTING_ENTRIES];
#endif

void ng_rpl_handle(kernel_pid_t iface, ng_ipv6_hdr_t *ipv6_hdr,
					ng_icmpv6_hdr_t *hdr, uint8_t *data, size_t data_size)
{
	#if ENABLE_DEBUG
	DEBUG("RPL msg received with code %" PRIu8 "\n", hdr->code);
	#endif

	if(!ng_ipv6_addr_equal(&ipv6_hdr->dst, ng_rpl_get_my_address())) {
		ng_rpl_set_my_address(&ipv6_hdr->dst);
	}

	switch (hdr->code) {
		case RPL_DIS_CODE: {
			//(rpl_dis_t *) & (rpl_buffer[IPV6_HDR_LEN + ICMPV6_HDR_LEN]));
			ng_rpl_dis_t *dis = (ng_rpl_dis_t*) &data[NG_DATA_OFFSET];
			ng_rpl_recv_DIS(dis, data_size, ipv6_hdr);
			break;
		}

		case RPL_DIO_CODE: {
			ng_rpl_dio_t *dio = (ng_rpl_dio_t*) &data[NG_DATA_OFFSET];
			ng_rpl_recv_DIO(dio, data_size, ipv6_hdr);
			break;
		}

		case RPL_DAO_CODE: {
			ng_rpl_dao_t *dao = (ng_rpl_dao_t*) &data[NG_DATA_OFFSET];
			ng_rpl_recv_DAO(dao, data_size, ipv6_hdr);
			break;
		}

		case RPL_DAO_ACK_CODE: {
			ng_rpl_dao_ack_t *dao_ack = (ng_rpl_dao_ack_t*) &data[NG_DATA_OFFSET];
			ng_rpl_recv_DAO_ACK(dao_ack, data_size);
			break;
		}

		default:
			DEBUG("RPL: unknown code field %" PRIu8 "\n", hdr->code);
			break;
	}
}

void ng_rpl_add_routing_entry(ng_ipv6_addr_t *addr, ng_ipv6_addr_t *next_hop, uint16_t lifetime)
{
	//TODO: only dummy implementation
	ng_rpl_routing_entry_t new_entry;
	new_entry.address = *addr;
	new_entry.next_hop = *next_hop;
	new_entry.lifetime = lifetime;
	rpl_routing_table[0] = new_entry;

}
