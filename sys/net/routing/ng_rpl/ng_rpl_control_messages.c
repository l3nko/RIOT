/*
 * ng_rpl_control_messages.c
 *
 *  Created on: 27/lug/2015
 *      Author: l3nko
 */

#include "ng_rpl_control_messages.h"
#include "ng_rpl_dodag.h"
#include "net/ng_rpl.h"

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
		dodag->of = (struct rpl_of_t *) rpl_get_of_for_ocp(RPL_DEFAULT_OCP);
		dodag->instance = inst;
//		dodag->mop = RPL_DEFAULT_MOP;
		dodag->dtsn = 1;
		dodag->prf = 0;
//		dodag->dio_interval_doubling = DEFAULT_DIO_INTERVAL_DOUBLINGS;
//		dodag->dio_min = DEFAULT_DIO_INTERVAL_MIN;
//		dodag->dio_redundancy = DEFAULT_DIO_REDUNDANCY_CONSTANT;
//		dodag->maxrankincrease = 0;
//		dodag->minhoprankincrease = (uint16_t)DEFAULT_MIN_HOP_RANK_INCREASE;
//		dodag->default_lifetime = (uint8_t)RPL_DEFAULT_LIFETIME;
//		dodag->lifetime_unit = RPL_LIFETIME_UNIT;
//		dodag->version = RPL_COUNTER_INIT;
//		dodag->grounded = RPL_GROUNDED;
//		dodag->node_status = (uint8_t) ROOT_NODE;
//		dodag->my_rank = RPL_ROOT_RANK;
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

//	trickle_start(rpl_process_pid, &dodag->trickle, RPL_MSG_TYPE_TRICKLE_INTERVAL,
//	                  RPL_MSG_TYPE_TRICKLE_CALLBACK, (1 << dodag->dio_min), dodag->dio_interval_doubling,
//	                  dodag->dio_redundancy);
	DEBUGF("ROOT INIT FINISHED\n");
}

void ng_rpl_recv_DIS(ng_rpl_dis_t* dis)
{
	int len = 2; //DIS_BASE_LEN;
	ng_rpl_dodag_t *dodag, *end;
	uint8_t options_missing = 1;

//	while (len < (NTOHS(ipv6_buf->length) - ICMPV6_HDR_LEN)) {
//		rpl_opt_buf = get_rpl_opt_buf(len);
//
//		switch (rpl_opt_buf->type) {
//			case (RPL_OPT_PAD1): {
//				len += 1;
//				break;
//			}
//
//			case (RPL_OPT_PADN): {
//				len += rpl_opt_buf->length;
//				break;
//			}
//
//			case (RPL_OPT_SOLICITED_INFO): {
//				options_missing = 0;
//				len += RPL_OPT_SOLICITED_INFO_LEN;
//
//				/* extract and check */
//				if (rpl_opt_buf->length != RPL_OPT_SOLICITED_INFO_LEN) {
//					/* error malformed */
//					return;
//				}
//
//				rpl_opt_solicited_buf = get_rpl_opt_solicited_buf(len);
//
//				for (dodag = rpl_dodags, end = dodag + RPL_MAX_DODAGS; dodag < end; dodag++) {
//					if (dodag->joined) {
//						if (rpl_opt_solicited_buf->VID_Flags & RPL_DIS_I_MASK) {
//							if (dodag->instance->id != rpl_opt_solicited_buf->rplinstanceid) {
//								continue;
//							}
//						}
//
//						if (rpl_opt_solicited_buf->VID_Flags & RPL_DIS_D_MASK) {
//							if (!rpl_equal_id(&dodag->dodag_id, &rpl_opt_solicited_buf->dodagid)) {
//								continue;
//							}
//						}
//
//						if (rpl_opt_solicited_buf->VID_Flags & RPL_DIS_V_MASK) {
//							if (dodag->version != rpl_opt_solicited_buf->version) {
//								continue;
//							}
//						}
//
//						rpl_send_DIO(dodag, &ipv6_buf->srcaddr);
//						trickle_reset_timer(&dodag->trickle);
//					}
//				}
//
//				break;
//			}
//
//			default:
//				return;
//		}
//	}
//
//	if (options_missing) {
//		for (dodag = rpl_dodags, end = dodag + RPL_MAX_DODAGS; dodag < end; dodag++) {
//			if (dodag->joined) {
//				rpl_send_DIO(dodag, &ipv6_buf->srcaddr);
//				trickle_reset_timer(&dodag->trickle);
//			}
//		}
//	}
}


