/*
 * ng_rpl_control_messages.c
 *
 *  Created on: 27/lug/2015
 *      Author: l3nko
 */

#include "ng_rpl_control_messages.h"
#include "ng_rpl_dodag.h"
#include "ng_rpl_of_manager.h"
#include "net/ng_rpl.h"
#include "net/ng_rpl/ng_rpl_config.h"

#define ENABLE_DEBUG    (0)
#include "debug.h"
#include <inttypes.h>	//TODO. da rimuovere, solo per debug!!
#include "net/ng_nettype.h"

#if ENABLE_DEBUG
/* For PRIu16 etc. */
#include <inttypes.h>
static char addr_str[IPV6_MAX_ADDR_STR_LEN];
#endif

static uint8_t send_buffer[260];
//static uint8_t recv_buffer[260];

static inline uint8_t NG_RPL_COUNTER_INCREMENT(uint8_t counter)
{
    return (counter > RPL_COUNTER_LOWER_REGION ? (counter == RPL_COUNTER_MAX ? counter = 0 : ++counter) : (counter == RPL_COUNTER_LOWER_REGION ? counter = 0 : ++counter));
}

static inline bool NG_RPL_COUNTER_IS_INIT(uint8_t counter)
{
    return (counter > RPL_COUNTER_LOWER_REGION);
}

static inline bool NG_RPL_COUNTER_GREATER_THAN_LOCAL(uint8_t A, uint8_t B)
{
    return (((A < B) && (RPL_COUNTER_LOWER_REGION + 1 - B + A < RPL_COUNTER_SEQ_WINDOW)) || ((A > B) && (A - B < RPL_COUNTER_SEQ_WINDOW)));
}

static inline bool NG_RPL_COUNTER_GREATER_THAN(uint8_t A, uint8_t B)
{
    return ((A > RPL_COUNTER_LOWER_REGION) ? ((B > RPL_COUNTER_LOWER_REGION) ? NG_RPL_COUNTER_GREATER_THAN_LOCAL(A, B) : 0) : ((B > RPL_COUNTER_LOWER_REGION) ? 1 : NG_RPL_COUNTER_GREATER_THAN_LOCAL(A, B)));
}

/********* static functions *********/
//void ng_rpl_send_DAO(ng_rpl_dodag_t *my_dodag, ng_ipv6_addr_t *destination, uint8_t lifetime, bool default_lifetime,
//                  uint8_t start_index);
//void ng_rpl_send_DAO_ACK(ng_rpl_dodag_t *my_dodag, ng_ipv6_addr_t *destination);
//void ng_rpl_send_DIO(ng_rpl_dodag_t *mydodag, ng_ipv6_addr_t *destination);
//void ng_rpl_send_DIS(ng_ipv6_addr_t *destination);
/************************************/

void ng_rpl_init_root(ng_rpl_options_t *rpl_opts)
{
#if (RPL_DEFAULT_MOP == RPL_MOP_NON_STORING_MODE)
#ifndef RPL_NODE_IS_ROOT
    puts("\n############################## ERROR ###############################");
    puts("This configuration has NO ROUTING TABLE available for the root node!");
    puts("The root will NOT be INITIALIZED.");
    puts("Please build the binary for root in non-storing MOP with:");
    puts("\t\t'make RPL_NODE_IS_ROOT=1'");
    puts("############################## ERROR ###############################\n");
    return;
#endif
#endif

    ng_rpl_instance_t *inst;
    ng_rpl_dodag_t *dodag;

	if (dodag != NULL) {
		dodag->of = (struct ng_rpl_of_t *) ng_rpl_get_of_for_ocp(0);//(RPL_DEFAULT_OCP);
		dodag->instance = inst;
		dodag->mop = NG_RPL_DEFAULT_MOP;
		dodag->dtsn = 1;
		dodag->prf = 0;
		dodag->dio_interval_doubling = DEFAULT_DIO_INTERVAL_DOUBLINGS;
		dodag->dio_min = DEFAULT_DIO_INTERVAL_MIN;
		dodag->dio_redundancy = DEFAULT_DIO_REDUNDANCY_CONSTANT;
		dodag->maxrankincrease = 0;
		dodag->minhoprankincrease = (uint16_t)DEFAULT_MIN_HOP_RANK_INCREASE;
		dodag->default_lifetime = (uint8_t)RPL_DEFAULT_LIFETIME;
		dodag->lifetime_unit = RPL_LIFETIME_UNIT;
		dodag->version = RPL_COUNTER_INIT;
		dodag->grounded = RPL_GROUNDED;
		dodag->node_status = (uint8_t) ROOT_NODE;
		dodag->my_rank = RPL_ROOT_RANK;
		dodag->joined = 1;
		dodag->my_preferred_parent = NULL;
		if (rpl_opts) {
			dodag->prefix = rpl_opts->prefix;
			dodag->prefix_length = rpl_opts->prefix_len;
			dodag->prefix_flags = rpl_opts->prefix_flags;
			dodag->prefix_preferred_lifetime =
				rpl_opts->prefix_valid_lifetime ? rpl_opts->prefix_preferred_lifetime : 0xffffffff;
			dodag->prefix_valid_lifetime =
				rpl_opts->prefix_valid_lifetime ? rpl_opts->prefix_valid_lifetime : 0xffffffff;
		}
	}
	else {
		DEBUGF("Error - could not generate DODAG\n");
		return;
	}

	//TODO: capire meglio come funzionano questi trickle !!
//	trickle_start(thread_getpid(), &dodag->trickle, RPL_MSG_TYPE_TRICKLE_INTERVAL,
//	                  RPL_MSG_TYPE_TRICKLE_CALLBACK, (1 << dodag->dio_min), dodag->dio_interval_doubling,
//	                  dodag->dio_redundancy);
	DEBUGF("ROOT INIT FINISHED\n");
}

