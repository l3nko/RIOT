/*
 * ng_rpl_config.h
 *
 *  Created on: 27/lug/2015
 *      Author: l3nko
 */

#ifndef NG_RPL_CONFIG_H_
#define NG_RPL_CONFIG_H_


#ifdef __cplusplus
extern "C" {
#endif

/*  Default values */
#define NG_RPL_MOP_NO_DOWNWARD_ROUTES  0x00
#define NG_RPL_MOP_NON_STORING_MODE    0x01
#define NG_RPL_MOP_STORING_MODE_NO_MC  0x02
#define NG_RPL_MOP_STORING_MODE_MC     0x03

/*  RPL Constants and Variables */
#ifndef NG_RPL_DEFAULT_MOP
	#define NG_RPL_DEFAULT_MOP NG_RPL_MOP_STORING_MODE_NO_MC
#endif

#define NG_RPL_SEQUENCE_WINDOW         16

/* Node Status */
#define NORMAL_NODE  0
#define ROOT_NODE    1
#define LEAF_NODE    2

#define BASE_RANK 0
#define INFINITE_RANK 0xFFFF
#define RPL_DEFAULT_INSTANCE 0
#define DEFAULT_PATH_CONTROL_SIZE 0
#define DEFAULT_DIO_INTERVAL_MIN 11

#define ICMPV6_HDR_LEN              (0x4)
#define IPV6_HDR_LEN                (0x28)
/* packet base lengths */
#define DIO_BASE_LEN                            24
#define DIS_BASE_LEN                            2
#define DAO_BASE_LEN                            4
#define DAO_D_LEN                               24
#define DAO_ACK_LEN                             4
#define DAO_ACK_D_LEN                           24
#define RPL_OPT_LEN                             2
#define RPL_OPT_DODAG_CONF_LEN                  14
#define RPL_OPT_DODAG_CONF_LEN_WITH_OPT_LEN     (RPL_OPT_DODAG_CONF_LEN + RPL_OPT_LEN)
#define RPL_OPT_PREFIX_INFO_LEN                 30
#define RPL_OPT_PREFIX_INFO_LEN_WITH_OPT_LEN    (RPL_OPT_PREFIX_INFO_LEN + RPL_OPT_LEN)
#define RPL_OPT_SOLICITED_INFO_LEN              21
#define RPL_OPT_TARGET_LEN                      18
#define RPL_OPT_TARGET_LEN_WITH_OPT_LEN         (RPL_OPT_TARGET_LEN + RPL_OPT_LEN)
#define RPL_OPT_TRANSIT_LEN                     20
#define RPL_OPT_TRANSIT_LEN_WITH_OPT_LEN        (RPL_OPT_TRANSIT_LEN + RPL_OPT_LEN)

/* message options */
#define RPL_OPT_PAD1                 0
#define RPL_OPT_PADN                 1
#define RPL_OPT_DAG_METRIC_CONTAINER 2
#define RPL_OPT_ROUTE_INFO           3
#define RPL_OPT_DODAG_CONF           4
#define RPL_OPT_TARGET               5
#define RPL_OPT_TRANSIT              6
#define RPL_OPT_SOLICITED_INFO       7
#define RPL_OPT_PREFIX_INFO          8
#define RPL_OPT_TARGET_DESC          9

/* Counters */
#define RPL_COUNTER_MAX                 255
#define RPL_COUNTER_LOWER_REGION        127
#define RPL_COUNTER_SEQ_WINDOW          16
#define RPL_COUNTER_INIT                (RPL_COUNTER_MAX - RPL_COUNTER_SEQ_WINDOW + 1)

/* standard value: */
/* #define DEFAULT_DIO_INTERVAL_MIN 3 */
#define DEFAULT_DIO_INTERVAL_DOUBLINGS 7
/* standard value: */
/* #define DEFAULT_DIO_INTERVAL_DOUBLINGS 20 */
#define DEFAULT_DIO_REDUNDANCY_CONSTANT 10
#define DEFAULT_MIN_HOP_RANK_INCREASE 256
#define ROOT_RANK DEFAULT_MIN_HOP_RANK_INCREASE
/* DAO_DELAY is in seconds */
#define DEFAULT_DAO_DELAY 3
#define REGULAR_DAO_INTERVAL 300
#define DAO_SEND_RETRIES 4
#define DEFAULT_WAIT_FOR_DAO_ACK 15
#define RPL_DODAG_ID_LEN 128

/* others */
#define NUMBER_IMPLEMENTED_OFS 1//2
#define RPL_MAX_DODAGS 2//3
#define RPL_MAX_INSTANCES 1
#define RPL_MAX_PARENTS 5
#ifndef RPL_MAX_ROUTING_ENTRIES
    #if (NG_RPL_DEFAULT_MOP == NG_RPL_MOP_NO_DOWNWARD_ROUTES)
    	#define NG_RPL_MAX_ROUTING_ENTRIES (128)
    #elif (NG_RPL_DEFAULT_MOP == NG_RPL_MOP_NON_STORING_MODE)
        #ifdef RPL_NODE_IS_ROOT
        	#define RPL_MAX_ROUTING_ENTRIES (128)
        #else
        	#define RPL_MAX_ROUTING_ENTRIES (0)
        #endif
    #elif (NG_RPL_DEFAULT_MOP == NG_RPL_MOP_STORING_MODE_NO_MC)
    	#define RPL_MAX_ROUTING_ENTRIES (32)//(128)				//OSS: ogni entry è 36 byte
    #else // NG_RPL_DEFAULT_MOP == NG_RPL_MOP_STORING_MODE_MC
    	#define RPL_MAX_ROUTING_ENTRIES (128)
    #endif
#endif
#define RPL_MAX_SRH_PATH_LENGTH 10;
#define RPL_SRH_ENTRIES 15
#define RPL_ROOT_RANK 256
#define RPL_DEFAULT_LIFETIME 0xff
#define RPL_LIFETIME_UNIT 2
#define RPL_LIFETIME_STEP 2
#define RPL_GROUNDED 1
#define RPL_PRF_MASK 0x7
#define RPL_MOP_SHIFT 3
#define RPL_SHIFTED_MOP_MASK 0x7
#define RPL_DIS_V_MASK 0x80
#define RPL_DIS_I_MASK 0x40
#define RPL_DIS_D_MASK 0x20
#define RPL_GROUNDED_SHIFT 7
#define RPL_DEFAULT_OCP 0

#ifdef __cplusplus
}
#endif

#endif /* NG_RPL_CONFIG_H_ */