void ng_rpl_recv_DIO(ng_rpl_dio_t* dio)
{
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
//	dio_dodag.mop = ((dio->g_mop_prf >> RPL_MOP_SHIFT) & RPL_SHIFTED_MOP_MASK);
//	dio_dodag.grounded = dio->g_mop_prf >> RPL_GROUNDED_SHIFT;
//	dio_dodag.prf = (dio->g_mop_prf & RPL_PRF_MASK);
	dio_dodag.version = dio->version_number;
	dio_dodag.instance = dio_insance;

//	uint8_t has_dodag_conf_opt = 0;

//	/* Parse until all options are consumed.
//	     * ipv6_buf->length contains the packet length minus ipv6 and
//	     * icmpv6 header, so only ICMPV6_HDR_LEN remains to be
//	     * subtracted.  */
//	    while (len < (NTOHS(ipv6_buf->length) - ICMPV6_HDR_LEN)) {
//	        DEBUGF("parsing DIO options\n");
//	        rpl_opt_buf = get_rpl_opt_buf(len);
//
//	        switch (rpl_opt_buf->type) {
//
//	            case (RPL_OPT_PAD1): {
//	                len += 1;
//	                break;
//	            }
//
//	            case (RPL_OPT_PADN): {
//	                len += rpl_opt_buf->length;
//	                break;
//	            }
//
//	            case (RPL_OPT_DAG_METRIC_CONTAINER): {
//	                len += rpl_opt_buf->length;
//	                break;
//	            }
//
//	            case (RPL_OPT_ROUTE_INFO): {
//	                len += rpl_opt_buf->length;
//	                break;
//	            }
//
//	            case (RPL_OPT_DODAG_CONF): {
//	                has_dodag_conf_opt = 1;
//
//	                if (rpl_opt_buf->length != RPL_OPT_DODAG_CONF_LEN) {
//	                    DEBUGF("DODAG configuration is malformed.\n");
//	                    /* error malformed */
//	                    return;
//	                }
//
//	                rpl_opt_dodag_conf_buf = get_rpl_opt_dodag_conf_buf(len);
//	                dio_dodag.dio_interval_doubling = rpl_opt_dodag_conf_buf->DIOIntDoubl;
//	                dio_dodag.dio_min = rpl_opt_dodag_conf_buf->DIOIntMin;
//	                dio_dodag.dio_redundancy = rpl_opt_dodag_conf_buf->DIORedun;
//	                dio_dodag.maxrankincrease = byteorder_ntohs(rpl_opt_dodag_conf_buf->MaxRankIncrease);
//	                dio_dodag.minhoprankincrease = byteorder_ntohs(rpl_opt_dodag_conf_buf->MinHopRankIncrease);
//	                dio_dodag.default_lifetime = rpl_opt_dodag_conf_buf->default_lifetime;
//	                dio_dodag.lifetime_unit = byteorder_ntohs(rpl_opt_dodag_conf_buf->lifetime_unit);
//	                dio_dodag.of = (struct rpl_of_t *) rpl_get_of_for_ocp(byteorder_ntohs(rpl_opt_dodag_conf_buf->ocp));
//	                if (dio_dodag.of == NULL) {
//	                    DEBUGF("[Error] OCP from DIO is not supported! ocp: %x\n",
//	                    byteorder_ntohs(rpl_opt_dodag_conf_buf->ocp));
//	                    return;
//	                }
//
//	                len += RPL_OPT_DODAG_CONF_LEN_WITH_OPT_LEN;
//	                break;
//	            }
//
//	            case (RPL_OPT_PREFIX_INFO): {
//	                if (rpl_opt_buf->length != RPL_OPT_PREFIX_INFO_LEN) {
//	                    /* error malformed */
//	                    return;
//	                }
//
//	                rpl_opt_prefix_information_buf = get_rpl_opt_prefix_information_buf(len);
//
//	                /* autonomous address-configuration flag */
//	                if (rpl_opt_prefix_information_buf->flags & (1 << 6)) {
//	                    ipv6_addr_t tmp;
//	                    tmp = rpl_opt_prefix_information_buf->prefix;
//	                    if (!ipv6_addr_is_link_local(&tmp)) {
//	                        if (byteorder_ntohl(rpl_opt_prefix_information_buf->preferred_lifetime)
//	                                <= byteorder_ntohl(rpl_opt_prefix_information_buf->valid_lifetime)) {
//	                            ipv6_addr_set_by_eui64(&tmp, rpl_if_id, &tmp);
//	                            ipv6_net_if_add_addr(rpl_if_id, &tmp,
//	                                    NDP_ADDR_STATE_PREFERRED,
//	                                    byteorder_ntohl(rpl_opt_prefix_information_buf->valid_lifetime),
//	                                    byteorder_ntohl(rpl_opt_prefix_information_buf->preferred_lifetime),
//	                                    0);
//	                            dio_dodag.prefix = rpl_opt_prefix_information_buf->prefix;
//	                            dio_dodag.prefix_length = rpl_opt_prefix_information_buf->prefix_length;
//	                            dio_dodag.prefix_valid_lifetime =
//	                                byteorder_ntohl(rpl_opt_prefix_information_buf->valid_lifetime);
//	                            dio_dodag.prefix_preferred_lifetime =
//	                                byteorder_ntohl(rpl_opt_prefix_information_buf->preferred_lifetime);
//	                            dio_dodag.prefix_flags = rpl_opt_prefix_information_buf->flags;
//	                        }
//	                    }
//	                }
//
//	                len += RPL_OPT_PREFIX_INFO_LEN_WITH_OPT_LEN;
//	                break;
//	            }
//
//	            default:
//	                DEBUGF("[Error] Unsupported DIO option\n");
//	                return;
//	        }
//	    }
//
//	    /* handle packet content... */
//	    rpl_dodag_t *my_dodag = rpl_get_joined_dodag(dio_inst->id);
//
//	    if (my_dodag == NULL) {
//	        if (!has_dodag_conf_opt) {
//	            DEBUGF("send DIS\n");
//	            rpl_send_DIS(&ipv6_buf->srcaddr);
//	        }
//
//	        if (byteorder_ntohs(rpl_dio_buf->rank) < ROOT_RANK) {
//	            DEBUGF("DIO with Rank < ROOT_RANK\n");
//	        }
//
//	        if (dio_dodag.mop != RPL_DEFAULT_MOP) {
//	            DEBUGF("Required MOP not supported\n");
//	        }
//
//	        if (dio_dodag.of == NULL) {
//	            DEBUGF("Required objective function not supported\n");
//	        }
//
//	        if (byteorder_ntohs(rpl_dio_buf->rank) != INFINITE_RANK) {
//	            DEBUGF("Will join DODAG\n");
//	            rpl_join_dodag(&dio_dodag, &ipv6_buf->srcaddr, byteorder_ntohs(rpl_dio_buf->rank));
//	        }
//	        else {
//	            DEBUGF("Cannot access DODAG because of DIO with infinite rank\n");
//	        }
//
//	        return;
//	    }
//
//	    if (rpl_equal_id(&my_dodag->dodag_id, &dio_dodag.dodag_id)) {
//	        /* "our" DODAG */
//	        if (RPL_COUNTER_GREATER_THAN(dio_dodag.version, my_dodag->version)) {
//	            if (my_dodag->my_rank == ROOT_RANK) {
//	                DEBUGF("[Warning] Inconsistent Dodag Version\n");
//	                my_dodag->version = RPL_COUNTER_INCREMENT(dio_dodag.version);
//	                trickle_reset_timer(&my_dodag->trickle);
//	            }
//	            else {
//	                DEBUGF("my dodag has no preferred_parent yet - seems to be odd since I have a parent.\n");
//	                my_dodag->version = dio_dodag.version;
//	                rpl_global_repair(my_dodag, &ipv6_buf->srcaddr, byteorder_ntohs(rpl_dio_buf->rank));
//	            }
//
//	            return;
//	        }
//	        else if (RPL_COUNTER_GREATER_THAN(my_dodag->version, dio_dodag.version)) {
//	            /* lower version number detected -> send more DIOs */
//	            trickle_reset_timer(&my_dodag->trickle);
//	            return;
//	        }
//	    }
//
//	    /* version matches, DODAG matches */
//	    if (byteorder_ntohs(rpl_dio_buf->rank) == INFINITE_RANK) {
//	        trickle_reset_timer(&my_dodag->trickle);
//	    }
//
//	    /* We are root, all done!*/
//	    if (my_dodag->my_rank == ROOT_RANK) {
//	        if (byteorder_ntohs(rpl_dio_buf->rank) != INFINITE_RANK) {
//	            trickle_increment_counter(&my_dodag->trickle);
//	        }
//
//	        return;
//	    }
//
//	    /*********************  Parent Handling *********************/
//
//	    rpl_parent_t *parent;
//	    parent = rpl_find_parent(my_dodag, &ipv6_buf->srcaddr);
//
//	    if (parent == NULL) {
//	        /* add new parent candidate */
//	        parent = rpl_new_parent(my_dodag, &ipv6_buf->srcaddr, byteorder_ntohs(rpl_dio_buf->rank));
//
//	        if (parent == NULL) {
//	            return;
//	        }
//	    }
//	    else {
//	        /* DIO OK */
//	        trickle_increment_counter(&my_dodag->trickle);
//	    }
//
//	    /* update parent rank */
//	    parent->rank = byteorder_ntohs(rpl_dio_buf->rank);
//	    rpl_parent_update(my_dodag, parent);
//
//	    if (my_dodag->my_preferred_parent == NULL) {
//	        DEBUGF("My dodag has no preferred_parent yet - seems to be odd since I have a parent...\n");
//	    }
//	    else if (rpl_equal_id(&parent->addr, &my_dodag->my_preferred_parent->addr) &&
//	             (parent->dtsn != rpl_dio_buf->dtsn)) {
//	        rpl_delay_dao(my_dodag);
//	    }
//
//	    parent->dtsn = rpl_dio_buf->dtsn;

}

void ng_rpl_recv_DAO(ng_rpl_dao_t* dao)
{
	DEBUG("instance %04X ", dao->rpl_instanceid);
	DEBUG("sequence %04X\n", dao->dao_sequence);

	ng_rpl_dodag_t *dodag = rpl_get_joined_dodag(dao->rpl_instanceid);

	if (dodag == NULL) {
		DEBUG("[Error] got DAO although not a DODAG\n");
		return;
	}

	int len = 4;//DAO_BASE_LEN;
	uint8_t increment_seq = 0;

	//TODO: vedere vecchio rpl_control_messages
}

void ng_rpl_recv_DAO_ACK(ng_rpl_dao_ack_t* dao_ack)
{
	ng_rpl_dodag_t *dodag = rpl_get_joined_dodag(dao_ack->rpl_instanceid);

	if (dodag == NULL) {
		return;
	}

	if (dao_ack->rpl_instanceid != dodag->instance->id) {
		return;
	}

	if (dao_ack->status != 0) {
		return;
	}

	dodag->ack_received = true;
	long_delay_dao(dodag);
}