/* static functions to extract meta data */
static ng_rpl_opt_t* get_rpl_opt_from_buf(uint8_t* buffer, uint16_t offset)
{
	return ((ng_rpl_opt_t*) &(buffer[ICMPV6_HDR_LEN + offset]) );
}
static ng_rpl_opt_solicited_t* get_rpl_opt_solicited_from_buf(uint8_t* buffer, uint16_t offset)
{
	return ((ng_rpl_opt_solicited_t*) &(buffer[ICMPV6_HDR_LEN + offset]) );
}
static ng_rpl_opt_dodag_conf_t* get_rpl_opt_dodag_conf_from_buf(uint8_t* buffer, uint16_t offset)
{
	return ((ng_rpl_opt_dodag_conf_t*) &(buffer[ICMPV6_HDR_LEN + offset]) );
}
static ng_rpl_opt_prefix_information_t* get_rpl_opt_prefix_inf_from_buf(uint8_t* buffer, uint16_t offset)
{
	return ((ng_rpl_opt_prefix_information_t*) &(buffer[ICMPV6_HDR_LEN + offset]) );
}
static ng_rpl_opt_target_t* get_rpl_opt_target_from_buf(uint8_t* buffer, uint16_t offset)
{
	return ((ng_rpl_opt_target_t*) &(buffer[ICMPV6_HDR_LEN + offset]) );
}
static ng_rpl_opt_transit_t* get_rpl_opt_transit_from_buf(uint8_t* buffer, uint16_t offset)
{
	return ((ng_rpl_opt_transit_t*) &(buffer[ICMPV6_HDR_LEN + offset]) );
}
/* end static functions */

void ng_rpl_recv_DIS(ng_rpl_dis_t* dis, size_t data_size, ng_ipv6_hdr_t* ipv6_hdr)
{
	uint16_t len = DIS_BASE_LEN;
	ng_rpl_dodag_t *dodag, *end;
	uint8_t options_missing = 1;
	ng_rpl_opt_t *dis_opt = NULL;

	while (len < data_size - ICMPV6_HDR_LEN) {
		dis_opt = get_rpl_opt_from_buf((uint8_t*)dis, len);

		switch (dis_opt->type) {
			case (RPL_OPT_PAD1): {
				len += 1;
				break;
			}

			case (RPL_OPT_PADN): {
				len += dis_opt->length;
				break;
			}

			case (RPL_OPT_SOLICITED_INFO): {
				options_missing = 0;
				len += RPL_OPT_SOLICITED_INFO_LEN;

				/* extract and check */
				if (dis_opt->length != RPL_OPT_SOLICITED_INFO_LEN) {
					/* error malformed */
					return;
				}

				ng_rpl_opt_solicited_t* opt_solicited = get_rpl_opt_solicited_from_buf((uint8_t*)dis, len);
				for (dodag = ng_rpl_get_dodags(), end = dodag + RPL_MAX_DODAGS; dodag < end; dodag++) {
					if (dodag->joined) {
						if (opt_solicited->VID_Flags & RPL_DIS_I_MASK) {
							if (dodag->instance->id != opt_solicited->rplinstanceid) {
								continue;
							}
						}

						if (opt_solicited->VID_Flags & RPL_DIS_D_MASK) {
							if (!ng_rpl_equal_id(&dodag->dodag_id, &opt_solicited->dodagid)) {
								continue;
							}
						}

						if (opt_solicited->VID_Flags & RPL_DIS_V_MASK) {
							if (dodag->version != opt_solicited->version) {
								continue;
							}
						}

						ng_rpl_send_DIO(dodag, &ipv6_hdr->src);
						trickle_reset_timer(&dodag->trickle);
					}
				}

				break;
			}

			default:
				return;
		}
	}

	if (options_missing) {
		for (dodag = ng_rpl_get_dodags(), end = dodag + RPL_MAX_DODAGS; dodag < end; dodag++) {
			if (dodag->joined) {
				ng_rpl_send_DIO(dodag, &ipv6_hdr->src);
				trickle_reset_timer(&dodag->trickle);
			}
		}
	}
}


