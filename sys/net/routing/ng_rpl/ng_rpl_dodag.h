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

//static inline uint8_t NG_RPL_COUNTER_INCREMENT(uint8_t counter)
//{
//    return (counter > RPL_COUNTER_LOWER_REGION ? (counter == RPL_COUNTER_MAX ? counter = 0 : ++counter) : (counter == RPL_COUNTER_LOWER_REGION ? counter = 0 : ++counter));
//}
//
//static inline bool NG_RPL_COUNTER_IS_INIT(uint8_t counter)
//{
//    return (counter > RPL_COUNTER_LOWER_REGION);
//}
//
//static inline bool NG_RPL_COUNTER_GREATER_THAN_LOCAL(uint8_t A, uint8_t B)
//{
//    return (((A < B) && (RPL_COUNTER_LOWER_REGION + 1 - B + A < RPL_COUNTER_SEQ_WINDOW)) || ((A > B) && (A - B < RPL_COUNTER_SEQ_WINDOW)));
//}
//
//static inline bool NG_RPL_COUNTER_GREATER_THAN(uint8_t A, uint8_t B)
//{
//    return ((A > RPL_COUNTER_LOWER_REGION) ? ((B > RPL_COUNTER_LOWER_REGION) ? NG_RPL_COUNTER_GREATER_THAN_LOCAL(A, B) : 0) : ((B > RPL_COUNTER_LOWER_REGION) ? 1 : NG_RPL_COUNTER_GREATER_THAN_LOCAL(A, B)));
//}

//spstituisce extern rpl_dodag[]
ng_rpl_dodag_t* ng_rpl_get_dodags(void);

ng_rpl_instance_t* ng_rpl_new_instance(uint8_t instanceid);
ng_rpl_instance_t* ng_rpl_get_instance(uint8_t instanceid);
ng_rpl_dodag_t* ng_rpl_get_joined_dodag(uint8_t instanceid);

bool ng_rpl_equal_id(ng_ipv6_addr_t *id1, ng_ipv6_addr_t *id2);

void ng_rpl_join_dodag(ng_rpl_dodag_t *dodag, ng_ipv6_addr_t *parent, uint16_t parent_rank)
void ng_rpl_global_repair(ng_rpl_dodag_t *my_dodag, ng_ipv6_addr_t *p_addr, uint16_t rank);

ng_rpl_parent_t* ng_rpl_find_parent(ng_rpl_dodag_t *dodag, ng_ipv6_addr_t *address);
ng_rpl_parent_t* ng_rpl_new_parent(ng_rpl_dodag_t *dodag, ng_ipv6_addr_t *address, uint16_t rank);
void rpl_delete_parent(ng_rpl_parent_t* parent);
void ng_rpl_delete_worst_parent(void);

#ifdef __cplusplus
}
#endif

#endif /* NG_RPL_DODAG_H_ */
