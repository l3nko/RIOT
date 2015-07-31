/*
 * ng_rpl_dodag.c
 *
 *  Created on: 27/lug/2015
 *      Author: l3nko
 */

#include "ng_rpl_dodag.h"
#include "net/ng_rpl/ng_rpl_config.h"

#define ENABLE_DEBUG (0)
#include "debug.h"

#if ENABLE_DEBUG
static char addr_str[40]; //[IPV6_MAX_ADDR_STR_LEN];
#endif

static ng_rpl_instance_t instances[RPL_MAX_INSTANCES];
static ng_rpl_dodag_t dodags[RPL_MAX_DODAGS];	//NB. erano extern
static ng_rpl_parent_t parents[RPL_MAX_PARENTS];

ng_rpl_dodag_t* ng_rpl_get_dodags(void)
{
	return &dodags[0];
}

ng_rpl_instance_t* ng_rpl_new_instance(uint8_t instanceid)
{
	ng_rpl_instance_t *inst;
	ng_rpl_instance_t *end ;

	for (inst = &instances[0], end = inst + RPL_MAX_INSTANCES; inst < end; inst++) {
		if (inst->used == 0) {
			memset(inst, 0, sizeof(*inst));
			inst->used = 1;
			inst->id = instanceid;
			return inst;
		}
	}

	return NULL;
}

ng_rpl_instance_t* ng_rpl_get_instance(uint8_t instanceid)
{
	for (int i = 0; i < RPL_MAX_INSTANCES; i++) {
		if (instances[i].used && (instances[i].id == instanceid)) {
			return &instances[i];
		}
	}

	return NULL;
}

ng_rpl_instance_t* ng_rpl_get_my_instance(void)
{
    for (int i = 0; i < RPL_MAX_INSTANCES; i++) {
        if (instances[i].joined) {
            return &instances[i];
        }
    }

    return NULL;
}

ng_rpl_dodag_t* ng_rpl_get_joined_dodag(uint8_t instanceid)
{
	for (int i = 0; i < RPL_MAX_DODAGS; i++) {
		if (dodags[i].joined && dodags[i].instance->id == instanceid) {
			return &dodags[i];
		}
	}

	return NULL;
}

bool ng_rpl_equal_id(ng_ipv6_addr_t *id1, ng_ipv6_addr_t *id2)
{
    for (uint8_t i = 0; i < 2; i++) {
        DEBUGF("ID1: %d ID2: %d\n", id1->u8[12 + i], id2->u8[12 + i]);

        if (id1->u8[14 + i] != id2->u8[14 + i]) {
            return false;
        }
    }

    return true;
}