void ng_rpl_recv_DIO(ng_rpl_dio_t* dio, size_t data_size, ng_ipv6_hdr_t *ipv6_hdr)
{
	//DEBUGF("instance %04X ", rpl_dio_buf->rpl_instanceid);
	//DEBUGF("rank %04X\n", byteorder_ntohs(rpl_dio_buf->rank));
	uint16_t len = DIO_BASE_LEN;

	ng_rpl_instance_t *dio_insance = ng_rpl_get_instance(dio->rpl_instanceid);
	if(dio_insance == NULL) {
		dio_insance = ng_rpl_new_instance(dio->rpl_instanceid);

		if (dio_insance == NULL) {
			DEBUGF("Failed to create a new RPL instance!\n");
			return;
		}
	}

	ng_rpl_dodag_t dio_dodag;
	memset(&dio_dodag, 0, sizeof(dio_dodag));

	memcpy(&dio_dodag.dodag_id, &dio->dodagid, sizeof(dio_dodag.dodag_id));
	dio_dodag.dtsn = dio->dtsn;
	dio_dodag.mop = ((dio->g_mop_prf >> RPL_MOP_SHIFT) & RPL_SHIFTED_MOP_MASK);
	dio_dodag.grounded = dio->g_mop_prf >> RPL_GROUNDED_SHIFT;
	dio_dodag.prf = (dio->g_mop_prf & RPL_PRF_MASK);
	dio_dodag.version = dio->version_number;
	dio_dodag.instance = dio_insance;

	uint8_t has_dodag_conf_opt = 0;
	ng_rpl_opt_t *dio_opt = NULL;

	/* Parse until all options are consumed.  */
	while (len < data_size - ICMPV6_HDR_LEN) {
		//DEBUGF("parsing DIO options\n");
		dio_opt = get_rpl_opt_from_buf((uint8_t*)dio, len);

		switch (dio_opt->type) {

			case (RPL_OPT_PAD1): {
				len += 1;
				break;
			}

			case (RPL_OPT_PADN): {
				len += dio_opt->length;
				break;
			}

			case (RPL_OPT_DAG_METRIC_CONTAINER): {
				len += dio_opt->length;
				break;
			}

			case (RPL_OPT_ROUTE_INFO): {
				len += dio_opt->length;
				break;
			}

			case (RPL_OPT_DODAG_CONF): {
				has_dodag_conf_opt = 1;

				if (dio_opt->length != RPL_OPT_DODAG_CONF_LEN) {
					DEBUGF("DODAG configuration is malformed.\n");
					/* error malformed */
					return;
				}

				ng_rpl_opt_dodag_conf_t* opt_dodag_conf = get_rpl_opt_dodag_conf_from_buf((uint8_t*)dio, len);
				dio_dodag.dio_interval_doubling = opt_dodag_conf->DIOIntDoubl;
				dio_dodag.dio_min = opt_dodag_conf->DIOIntMin;
				dio_dodag.dio_redundancy = opt_dodag_conf->DIORedun;
				dio_dodag.maxrankincrease = byteorder_ntohs(opt_dodag_conf->MaxRankIncrease);
				dio_dodag.minhoprankincrease = byteorder_ntohs(opt_dodag_conf->MinHopRankIncrease);
				dio_dodag.default_lifetime = opt_dodag_conf->default_lifetime;
				dio_dodag.lifetime_unit = byteorder_ntohs(opt_dodag_conf->lifetime_unit);
				dio_dodag.of = (struct ng_rpl_of_t *) ng_rpl_get_of_for_ocp(byteorder_ntohs(opt_dodag_conf->ocp));
				if (dio_dodag.of == NULL) {
					DEBUGF("[Error] OCP from DIO is not supported! ocp: %x\n",
							byteorder_ntohs(opt_dodag_conf->ocp));
					return;
				}

				len += RPL_OPT_DODAG_CONF_LEN_WITH_OPT_LEN;
				break;
			}

			case (RPL_OPT_PREFIX_INFO): {
				if (dio_opt->length != RPL_OPT_PREFIX_INFO_LEN) {
					/* error malformed */
					return;
				}

				ng_rpl_opt_prefix_information_t* opt_prefix_info = get_rpl_opt_prefix_inf_from_buf((uint8_t*)dio, len);
				/* autonomous address-configuration flag */
				if (opt_prefix_info->flags & (1 << 6)) {
					ng_ipv6_addr_t tmp;
					tmp = opt_prefix_info->prefix;
					if (!ng_ipv6_addr_is_link_local(&tmp)) {
						if (byteorder_ntohl(opt_prefix_info->preferred_lifetime)
								<= byteorder_ntohl(opt_prefix_info->valid_lifetime)) {
							//TODO: cercare funzioni ng_ipv6
//							ipv6_addr_set_by_eui64(&tmp, rpl_if_id, &tmp);
//							ipv6_net_if_add_addr(rpl_if_id, &tmp,
//									NDP_ADDR_STATE_PREFERRED,
//									byteorder_ntohl(opt_prefix_info->valid_lifetime),
//									byteorder_ntohl(opt_prefix_info->preferred_lifetime),
//									0);
							dio_dodag.prefix = opt_prefix_info->prefix;
							dio_dodag.prefix_length = opt_prefix_info->prefix_length;
							dio_dodag.prefix_valid_lifetime =
								byteorder_ntohl(opt_prefix_info->valid_lifetime);
							dio_dodag.prefix_preferred_lifetime =
								byteorder_ntohl(opt_prefix_info->preferred_lifetime);
							dio_dodag.prefix_flags = opt_prefix_info->flags;
						}
					}
				}

				len += RPL_OPT_PREFIX_INFO_LEN_WITH_OPT_LEN;
				break;
			}

			default:
				DEBUGF("[Error] Unsupported DIO option\n");
				return;
		}
	}

	/* handle packet content... */
	ng_rpl_dodag_t *my_dodag = ng_rpl_get_joined_dodag(dio_insance->id);

	if (my_dodag == NULL) {
		if (!has_dodag_conf_opt) {
			DEBUGF("send DIS\n");
			ng_rpl_send_DIS(&ipv6_hdr->src);
		}

		if (byteorder_ntohs(dio->rank) < ROOT_RANK) {
			DEBUGF("DIO with Rank < ROOT_RANK\n");
		}

		if (dio_dodag.mop != NG_RPL_DEFAULT_MOP) {
			DEBUGF("Required MOP not supported\n");
		}

		if (dio_dodag.of == NULL) {
			DEBUGF("Required objective function not supported\n");
		}

		if (byteorder_ntohs(dio->rank) != INFINITE_RANK) {
			DEBUGF("Will join DODAG\n");
			ng_rpl_join_dodag(&dio_dodag, &ipv6_hdr->src, byteorder_ntohs(dio->rank));
		}
		else {
			DEBUGF("Cannot access DODAG because of DIO with infinite rank\n");
		}

		return;
	}

	if (ng_rpl_equal_id(&my_dodag->dodag_id, &dio_dodag.dodag_id)) {
		/* "our" DODAG */
		if (NG_RPL_COUNTER_GREATER_THAN(dio_dodag.version, my_dodag->version)) {
			if (my_dodag->my_rank == ROOT_RANK) {
				DEBUGF("[Warning] Inconsistent Dodag Version\n");
				my_dodag->version = NG_RPL_COUNTER_INCREMENT(dio_dodag.version);
				trickle_reset_timer(&my_dodag->trickle);
			}
			else {
				DEBUGF("my dodag has no preferred_parent yet - seems to be odd since I have a parent.\n");
				my_dodag->version = dio_dodag.version;
				ng_rpl_global_repair(my_dodag, &ipv6_hdr->src, byteorder_ntohs(dio->rank));
			}

			return;
		}
		else if (NG_RPL_COUNTER_GREATER_THAN(my_dodag->version, dio_dodag.version)) {
			/* lower version number detected -> send more DIOs */
			trickle_reset_timer(&my_dodag->trickle);
			return;
		}
	}

	/* version matches, DODAG matches */
	if (byteorder_ntohs(dio->rank) == INFINITE_RANK) {
		trickle_reset_timer(&my_dodag->trickle);
	}

	/* We are root, all done!*/
	if (my_dodag->my_rank == ROOT_RANK) {
		if (byteorder_ntohs(dio->rank) != INFINITE_RANK) {
			trickle_increment_counter(&my_dodag->trickle);
		}

		return;
	}

	/*********************  Parent Handling *********************/

	ng_rpl_parent_t *parent = ng_rpl_find_parent(my_dodag, &ipv6_hdr->src);
	if (parent == NULL) {
		/* add new parent candidate */
		parent = ng_rpl_new_parent(my_dodag, &ipv6_hdr->src, byteorder_ntohs(dio->rank));
		if (parent == NULL) {
			return;
		}
	}
	else {
		/* DIO OK */
		trickle_increment_counter(&my_dodag->trickle);
	}

	/* update parent rank */
	parent->rank = byteorder_ntohs(dio->rank);
	ng_rpl_parent_update(my_dodag, parent);

	if (my_dodag->my_preferred_parent == NULL) {
		DEBUGF("My dodag has no preferred_parent yet - seems to be odd since I have a parent...\n");
	}
	else if (ng_rpl_equal_id(&parent->addr, &my_dodag->my_preferred_parent->addr) &&
			 (parent->dtsn != dio->dtsn)) {
		//TODO
		//rpl_delay_dao(my_dodag);
	}

	parent->dtsn = dio->dtsn;

}

