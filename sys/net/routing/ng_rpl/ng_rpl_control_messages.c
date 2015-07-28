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

#if ENABLE_DEBUG
/* For PRIu16 etc. */
#include <inttypes.h>
#endif

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
static uint8_t* get_rpl_opt_from_buf(uint8_t* buffer, uint16_t len)
{
	return ((uint8_t*) &(buffer[IPV6_HDR_LEN + ICMPV6_HDR_LEN + len]) );
}

void ng_rpl_recv_DIS(ng_rpl_dis_t* dis, size_t data_size)
{
	uint16_t len = DIS_BASE_LEN;
	ng_rpl_dodag_t *dodag, *end;
	uint8_t options_missing = 1;
	ng_rpl_opt_t *dis_opt = NULL;

	while (len < data_size - ICMPV6_HDR_LEN) {
		dis_opt = (ng_rpl_opt_t*)get_rpl_opt_from_buf(dis, len);

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

				ng_rpl_opt_solicited_t* opt_solicited = (ng_rpl_opt_solicited_t*)get_rpl_opt_from_buf(dis, len);
				for (dodag = ng_rpl_get_dodags(), end = dodag + RPL_MAX_DODAGS; dodag < end; dodag++) {
					if (dodag->joined) {
						if (opt_solicited->VID_Flags & RPL_DIS_I_MASK) {
							if (dodag->instance->id != opt_solicited->rplinstanceid) {
								continue;
							}
						}

						if (opt_solicited->VID_Flags & RPL_DIS_D_MASK) {
							if (!rpl_equal_id(&dodag->dodag_id, &opt_solicited->dodagid)) {
								continue;
							}
						}

						if (opt_solicited->VID_Flags & RPL_DIS_V_MASK) {
							if (dodag->version != opt_solicited->version) {
								continue;
							}
						}

						//TODO: send_DIO
						//rpl_send_DIO(dodag, &ipv6_buf->srcaddr);
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
				//TODO: send_DIO
				//rpl_send_DIO(dodag, &ipv6_buf->srcaddr);
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
		dio_opt = (ng_rpl_opt_t*)get_rpl_opt_from_buf(dio, len);

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

				ng_rpl_opt_dodag_conf_t* opt_dodag_conf = (ng_rpl_opt_dodag_conf_t*)get_rpl_opt_from_buf(dio, len);
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

				ng_rpl_opt_prefix_information_t* opt_prefix_info = (ng_rpl_opt_prefix_information_t*)get_rpl_opt_from_buf(dio, len);
				/* autonomous address-configuration flag */
				if (opt_prefix_info->flags & (1 << 6)) {
					ipv6_addr_t tmp;
					tmp = opt_prefix_info->prefix;
					if (!ipv6_addr_is_link_local(&tmp)) {
						if (byteorder_ntohl(opt_prefix_info->preferred_lifetime)
								<= byteorder_ntohl(opt_prefix_info->valid_lifetime)) {
							ipv6_addr_set_by_eui64(&tmp, rpl_if_id, &tmp);
							ipv6_net_if_add_addr(rpl_if_id, &tmp,
									NDP_ADDR_STATE_PREFERRED,
									byteorder_ntohl(opt_prefix_info->valid_lifetime),
									byteorder_ntohl(opt_prefix_info->preferred_lifetime),
									0);
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
			//TODO: rpl send DIS
			//rpl_send_DIS(&ipv6_buf->srcaddr);
		}

		if (byteorder_ntohs(dio->rank) < ROOT_RANK) {
			DEBUGF("DIO with Rank < ROOT_RANK\n");
		}

		if (dio_dodag.mop != RPL_DEFAULT_MOP) {
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
		else if (RPL_COUNTER_GREATER_THAN(my_dodag->version, dio_dodag.version)) {
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
    	dao_opt = (ng_rpl_opt_t*)get_rpl_opt_from_buf(dio, len);

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
            	ng_rpl_opt_target_t* opt_target = (ng_rpl_opt_target_t*)get_rpl_opt_from_buf(dio, len);
                if (opt_target->prefix_length != RPL_DODAG_ID_LEN) {
                    DEBUGF("prefixes are not supported yet\n");
                    break;
                }

                len += (opt_target->length + RPL_OPT_LEN);
                ng_rpl_opt_transit_t* opt_transit = (ng_rpl_opt_transit_t*)get_rpl_opt_from_buf(dio, len);

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

                //TODO: ng_rpl_add_routing_entry
                rpl_add_routing_entry(&opt_target->target, &ipv6_hdr->src,
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
		return;
	}

	if (dao_ack->rpl_instanceid != dodag->instance->id) {
		return;
	}

	if (dao_ack->status != 0) {
		return;
	}

	dodag->ack_received = true;
	//TODO: long_delay_dao(dodag);
}

