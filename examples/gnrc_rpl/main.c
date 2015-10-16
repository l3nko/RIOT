/*
 * Copyright (C) 2015 Freie Universität Berlin
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     examples
 * @{
 *
 * @file
 * @brief       Example application for demonstrating the RIOT network stack
 *
 * @author      Hauke Petersen <hauke.petersen@fu-berlin.de>
 *
 * @}
 */

#include <stdio.h>
// #include <stdlib.h>

#include "shell.h"

// ps and reboot command
#include "ps.h"
#include "kernel.h"


int my_ps_handler(int argc, char **argv)
{
    (void) argc; (void) argv;
    ps();
    // printf("NOT LOADED!!!\n");
    return 0;
}

int my_reboot_handler(int argc, char **argv)
{
    (void) argc; (void) argv;
    (void) reboot(RB_AUTOBOOT);
    return 0;
}

extern kernel_pid_t gnrc_rpl_pid;
extern kernel_pid_t gnrc_rpl_init(kernel_pid_t if_pid);
int my_rpl_handler(int argc, char **argv)
{
  kernel_pid_t iface_pid = (kernel_pid_t) argv[1][0] - '0';
  gnrc_rpl_init(iface_pid);
  printf("RPL initialized on interface %d with pid %d\n", iface_pid, gnrc_rpl_pid);
  return 0;
}

static const shell_command_t shell_commands[] = {
    {"reboot", "Reboot the node", my_reboot_handler},
	{"ps", "Prints information about running threads", my_ps_handler},
    {"rpl", "init RPL protocol (usage: rpl [PID])", my_rpl_handler },
    { NULL, NULL, NULL }
};

int main(void)
{
    // kernel_pid_t pids[GNRC_NETIF_NUMOF];
    // size_t numof = gnrc_netif_get(pids);
    //
    // for (size_t i = 0; i < numof && i < GNRC_NETIF_NUMOF; i++) {
    //     gnrc_rpl_init(pids[i]);
    //     printf("Successfully initialized RPL on interface %d\n", pids[i]);
    // }

    /* start shell */
    puts("All up, running the shell now");
    char line_buf[SHELL_DEFAULT_BUFSIZE];
    shell_run(shell_commands, line_buf, SHELL_DEFAULT_BUFSIZE);

    /* should be never reached */
    return 0;
}