void ng_rpl_join_dodag(ng_rpl_dodag_t *dodag, ng_ipv6_addr_t *parent, uint16_t parent_rank)
{
    ng_rpl_dodag_t *my_dodag;
    ng_rpl_parent_t *preferred_parent;
    my_dodag = ng_rpl_new_dodag(dodag->instance, &dodag->dodag_id);

    if (my_dodag == NULL) {
        return;
    }

    my_dodag->instance->joined = 1;
    my_dodag->of = dodag->of;
    my_dodag->mop = dodag->mop;
    my_dodag->dtsn = dodag->dtsn;
    my_dodag->prf = dodag->prf;
    my_dodag->dio_interval_doubling = dodag->dio_interval_doubling;
    my_dodag->dio_min = dodag->dio_min;
    my_dodag->dio_redundancy = dodag->dio_redundancy;
    my_dodag->maxrankincrease = dodag->maxrankincrease;
    my_dodag->minhoprankincrease = dodag->minhoprankincrease;
    my_dodag->default_lifetime = dodag->default_lifetime;
    my_dodag->lifetime_unit = dodag->lifetime_unit;
    my_dodag->version = dodag->version;
    my_dodag->grounded = dodag->grounded;
    my_dodag->prefix_length = dodag->prefix_length;
    my_dodag->prefix = dodag->prefix;
    my_dodag->prefix_valid_lifetime = dodag->prefix_valid_lifetime;
    my_dodag->prefix_preferred_lifetime = dodag->prefix_preferred_lifetime;
    my_dodag->prefix_flags = dodag->prefix_flags;
    my_dodag->joined = 1;

    preferred_parent = ng_rpl_new_parent(my_dodag, parent, parent_rank);

    if (preferred_parent == NULL) {
        ng_rpl_del_dodag(my_dodag);
        return;
    }

    my_dodag->my_preferred_parent = preferred_parent;
    my_dodag->node_status = (uint8_t) NORMAL_NODE;
    my_dodag->my_rank = dodag->of->calc_rank(preferred_parent, dodag->my_rank);
    my_dodag->dao_seq = RPL_COUNTER_INIT;
    my_dodag->min_rank = my_dodag->my_rank;

#if ENABLE_DEBUG
    DEBUG("Joint DODAG:\n");
    DEBUG("\tMOP:\t%02X\n", my_dodag->mop);
    DEBUG("\tminhoprankincrease :\t%04X\n", my_dodag->minhoprankincrease);
    DEBUG("\tdefault_lifetime:\t%02X\n", my_dodag->default_lifetime);
    DEBUG("\tgrounded:\t%02X\n", my_dodag->grounded);
    DEBUG("\tmy_preferred_parent:\t%s\n",
          ng_ipv6_addr_to_str(addr_str, 40, //IPV6_MAX_ADDR_STR_LEN,
                           &my_dodag->my_preferred_parent->addr));
    DEBUG("\tmy_preferred_parent rank\t%02X\n", my_dodag->my_preferred_parent->rank);
    DEBUG("\tmy_preferred_parent lifetime\t%04X\n", my_dodag->my_preferred_parent->lifetime);
#endif

    //TODO
//    trickle_start(rpl_process_pid, &my_dodag->trickle, RPL_MSG_TYPE_TRICKLE_INTERVAL,
//                  RPL_MSG_TYPE_TRICKLE_CALLBACK, (1 << my_dodag->dio_min), my_dodag->dio_interval_doubling,
//                  my_dodag->dio_redundancy);
//    rpl_delay_dao(my_dodag);
}
static void ng_rpl_trickle_send_dio(void *args)
{
//    ng_ipv6_addr_t mcast;

    //TODO:
//    ipv6_addr_set_all_rpl_nodes_addr(&mcast);
//    rpl_send_DIO((ng_rpl_dodag_t *) args, &mcast);
}

ng_rpl_dodag_t* ng_rpl_new_dodag(ng_rpl_instance_t *inst, ng_ipv6_addr_t *dodagid)
{
    if (inst == NULL) {
        DEBUGF("Error - No instance specified\n");
        return NULL;
    }
    ng_rpl_dodag_t *dodag;
    ng_rpl_dodag_t *end;

    for (dodag = &dodags[0], end = dodag + RPL_MAX_DODAGS; dodag < end; dodag++) {
        if (dodag->used == 0) {
            memset(dodag, 0, sizeof(*dodag));
            dodag->instance = inst;
            dodag->my_rank = INFINITE_RANK;
            dodag->used = 1;
            dodag->ack_received = true;
            dodag->dao_counter = 0;
            dodag->trickle.callback.func = &ng_rpl_trickle_send_dio;
            dodag->trickle.callback.args = dodag;
            memcpy(&dodag->dodag_id, dodagid, sizeof(*dodagid));
            return dodag;
        }
    }

    return NULL;
}

ng_rpl_dodag_t* ng_rpl_get_my_dodag(void)
{
    for (int i = 0; i < RPL_MAX_DODAGS; i++) {
        if (rpl_dodags[i].joined) {
            return &rpl_dodags[i];
        }
    }

    return NULL;
}

void ng_rpl_del_dodag(ng_rpl_dodag_t *dodag)
{
    ng_rpl_leave_dodag(dodag);
    memset(dodag, 0, sizeof(*dodag));
}

void ng_rpl_leave_dodag(ng_rpl_dodag_t *dodag)
{
    dodag->joined = 0;
    dodag->my_preferred_parent = NULL;
    ng_rpl_delete_all_parents(dodag);
    trickle_stop(&dodag->trickle);
    vtimer_remove(&dodag->dao_timer);
}

