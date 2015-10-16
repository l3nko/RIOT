/*
 * auto_init_cc2420.c
 *
 *  Created on: 06/ott/2015
 *      Author: l3nko
 */

 /**
  * @ingroup auto_init_gnrc_netif
  * @{
  *
  * @file
  * @brief   Auto initialization for CC2420 network interfaces
  *
  * @author  Kaspar Schleiser <kaspar@schleiser.de>
  */

#ifdef MODULE_CC2420

#include "board.h"
#include "net/gnrc/nomac.h"
#include "net/gnrc.h"

#include "cc2420.h"
#include "cc2420_params.h"

#define ENABLE_DEBUG (0)
#include "debug.h"

#define CC2420_NUM (sizeof(cc2420_params)/sizeof(cc2420_params_t))

static cc2420_t cc2420_devs[CC2420_NUM];

/**
 * @brief   Define stack parameters for the MAC layer thread
 * @{
 */
#define CC2420_MAC_STACKSIZE           (THREAD_STACKSIZE_MAIN)//(THREAD_STACKSIZE_DEFAULT)
#define CC2420_MAC_PRIO                (THREAD_PRIORITY_MAIN - 4)

/**
 * @brief   Stacks for the MAC layer threads
 */
static char _nomac_stacks[CC2420_MAC_STACKSIZE][CC2420_NUM];

void auto_init_cc2420(void)
{
  for (int i = 0; i < CC2420_NUM; i++) {
    const cc2420_params_t *p = &cc2420_params[i];
    DEBUG("Initializing CC2420 radio at SPI_%i\n", p->spi);
    int res = cc2420_init(&cc2420_devs[i], p);

    if (res < 0) {
        DEBUG("Error initializing CC2420 radio device!");
    }
    else {
        gnrc_nomac_init(_nomac_stacks[i],
                        CC2420_MAC_STACKSIZE, CC2420_MAC_PRIO, "cc2420",
                        (gnrc_netdev_t *)&cc2420_devs[i]);
    }
  }
}

#else
typedef int dont_be_pedantic;
#endif /* MODULE_CC2420*/
/** @} */
