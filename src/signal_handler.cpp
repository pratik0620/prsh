#include<iostream>
#include<signal.h>
#include<unistd.h>

#include "signal_handler.h"

volatile sig_atomic_t g_interrupted = 0;

void handle_sigint(int sig) {
    g_interrupted = 1;
    const char msg[] = "\n";
    write(STDOUT_FILENO, msg, sizeof(msg) - 1);
}

void handle_signal() {
    struct sigaction int_sa{};
    int_sa.sa_handler = &handle_sigint;
    int_sa.sa_flags = SA_RESTART;
    sigemptyset(&int_sa.sa_mask);
    if (sigaction(SIGINT, &int_sa, NULL) == -1) {
        perror("sigaction");
    }
}