void ng_rpl_recv_DAO(ng_rpl_dao_t* dao, size_t data_size, ng_ipv6_hdr_t* ipv6_hdr)
{
#if NG_RPL_DEFAULT_MOP == NG_RPL_MOP_NON_STORING_MODE
    DEBUGF("[Error] something went wrong - got a DAO.\n");
    return;
#else
	#if ENABLE_DEBUG
    	DEBUG("instance %04X ", dao->rpl_instanceid);
    	DEBUG("sequence %04X\n", dao->dao_sequence);
	#endif

    ng_rpl_dodag_t *my_dodag = ng_rpl_get_joined_dodag(dao->rpl_instanceid);

    if (my_dodag == NULL) {
        DEBUG("[Error] got DAO although not a DODAG\n");
        return;
    }

    uint16_t len = DAO_BASE_LEN;
    uint8_t increment_seq = 0;

    ng_rpl_opt_t* dao_opt;
    while (len < data_size - ICMPV6_HDR_LEN) {
    	//DEBUGF("parsing DIO options\n");
    	dao_opt = get_rpl_opt_from_buf((uint8_t*)dao, len);

        switch (dao_opt->type) {

            case (RPL_OPT_PAD1): {
                len += 1;
                break;
            }

            case (RPL_OPT_PADN): {
                len += (dao_opt->length + RPL_OPT_LEN);
                break;
            }

            case (RPL_OPT_DAG_METRIC_CONTAINER): {
                len += (dao_opt->length + RPL_OPT_LEN);
                break;
            }

            case (RPL_OPT_TARGET): {
            	ng_rpl_opt_target_t* opt_target = (ng_rpl_opt_target_t*)get_rpl_opt_target_from_buf((uint8_t*)dao, len);
                if (opt_target->prefix_length != RPL_DODAG_ID_LEN) {
                    DEBUGF("prefixes are not supported yet\n");
                    break;
                }

                len += (opt_target->length + RPL_OPT_LEN);
                ng_rpl_opt_transit_t* opt_transit = get_rpl_opt_transit_from_buf((uint8_t*)dao, len);

                if (opt_transit->type != RPL_OPT_TRANSIT) {
                    DEBUGF("[Error] - no transit information for target option type = %d\n",
                    		opt_transit->type);
                    break;
                }

                len += (opt_transit->length + RPL_OPT_LEN);
                /* route lifetime seconds = (DAO lifetime) * (Unit Lifetime) */

#if (ng_RPL_DEFAULT_MOP == NG_RPL_MOP_NON_STORING_MODE) && (RPL_MAX_ROUTING_ENTRIES != 0)
                rpl_add_srh_entry(&opt_target->target, &opt_transit->parent,
                		opt_transit->path_lifetime * my_dodag->lifetime_unit);
#elif (RPL_MAX_ROUTING_ENTRIES != 0)
                DEBUG("Adding routing information: Target: %s, Source: %s, Lifetime: %u\n",
						ng_ipv6_addr_to_str(addr_str, IPV6_MAX_ADDR_STR_LEN, &opt_target->target),
						ng_ipv6_addr_to_str(addr_str, IPV6_MAX_ADDR_STR_LEN, &ipv6_hdr->src),
						(opt_transit->path_lifetime * my_dodag->lifetime_unit));

                ng_rpl_add_routing_entry(&opt_target->target, &ipv6_hdr->src,
                		opt_transit->path_lifetime * my_dodag->lifetime_unit);
#endif
                increment_seq = 1;
                break;
            }

            case (RPL_OPT_TRANSIT): {
                len += (dao_opt->length + RPL_OPT_LEN);
                break;
            }

            case (RPL_OPT_TARGET_DESC): {
                len += (dao_opt->length + RPL_OPT_LEN);
                break;
            }

            default:
                return;
        }
    }

#if NG_RPL_DEFAULT_MOP != NG_RPL_MOP_NON_STORING_MODE
    //TODO:
    //rpl_send_DAO_ACK(my_dodag, &ipv6_buf->srcaddr);
#endif

    if (increment_seq) {
        NG_RPL_COUNTER_INCREMENT(my_dodag->dao_seq);
        //TODO:
        //rpl_delay_dao(my_dodag);
    }
#endif
}

