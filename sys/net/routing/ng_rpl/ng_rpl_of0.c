/*
 * ng_rpl_of0.c
 *
 *  Created on: 27/lug/2015
 *      Author: l3nko
 */

#include "ng_rpl_of0.h"

//Function Prototypes
static uint16_t calc_rank(ng_rpl_parent_t *, uint16_t);
static ng_rpl_parent_t *which_parent(ng_rpl_parent_t *, ng_rpl_parent_t *);
static ng_rpl_dodag_t *which_dodag(ng_rpl_dodag_t *, ng_rpl_dodag_t *);
static void reset(ng_rpl_dodag_t *);

static ng_rpl_of_t ng_rpl_of0 = {
    0x0,				//ocp
    calc_rank,
    which_parent,
    which_dodag,
    reset,
    NULL,				//parent_state_callback
    NULL,				//init
    NULL				//process_dio
};


ng_rpl_of_t *ng_rpl_get_of0(void)
{
	return &ng_rpl_of0;
}

void reset(ng_rpl_dodag_t *dodag)
{
    /* Nothing to do in OF0 */
    (void) dodag;
}

uint16_t calc_rank(ng_rpl_parent_t *parent, uint16_t base_rank)
{
    if (base_rank == 0) {
        if (parent == NULL) {
            return 0xFFFF; //INFINITE_RANK;
        }

        base_rank = parent->rank;
    }

    uint16_t add;

    if (parent != NULL) {
        add = parent->dodag->minhoprankincrease;
    }
    else {
        add = 256; //DEFAULT_MIN_HOP_RANK_INCREASE;
    }

    if (base_rank + add < base_rank) {
        return 0xFFFF; //INFINITE_RANK;
    }

    return base_rank + add;
}

/* We simply return the Parent with lower rank */
ng_rpl_parent_t *which_parent(ng_rpl_parent_t *p1, ng_rpl_parent_t *p2)
{
    if (p1->rank < p2->rank) {
        return p1;
    }

    return p2;
}

/* Not used yet, as the implementation only makes use of one dodag for now. */
ng_rpl_dodag_t *which_dodag(ng_rpl_dodag_t *d1, ng_rpl_dodag_t *d2)
{
    (void) d2;
    return d1;
}