void ng_rpl_global_repair(ng_rpl_dodag_t *my_dodag, ng_ipv6_addr_t *p_addr, uint16_t rank)
{
    DEBUGF("[INFO] Global repair started\n");

    if (my_dodag == NULL) {
        DEBUGF("[Error] - no global repair possible, if not part of a DODAG\n");
        return;
    }

    //TODO
    //rpl_delete_all_parents(my_dodag);
    my_dodag->dtsn++;
    //TODO
    //my_dodag->my_preferred_parent = rpl_new_parent(my_dodag, p_addr, rank);

    if (my_dodag->my_preferred_parent == NULL) {
        DEBUGF("[Error] no more parent after global repair\n");
        my_dodag->my_rank = INFINITE_RANK;
    }
    else {
        /* Calc new Rank */
        my_dodag->my_rank = my_dodag->of->calc_rank(my_dodag->my_preferred_parent,
                            my_dodag->my_rank);
        my_dodag->min_rank = my_dodag->my_rank;
        trickle_reset_timer(&my_dodag->trickle);

        //TODO
        //rpl_delay_dao(my_dodag);
    }

    DEBUGF("Migrated to DODAG Version %d. My new Rank: %d\n", my_dodag->version,
           my_dodag->my_rank);
}

ng_rpl_parent_t* ng_rpl_new_parent(ng_rpl_dodag_t *dodag, ng_ipv6_addr_t *address, uint16_t rank)
{
    ng_rpl_parent_t *parent;
    ng_rpl_parent_t *end;

    for (parent = &parents[0], end = parents + RPL_MAX_PARENTS; parent < end; parent++) {
        if (parent->used == 0) {
            memset(parent, 0, sizeof(*parent));
            parent->used = 1;
            parent->addr = *address;
            parent->rank = rank;
            parent->dodag = dodag;
            parent->lifetime = dodag->default_lifetime * dodag->lifetime_unit;
            /* dtsn is set at the end of recv_dio function */
            parent->dtsn = 0;
            return parent;
        }
    }

    ng_rpl_delete_worst_parent();
    return ng_rpl_new_parent(dodag, address, rank);
}

void ng_rpl_delete_worst_parent(void)
{
    uint8_t worst = 0xFF;
    uint16_t max_rank = 0x0000;

    for (int i = 0; i < RPL_MAX_PARENTS; i++) {
        if (parents[i].rank > max_rank) {
            worst = i;
            max_rank = parents[i].rank;
        }
    }

    if (worst == 0xFF) {
        /* Fehler, keine parents -> sollte nicht passieren */
        return;
    }

    ng_rpl_delete_parent(&parents[worst]);

}

void ng_rpl_delete_parent(ng_rpl_parent_t* parent)
{
    if (parent == parent->dodag->my_preferred_parent) {
        parent->dodag->my_preferred_parent = NULL;
    }
    memset(parent, 0, sizeof(*parent));
}

ng_rpl_parent_t* ng_rpl_find_parent(ng_rpl_dodag_t *dodag, ng_ipv6_addr_t *address)
{
    ng_rpl_parent_t *parent;
    ng_rpl_parent_t *end;

    for (parent = &parents[0], end = parents + RPL_MAX_PARENTS; parent < end; parent++) {
        if ((parent->used) && (ng_rpl_equal_id(address, &parent->addr)
                    && (parent->dodag->instance->id == dodag->instance->id)
                    && (!memcmp(&parent->dodag->dodag_id,
                        &dodag->dodag_id, sizeof(ng_ipv6_addr_t))))) {
            return parent;
        }
    }

    return NULL;
}

ng_ipv6_addr_t* ng_rpl_get_my_preferred_parent(void)
{
    ng_rpl_dodag_t* my_dodag = ng_rpl_get_my_dodag();

    if (my_dodag == NULL) {
        return NULL;
    }

    return &my_dodag->my_preferred_parent->addr;
}

