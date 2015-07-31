/*
 * ng_rpl_dodag.h
 *
 *  Created on: 27/lug/2015
 *      Author: l3nko
 */

#ifndef NG_RPL_DODAG_H_
#define NG_RPL_DODAG_H_

#include "net/ng_rpl/ng_rpl_types.h"
//#include "net/ng_rpl/ng_rpl_config.h"

#ifdef __cplusplus
extern "C" {
#endif

//spstituisce extern rpl_dodags[]
ng_rpl_dodag_t* ng_rpl_get_dodags(void);

/* rpl instances */
ng_rpl_instance_t* ng_rpl_new_instance(uint8_t instanceid);
ng_rpl_instance_t* ng_rpl_get_instance(uint8_t instanceid);
ng_rpl_instance_t* ng_rpl_get_my_instance(void);

/* dodag */
ng_rpl_dodag_t* ng_rpl_new_dodag(ng_rpl_instance_t *inst, ng_ipv6_addr_t *dodagid);
ng_rpl_dodag_t* ng_rpl_get_joined_dodag(uint8_t instanceid);
ng_rpl_dodag_t* ng_rpl_get_my_dodag(void);
void ng_rpl_join_dodag(ng_rpl_dodag_t *dodag, ng_ipv6_addr_t *parent, uint16_t parent_rank);
void ng_rpl_del_dodag(ng_rpl_dodag_t *dodag);
void ng_rpl_leave_dodag(ng_rpl_dodag_t *dodag);

/* parent */
ng_rpl_parent_t* ng_rpl_new_parent(ng_rpl_dodag_t *dodag, ng_ipv6_addr_t *address, uint16_t rank);
ng_rpl_parent_t* ng_rpl_find_parent(ng_rpl_dodag_t *dodag, ng_ipv6_addr_t *address);
ng_ipv6_addr_t* ng_rpl_get_my_preferred_parent(void);
void ng_rpl_delete_parent(ng_rpl_parent_t* parent);
void ng_rpl_delete_worst_parent(void);
void ng_rpl_delete_all_parents(ng_rpl_dodag_t *dodag);
ng_rpl_parent_t* ng_rpl_find_preferred_parent(ng_rpl_dodag_t *my_dodag);
void ng_rpl_parent_update(ng_rpl_dodag_t *my_dodag, ng_rpl_parent_t *parent);

/* utils */
void ng_rpl_global_repair(ng_rpl_dodag_t *my_dodag, ng_ipv6_addr_t *p_addr, uint16_t rank);
void ng_rpl_local_repair(ng_rpl_dodag_t *my_dodag);
bool ng_rpl_equal_id(ng_ipv6_addr_t *id1, ng_ipv6_addr_t *id2);
uint16_t ng_rpl_calc_rank(uint16_t abs_rank, uint16_t minhoprankincrease);

#ifdef __cplusplus
}
#endif

#endif /* NG_RPL_DODAG_H_ */
