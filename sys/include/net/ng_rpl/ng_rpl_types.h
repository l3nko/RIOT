/*
 * ng_rpl_types.h
 *
 *  Created on: 24/lug/2015
 *      Author: l3nko
 */

#ifndef NG_RPL_TYPES_H_
#define NG_RPL_TYPES_H_

/*
 * Copyright (C) 2013  INRIA.
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup rpl
 * @{
 * @file
 * @brief   RPL data types
 *
 * File, which defines all structs used by RPL.
 *
 * @author  Eric Engel <eric.engel@fu-berlin.de>
 * @}
 */

#include <string.h>
#include "net/ng_ipv6.h"
#include "trickle.h"

#ifdef __cplusplus
extern "C" {
#endif

/* Modes of Operation */

/* DIO Base Object (RFC 6550 Fig. 14) */
typedef struct __attribute__((packed)) {
    uint8_t rpl_instanceid;
    uint8_t version_number;
    network_uint16_t rank;
    uint8_t g_mop_prf;
    uint8_t dtsn;
    uint8_t flags;
    uint8_t reserved;
    ng_ipv6_addr_t dodagid;
} ng_rpl_dio_t;

typedef struct __attribute__((packed)) {
    uint8_t flags;
    uint8_t reserved;
} ng_rpl_dis_t;

/* DAO Base Object (RFC 6550 Fig. 16) */
typedef struct __attribute__((packed)) {
    uint8_t rpl_instanceid;
    uint8_t k_d_flags;
    uint8_t reserved;
    uint8_t dao_sequence;
} ng_rpl_dao_t;

/* DAO ACK Base Object (RFC 6550 Fig. 17.) */
typedef struct __attribute__((packed)) {
    uint8_t rpl_instanceid;
    uint8_t d_reserved;
    uint8_t dao_sequence;
    uint8_t status;
} ng_rpl_dao_ack_t;

///* DODAG ID Struct */
///* may be present in dao or dao_ack packets */
//typedef struct __attribute__((packed)) {
//    ng_ipv6_addr_t dodagid;
//} ng_dodag_id_t;

/* RPL-Option Generic Format (RFC 6550 Fig. 19) */
typedef struct __attribute__((packed)) {
    uint8_t type;
    uint8_t length;
} ng_rpl_opt_t;

/* DODAG Configuration-Option (RFC 6550 Fig. 24) */
typedef struct __attribute__((packed)) {
    uint8_t type;
    uint8_t length;
    uint8_t flags_a_pcs;
    uint8_t DIOIntDoubl;
    uint8_t DIOIntMin;
    uint8_t DIORedun;
    network_uint16_t MaxRankIncrease;
    network_uint16_t MinHopRankIncrease;
    network_uint16_t ocp;
    uint8_t reserved;
    uint8_t default_lifetime;
    network_uint16_t lifetime_unit;
} ng_rpl_opt_dodag_conf_t;

/* RPL Solicited Information Option (RFC 6550 Fig. 28) */
typedef struct __attribute__((packed)) {
    uint8_t type;
    uint8_t length;
    uint8_t rplinstanceid;
    uint8_t VID_Flags;
    ng_ipv6_addr_t dodagid;
    uint8_t version;
} ng_rpl_opt_solicited_t;

/* RPL Target-Option (RFC 6550 Fig. 25) */
/* TODO: ipv6_addr_t target may be replaced by a target prefix of variable length */
typedef struct __attribute__((packed)) {
    uint8_t type;
    uint8_t length;
    uint8_t flags;
    uint8_t prefix_length;
    ng_ipv6_addr_t target;
} ng_rpl_opt_target_t;

/* RPL Transit-Option (RFC 6550 Fig. 26) */
typedef struct __attribute__((packed)) {
    uint8_t type;
    uint8_t length;
    uint8_t e_flags;
    uint8_t path_control;
    uint8_t path_sequence;
    uint8_t path_lifetime;
    ng_ipv6_addr_t parent;
} ng_rpl_opt_transit_t;

/* RPL Prefix Information Option (RFC 6550 Fig. 29) */
typedef struct __attribute__((packed)) {
    uint8_t type;
    uint8_t length;
    uint8_t prefix_length;
/* RFC 6550 https://tools.ietf.org/html/rfc6550#page-61 */
#define RPL_PREFIX_INFO_ROUTER_ADDRESS      (1 << 5)
#define RPL_PREFIX_INFO_AUTO_ADDR_CONF      (1 << 6)
#define RPL_PREFIX_INFO_ON_LINK             (1 << 7)
    uint8_t flags;
    network_uint32_t valid_lifetime;
    network_uint32_t preferred_lifetime;
    network_uint32_t reserved2;
    ng_ipv6_addr_t prefix;
} ng_rpl_opt_prefix_information_t;

struct rpl_dodag_t;

typedef struct {
	ng_ipv6_addr_t         	addr;
    uint16_t            	rank;
    uint8_t             	dtsn;
    struct ng_rpl_dodag_t*	dodag;
    uint16_t            	lifetime;
    double              	link_metric;
    uint8_t             	link_metric_type;
    uint8_t             	used;
} ng_rpl_parent_t;

struct ng_rpl_of_t;

typedef struct {
    uint8_t id;
    uint8_t used;
    uint8_t joined;
} ng_rpl_instance_t;

//Node-internal representation of a DODAG, with nodespecific information
typedef struct ng_rpl_dodag_t {
	ng_rpl_instance_t *instance;
	ng_ipv6_addr_t dodag_id;
    uint8_t used;
    uint8_t mop;
    uint8_t dtsn;
    uint8_t prf;
    uint8_t dio_interval_doubling;
    uint8_t dio_min;
    uint8_t dio_redundancy;
    uint16_t maxrankincrease;
    uint16_t minhoprankincrease;
    uint8_t default_lifetime;
    uint16_t lifetime_unit;
    uint8_t version;
    uint8_t grounded;
    uint16_t my_rank;
    uint8_t node_status;
    uint8_t dao_seq;
    uint16_t min_rank;
    uint8_t joined;
    ng_rpl_parent_t *my_preferred_parent;
    struct ng_rpl_of_t *of;
    trickle_t trickle;
    bool ack_received;
    uint8_t dao_counter;
    ng_ipv6_addr_t prefix;
    uint8_t prefix_length;
    uint32_t prefix_valid_lifetime;
    uint32_t prefix_preferred_lifetime;
    uint8_t prefix_flags;
    timex_t dao_time;
    vtimer_t dao_timer;
} ng_rpl_dodag_t;

typedef struct ng_rpl_of_t {
    uint16_t ocp;
    uint16_t (*calc_rank)(ng_rpl_parent_t *parent, uint16_t base_rank);
    ng_rpl_parent_t *(*which_parent)(ng_rpl_parent_t *, ng_rpl_parent_t *);
    ng_rpl_dodag_t *(*which_dodag)(ng_rpl_dodag_t *, ng_rpl_dodag_t *);
    void (*reset)(ng_rpl_dodag_t *);
    void (*parent_state_callback)(ng_rpl_parent_t *, int, int);
    void (*init)(void);  //OF specific init function
    void (*process_dio)(void);  //DIO processing callback (acc. to OF0 spec, chpt 5)
} ng_rpl_of_t;

typedef struct {
    ng_ipv6_addr_t address;
    ng_ipv6_addr_t next_hop;
    uint16_t lifetime;
    uint8_t used;
} ng_rpl_routing_entry_t;

/* Parameters passed to RPL on initialization */
typedef struct {
    uint8_t instance_id;
    ng_ipv6_addr_t prefix;
    uint8_t prefix_len;
    uint8_t prefix_flags;
    uint32_t prefix_valid_lifetime;
    uint32_t prefix_preferred_lifetime;
} ng_rpl_options_t;

#ifdef __cplusplus
}
#endif

#endif /* NG_RPL_TYPES_H_ */
