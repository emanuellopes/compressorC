/**
* @file signal_handler.c
* @brief Handle signal
* @date 2015-12-15
* @author 2140825@ipleiria.pt
*/
#include <stdio.h>
#include <time.h>
#include <signal.h>


#include "errors.h"
#include "debug.h"
#include "signal_handler.h"

/**
 * global far for signal sigaction
 */
int running = 1;

static void handler(int);

/**
* @param signal
* @return nothing
*/
void handler(int signal) {
    (void) signal;
    running = 0;
}

/**
 * @brief signal para o ctrl+c
 */
void install_signal_handler(void) {
    struct  sigaction config;
    config.sa_handler = handler;
    sigemptyset(&config.sa_mask);
    config.sa_flags = SA_RESTART;
    if (sigaction(SIGINT, &config, NULL) != 0) {
        ERROR(C_ERR_SYS_CALL, "sigaction failed");
    }
}

/**
 * @brief return hour and date of operation interrupted
 */
void getDate(void) {
    time_t rawtime;
    struct tm *info;
    char buffer[80];
    time(&rawtime);
    info = localtime( &rawtime );
    strftime(buffer, 80, "%Y-%m-%d %Hh%M", info);
    fprintf(stderr,"[SIGINT] - Operation interrupted by user @%s\n", buffer);
}