void ng_rpl_recv_DAO_ACK(ng_rpl_dao_ack_t* dao_ack, size_t data_size)
{
	ng_rpl_dodag_t *dodag = ng_rpl_get_joined_dodag(dao_ack->rpl_instanceid);

	if (dodag == NULL) {
		DEBUG("DAO_ACK not good: dodag null\n");
		return;
	}

	if (dao_ack->rpl_instanceid != dodag->instance->id) {
		DEBUG("DAO_ACK not good: rpl instance id different\n");
		return;
	}

	if (dao_ack->status != 0) {
		DEBUG("DAO_ACK not good: status not zero\n");
		return;
	}

	dodag->ack_received = true;
	//TODO: long_delay_dao(dodag);
}

/* obligatory for each mode. normally not modified */
void ng_rpl_send(ng_ipv6_addr_t *destination, uint8_t *payload, uint16_t p_len, uint8_t code)
{
	//TODO: DA TESTARE !!!!!!!!
	ng_pktsnip_t rpl_pkt;
	ng_pktsnip_t *icmpv6;
	ng_pktsnip_t *hdr;
	ng_ipv6_addr_t *src;

	rpl_pkt.type = NG_NETTYPE_ICMPV6;
	rpl_pkt.data = (void*)payload;
	rpl_pkt.size = p_len;

	/* ONLY DEBUG */
	uint8_t* pkt_bufer = (uint8_t*)rpl_pkt.data;
	printf("[ng_rpl] Dump pkt(0x%04x) with type %"PRIu8 " and size %d\n", &rpl_pkt,rpl_pkt.type, rpl_pkt.size);
	for(uint16_t i=0; i<rpl_pkt.size; i++) {
		printf("%"PRIu8 " ", pkt_bufer[i]);
	}
	printf("\n");
	/*********/

	icmpv6 = ng_icmpv6_build(&rpl_pkt, NG_ICMPV6_RPL_CTRL, code, p_len);
    //TODO: int ng_icmpv6_calc_csum(ng_pktsnip_t *hdr, ng_pktsnip_t *pseudo_hdr);

    /* ONLY DEBUG */
	uint8_t* icmpv6_bufer = (uint8_t*)icmpv6->data;
	printf("[ng_rpl] Dump icmpv6(0x%04x) with type %"PRIu8 " size %d and next 0x%04x\n", icmpv6,icmpv6->type, icmpv6->size, icmpv6->next);
	for(uint16_t i=0; i<icmpv6->size; i++) {
		printf("%"PRIu8 " ", icmpv6_bufer[i]);
	}
	printf("\n");
	/*********/

	//build header
	//TODO: farsi passare iface
	kernel_pid_t iface = KERNEL_PID_UNDEF;
	src = ng_ipv6_netif_find_best_src_addr(iface, destination);
	hdr = ng_ipv6_hdr_build(icmpv6, (uint8_t *)src, sizeof(ng_ipv6_addr_t),
								(uint8_t *)destination, sizeof(ng_ipv6_addr_t));

	if(hdr != NULL) {
		ng_ipv6_hdr_t* ipv6_hdr = (ng_ipv6_hdr_t *)hdr->data;
		ipv6_hdr->hl = 255;
		ipv6_hdr->v_tc_fl.u32 = 0x60; //IPV6_VER;
		//    ipv6_send_buf->trafficclass_flowlabel = 0;
		//    ipv6_send_buf->flowlabel = 0;
		//    ipv6_send_buf->nextheader = next_header;
		//    ipv6_send_buf->hoplimit = MULTIHOP_HOPLIMIT;
		//    ipv6_send_buf->length = HTONS(p_len);
	}
	else {
		puts("error: packet header buffer full");
		return;
	}

	/* ONLY DEBUG */
	uint8_t* ipv6_bufer = (uint8_t*)hdr->data;
	printf("[ng_rpl] Dump ipv6t(0x%04x) with type %"PRIu8 " size %d and next 0x%04x\n", hdr,hdr->type, hdr->size, hdr->next);
	for(uint16_t i=0; i<hdr->size; i++) {
		printf("%"PRIu8 " ", ipv6_bufer[i]);
	}
	printf("\n");
	/*********/

//	ng_netapi_send(iface, pkt);

	//TODO: clear send_buffer
	memset(send_buffer, 0x00, sizeof(send_buffer));
}

