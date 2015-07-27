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


void ng_rpl_handle(kernel_pid_t iface, ng_icmpv6_hdr_t *hdr, uint8_t *data, size_t data_size)
{
	#if ENABLE_DEBUG
	DEBUG("RPL msg received with code %" PRIu8 "\n", hdr->code);
	#endif

	switch (hdr->code) {
		case RPL_DIS_CODE: {
			//(rpl_dis_t *) & (rpl_buffer[IPV6_HDR_LEN + ICMPV6_HDR_LEN]));
			ng_rpl_dis_t *dis = (ng_rpl_dis_t*) &data[NG_DATA_OFFSET];
			ng_rpl_recv_DIS(dis);
			break;
		}

		case RPL_DIO_CODE: {
			ng_rpl_dio_t *dio = (ng_rpl_dio_t*) &data[NG_DATA_OFFSET];
			ng_rpl_recv_DIO(dio);
			break;
		}

		case RPL_DAO_CODE: {
			ng_rpl_dao_t *dao = (ng_rpl_dao_t*) &data[NG_DATA_OFFSET];
			ng_rpl_recv_DAO(dao);
			break;
		}

		case RPL_DAO_ACK_CODE: {
			ng_rpl_dao_ack_t *dao_ack = (ng_rpl_dao_ack_t*) &data[NG_DATA_OFFSET];
			ng_rpl_recv_DAO_ACK(dao_ack);
			break;
		}

		default:
			DEBUG("RPL: unknown code field %" PRIu8 "\n", hdr->code);
			break;
	}


}
