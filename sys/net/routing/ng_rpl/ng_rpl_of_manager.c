/*
 * ng_rpl_of_manager.c
 *
 *  Created on: 27/lug/2015
 *      Author: l3nko
 */

#include "ng_rpl_of_manager.h"
#include "net/ng_rpl/ng_rpl_config.h"
#include "ng_rpl_of0.h"

#define ENABLE_DEBUG (0)
#include "debug.h"

static ng_rpl_of_t* objective_functions[NUMBER_IMPLEMENTED_OFS];

void ng_rpl_of_manager_init(ng_ipv6_addr_t *my_address)
{
    /* insert new objective functions here */
	for(uint16_t i=0; i<NUMBER_IMPLEMENTED_OFS; i++) {
		objective_functions[0] = ng_rpl_get_of0();	//OSS: only of0 implemented
	}

//    objective_functions[0] = rpl_get_of0();
//    objective_functions[1] = rpl_get_of_mrhof();

//    if (RPL_DEFAULT_OCP == 1) {
//        DEBUG("%s, %d: INIT ETX BEACONING\n", __FILE__, __LINE__);
//        etx_init_beaconing(my_address);
//    }
}

ng_rpl_of_t* ng_rpl_get_of_for_ocp(uint16_t ocp)
{
	for (uint16_t i = 0; i < NUMBER_IMPLEMENTED_OFS; i++) {
		if (objective_functions[i] == NULL) {
			/* fallback if something goes wrong */
			return ng_rpl_get_of0();
		}
		else if (ocp == objective_functions[i]->ocp) {
			return objective_functions[i];
		}
	}

	return NULL;

}