/* implementation of static functions */
void ng_rpl_send_DAO(ng_rpl_dodag_t *my_dodag, ng_ipv6_addr_t *destination, uint8_t lifetime, bool default_lifetime,
                  uint8_t start_index)
{
	ng_rpl_dao_t* dao = (ng_rpl_dao_t*)send_buffer;
#if RPL_DEFAULT_MOP == RPL_MOP_NON_STORING_MODE
    (void) start_index;
#endif

#if ENABLE_DEBUG

    if (destination) {
        DEBUGF("Send DAO to %s\n", ipv6_addr_to_str(addr_str, IPV6_MAX_ADDR_STR_LEN, destination));
    }
    else {
        DEBUGF("Send DAO to default destination\n");
    }

#endif

    if (my_dodag == NULL) {
        DEBUGF("send_DAO: I have no my_dodag\n");
        return;
    }

    if (my_dodag->node_status == ROOT_NODE) {
        return;
    }

    if (destination == NULL) {
#if RPL_DEFAULT_MOP == RPL_MOP_NON_STORING_MODE
        destination = &my_dodag->dodag_id;
#else

        if (my_dodag->my_preferred_parent == NULL) {
            DEBUGF("send_DAO: my_dodag has no my_preferred_parent\n");
            return;
        }

        destination = &my_dodag->my_preferred_parent->addr;
#endif
    }

    if (default_lifetime) {
        lifetime = my_dodag->default_lifetime;
    }


    dao->rpl_instanceid = my_dodag->instance->id;
    dao->k_d_flags = 0x00;
    dao->dao_sequence = my_dodag->dao_seq;

    uint16_t opt_len = 0;
//    rpl_send_opt_target_buf = get_rpl_send_opt_target_buf(DAO_BASE_LEN);
    ng_rpl_opt_target_t* opt_target = get_rpl_opt_target_from_buf(send_buffer, DAO_BASE_LEN);


#if RPL_DEFAULT_MOP != RPL_MOP_NON_STORING_MODE
    /* add all targets from routing table as targets */
    uint8_t entries = 0;
    uint8_t continue_index = 0;

    for (uint8_t i = start_index; i < rpl_max_routing_entries; i++) {
        if (ng_rpl_get_routing_table()[i].used) {
        	opt_target->type = RPL_OPT_TARGET;
        	opt_target->length = RPL_OPT_TARGET_LEN;
        	opt_target->flags = 0x00;
        	opt_target->prefix_length = RPL_DODAG_ID_LEN;
            memcpy(&opt_target->target, &rpl_get_routing_table()[i].address,
                   sizeof(ng_ipv6_addr_t));
            opt_len += RPL_OPT_TARGET_LEN_WITH_OPT_LEN;
//            rpl_send_opt_transit_buf = get_rpl_send_opt_transit_buf(DAO_BASE_LEN + opt_len);
            ng_rpl_opt_transit_t* opt_transit = get_rpl_opt_transit_from_buf(send_buffer, DAO_BASE_LEN + opt_len);
            opt_transit->type = RPL_OPT_TRANSIT;
            opt_transit->length = (RPL_OPT_TRANSIT_LEN - sizeof(ipv6_addr_t));
            opt_transit->e_flags = 0x00;
            opt_transit->path_control = 0x00; /* not used */
            opt_transit->path_sequence = 0x00; /* not used */
            opt_transit->path_lifetime = lifetime;
            opt_len += (RPL_OPT_TRANSIT_LEN_WITH_OPT_LEN - sizeof(ipv6_addr_t));
//            rpl_send_opt_target_buf = get_rpl_send_opt_target_buf(DAO_BASE_LEN + opt_len);
            opt_target = get_rpl_opt_target_from_buf(send_buffer, DAO_BASE_LEN + opt_len);
            entries++;
        }

        /* Split DAO, so packages don't get too big.
         * The value 5 is based on experience. */
        if (entries >= 5) {
            continue_index = i + 1;
            break;
        }
    }

#endif
    /* add own address */
    opt_target->type = RPL_OPT_TARGET;
    opt_target->length = RPL_OPT_TARGET_LEN;
    opt_target->flags = 0x00;
    opt_target->prefix_length = RPL_DODAG_ID_LEN;
    if (!ng_ipv6_addr_is_unspecified(&my_dodag->prefix)) {
    	ng_ipv6_addr_t tmp;
        //TODO: ?!?!?
//    	ng_ipv6_addr_set_by_eui64(&tmp, rpl_if_id, &my_dodag->prefix);
        memcpy(&opt_target->target, &tmp, sizeof(ng_ipv6_addr_t));
    }
    else {
    	//TODO: my_address?!?!
//        memcpy(&opt_target->target, &my_address, sizeof(ng_ipv6_addr_t));
    }
    opt_len += RPL_OPT_TARGET_LEN_WITH_OPT_LEN;

//    rpl_send_opt_transit_buf = get_rpl_send_opt_transit_buf(DAO_BASE_LEN + opt_len);
    ng_rpl_opt_transit_t* opt_transit = get_rpl_opt_transit_from_buf(send_buffer, DAO_BASE_LEN + opt_len);
    opt_transit->type = RPL_OPT_TRANSIT;
    opt_transit->e_flags = 0x00;
    opt_transit->path_control = 0x00;
    opt_transit->path_sequence = 0x00;
    opt_transit->path_lifetime = lifetime;

#if RPL_DEFAULT_MOP == RPL_MOP_NON_STORING_MODE
    opt_transit->length = RPL_OPT_TRANSIT_LEN;
    memcpy(&opt_transit->parent, &my_dodag->my_preferred_parent->addr, sizeof(ng_ipv6_addr_t));
    opt_len += RPL_OPT_TRANSIT_LEN_WITH_OPT_LEN;
#else
    rpl_send_opt_transit_buf->length = (RPL_OPT_TRANSIT_LEN - sizeof(ng_ipv6_addr_t));
    opt_len += (RPL_OPT_TRANSIT_LEN_WITH_OPT_LEN - sizeof(ng_ipv6_addr_t));
#endif

//    uint16_t plen = ICMPV6_HDR_LEN + DAO_BASE_LEN + opt_len;
//    rpl_send(destination, (uint8_t *)icmp_send_buf, plen, IPV6_PROTO_NUM_ICMPV6);
    uint16_t plen = DAO_BASE_LEN + opt_len;
    ng_rpl_send(destination, send_buffer, plen, RPL_DAO_CODE);

#if RPL_DEFAULT_MOP != RPL_MOP_NON_STORING_MODE
    if (continue_index > 1) {
//        rpl_send_DAO(my_dodag, destination, lifetime, default_lifetime, continue_index);
        ng_rpl_send_DAO(my_dodag, destination, lifetime, default_lifetime, continue_index)
    }

#endif
}