void ng_rpl_delete_all_parents(ng_rpl_dodag_t *dodag)
{
    dodag->my_preferred_parent = NULL;
    for (int i = 0; i < RPL_MAX_PARENTS; i++) {
        if (parents[i].dodag && (dodag->instance->id == parents[i].dodag->instance->id) &&
                (!memcmp(&dodag->dodag_id, &parents[i].dodag->dodag_id, sizeof(ng_ipv6_addr_t)))) {
             memset(&parents[i], 0, sizeof(parents[i]));
        }
    }
}

ng_rpl_parent_t* ng_rpl_find_preferred_parent(ng_rpl_dodag_t *my_dodag)
{
    ng_rpl_parent_t *best = NULL;

    if (my_dodag == NULL) {
        DEBUG("Not part of a dodag\n");
        return NULL;
    }

    for (uint8_t i = 0; i < RPL_MAX_PARENTS; i++) {
        if (parents[i].used
                && (parents[i].dodag->instance->id == my_dodag->instance->id)
                && (!memcmp(&parents[i].dodag->dodag_id,
                    &my_dodag->dodag_id, sizeof(ng_ipv6_addr_t)))) {
            if ((parents[i].rank == INFINITE_RANK) || (parents[i].lifetime <= 1)) {
                DEBUG("Infinite rank, bad parent\n");
                continue;
            }
            else if (best == NULL) {
                DEBUG("possible parent\n");
                best = &parents[i];
            }
            else {
                best = my_dodag->of->which_parent(best, &parents[i]);
            }
        }
    }

    if (best == NULL) {
        return NULL;
    }

    if (my_dodag->my_preferred_parent == NULL) {
        my_dodag->my_preferred_parent = best;
    }

    if (!ng_rpl_equal_id(&my_dodag->my_preferred_parent->addr, &best->addr)) {
        if (my_dodag->mop != NG_RPL_MOP_NO_DOWNWARD_ROUTES) {
            /* send DAO with ZERO_LIFETIME to old parent */
        	//TODO
//        	rpl_send_DAO(my_dodag, &my_dodag->my_preferred_parent->addr, 0, false, 0);
        }

        my_dodag->my_preferred_parent = best;

        if (my_dodag->mop != NG_RPL_MOP_NO_DOWNWARD_ROUTES) {
        	//TODO
//            rpl_delay_dao(my_dodag);
        }

        trickle_reset_timer(&my_dodag->trickle);
    }

    return best;
}

uint16_t ng_rpl_calc_rank(uint16_t abs_rank, uint16_t minhoprankincrease)
{
    return abs_rank / minhoprankincrease;
}

void ng_rpl_parent_update(ng_rpl_dodag_t *my_dodag, ng_rpl_parent_t *parent)
{
    uint16_t old_rank;

    if (my_dodag == NULL) {
        DEBUG("Not part of a dodag - this should not happen");
        return;
    }

    old_rank = my_dodag->my_rank;

    /* update Parent lifetime */
    if (parent != NULL) {
        parent->lifetime = my_dodag->default_lifetime * my_dodag->lifetime_unit;
    }

    if (ng_rpl_find_preferred_parent(my_dodag) == NULL) {
        ng_rpl_local_repair(my_dodag);
    }

    if (ng_rpl_calc_rank(old_rank, my_dodag->minhoprankincrease) !=
        ng_rpl_calc_rank(my_dodag->my_rank, my_dodag->minhoprankincrease)) {
        if (my_dodag->my_rank < my_dodag->min_rank) {
            my_dodag->min_rank = my_dodag->my_rank;
        }

        trickle_reset_timer(&my_dodag->trickle);
    }
}

void ng_rpl_local_repair(ng_rpl_dodag_t *my_dodag)
{
    DEBUGF("[INFO] Local Repair started\n");

    if (my_dodag == NULL) {
        DEBUGF("[Error] - no local repair possible, if not part of a DODAG\n");
        return;
    }

    my_dodag->my_rank = INFINITE_RANK;
    my_dodag->dtsn++;
    ng_rpl_delete_all_parents(my_dodag);
    trickle_reset_timer(&my_dodag->trickle);

}
