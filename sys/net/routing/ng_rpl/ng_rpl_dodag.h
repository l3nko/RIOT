/*
 * ng_rpl_dodag.h
 *
 *  Created on: 27/lug/2015
 *      Author: l3nko
 */

#ifndef NG_RPL_DODAG_H_
#define NG_RPL_DODAG_H_

#include "net/ng_rpl/ng_rpl_types.h"

#ifdef __cplusplus
extern "C" {
#endif

ng_rpl_instance_t* ng_rpl_new_instance(uint8_t instanceid);
ng_rpl_instance_t* ng_rpl_get_instance(uint8_t instanceid);
ng_rpl_dodag_t* ng_rpl_get_joined_dodag(uint8_t instanceid);

#ifdef __cplusplus
}
#endif

#endif /* NG_RPL_DODAG_H_ */
