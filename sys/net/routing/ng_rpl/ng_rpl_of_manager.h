/*
 * rpl_of_manager.h
 *
 *  Created on: 27/lug/2015
 *      Author: l3nko
 */

#ifndef NG_RPL_OF_MANAGER_H_
#define NG_RPL_OF_MANAGER_H_

#include "net/ng_rpl/ng_rpl_types.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief Initialization of Manager and of-functions.
 * @param[in]   my_address Own address for initialization of beaconing
*/
void ng_rpl_of_manager_init(ng_ipv6_addr_t *my_address);

/**
 * @brief Returns objective function with a given cope point
 * @param[in]   ocp Objective code point of objective function
 * @return      Pointer of corresponding objective function implementation
*/
ng_rpl_of_t* ng_rpl_get_of_for_ocp(uint16_t ocp);


#ifdef __cplusplus
}
#endif

#endif /* NG_RPL_OF_MANAGER_H_ */