void ng_rpl_send_DAO_ACK(ng_rpl_dodag_t *my_dodag, ng_ipv6_addr_t *destination)
{
	#if ENABLE_DEBUG
    if (destination) {
        DEBUGF("Send DAO ACK to %s\n",
               ng_ipv6_addr_to_str(addr_str, IPV6_MAX_ADDR_STR_LEN, destination));
    }
	#endif

    if (my_dodag == NULL) {
        return;
    }

    ng_rpl_dao_ack_t* dao_ack = (ng_rpl_dao_ack_t*)send_buffer;
    dao_ack->rpl_instanceid = my_dodag->instance->id;
    dao_ack->d_reserved = 0;
    dao_ack->dao_sequence = my_dodag->dao_seq;
    dao_ack->status = 0;

    ng_rpl_send(destination, send_buffer, sizeof(ng_rpl_dao_ack_t), RPL_DAO_ACK_CODE);
}

void ng_rpl_send_DIO(ng_rpl_dodag_t *mydodag, ng_ipv6_addr_t *destination)
{
#if ENABLE_DEBUG

    if (destination) {
        DEBUGF("Send DIO to %s\n", ng_ipv6_addr_to_str(addr_str, IPV6_MAX_ADDR_STR_LEN, destination));
    }

#endif

    if (mydodag == NULL) {
        DEBUGF("Error - trying to send DIO without being part of a dodag.\n");
        return;
    }

    ng_rpl_dio_t* dio = (ng_rpl_dio_t*)send_buffer;
    DEBUGF("Sending DIO with ");
    dio->rpl_instanceid = mydodag->instance->id;
    DEBUG("instance %02X ", dio->rpl_instanceid);
    dio->version_number = mydodag->version;
    dio->rank = byteorder_htons(mydodag->my_rank);
    DEBUG("rank %04X\n", byteorder_ntohs(dio->rank));
    dio->g_mop_prf = (mydodag->grounded << RPL_GROUNDED_SHIFT) |
				     (mydodag->mop << RPL_MOP_SHIFT) | mydodag->prf;
    dio->dtsn = mydodag->dtsn;
    dio->flags = 0;
    dio->reserved = 0;
    dio->dodagid = mydodag->dodag_id;

    uint16_t opt_hdr_len = 0;
    /* DODAG configuration option */
//    rpl_send_opt_dodag_conf_buf = get_rpl_send_opt_dodag_conf_buf(DIO_BASE_LEN);
    ng_rpl_opt_dodag_conf_t* opt_dodag = get_rpl_opt_dodag_conf_from_buf(send_buffer, DIO_BASE_LEN);
    opt_dodag->type = RPL_OPT_DODAG_CONF;
    opt_dodag->length = RPL_OPT_DODAG_CONF_LEN;
    opt_dodag->flags_a_pcs = 0;
    opt_dodag->DIOIntDoubl = mydodag->dio_interval_doubling;
    opt_dodag->DIOIntMin = mydodag->dio_min;
    opt_dodag->DIORedun = mydodag->dio_redundancy;
    opt_dodag->MaxRankIncrease = byteorder_htons(mydodag->maxrankincrease);
    opt_dodag->MinHopRankIncrease = byteorder_htons(mydodag->minhoprankincrease);
    opt_dodag->ocp = byteorder_htons(mydodag->of->ocp);
    opt_dodag->reserved = 0;
    opt_dodag->default_lifetime = mydodag->default_lifetime;
    opt_dodag->lifetime_unit = byteorder_htons(mydodag->lifetime_unit);

    opt_hdr_len += RPL_OPT_DODAG_CONF_LEN_WITH_OPT_LEN;

    if (!ng_ipv6_addr_is_unspecified(&mydodag->prefix)) {
//        rpl_send_opt_prefix_information_buf = get_rpl_send_opt_prefix_information_buf(DIO_BASE_LEN + opt_hdr_len);
    	ng_rpl_opt_prefix_information_t* opt_prefix = get_rpl_opt_prefix_inf_from_buf(send_buffer, DIO_BASE_LEN + opt_hdr_len);
    	opt_prefix->type = RPL_OPT_PREFIX_INFO;
    	opt_prefix->length = RPL_OPT_PREFIX_INFO_LEN;
    	opt_prefix->flags = mydodag->prefix_flags;
    	opt_prefix->prefix = mydodag->prefix;
    	opt_prefix->prefix_length = mydodag->prefix_length;
        opt_prefix->preferred_lifetime = byteorder_htonl(mydodag->prefix_preferred_lifetime);
        opt_prefix->valid_lifetime = byteorder_htonl(mydodag->prefix_valid_lifetime);

        opt_hdr_len += RPL_OPT_PREFIX_INFO_LEN_WITH_OPT_LEN;
    }

//    uint16_t plen = ICMPV6_HDR_LEN + DIO_BASE_LEN + opt_hdr_len;
//    rpl_send(destination, (uint8_t *)icmp_send_buf, plen, IPV6_PROTO_NUM_ICMPV6);
    uint16_t plen = DIO_BASE_LEN + opt_hdr_len;
    ng_rpl_send(destination, send_buffer, plen, RPL_DIO_CODE);
}

void ng_rpl_send_DIS(ng_ipv6_addr_t *destination)
{
#if ENABLE_DEBUG

    if (destination) {
        DEBUGF("Send DIS to %s\n", ng_ipv6_addr_to_str(addr_str, IPV6_MAX_ADDR_STR_LEN, destination));
    }

#endif


    ng_rpl_dis_t* dis = (ng_rpl_dis_t*)send_buffer;
    dis->flags = 0x00;
    dis->reserved = 0x00;

//    uint16_t plen = ICMPV6_HDR_LEN + DIS_BASE_LEN;
//    rpl_send(destination, (uint8_t *)icmp_send_buf, plen, IPV6_PROTO_NUM_ICMPV6);
    uint16_t plen = DIS_BASE_LEN;
    ng_rpl_send(destination, send_buffer, plen, RPL_DIS_CODE);

}


