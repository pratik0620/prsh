#include<iostream>
#include<signal.h>

#include "signal_handler.h"

void handle_sigtstp(int sig) {
    std::cout << "\nStopped Process" << std::endl;
};

void handle_sigcont(int sig) {
    std::cout << "\nRestart Process" << std::endl;
}

void handle_signal() {
    struct sigaction stop_sa{};
    stop_sa.sa_handler = &handle_sigtstp;
    stop_sa.sa_flags = SA_RESTART;
    if (sigaction(SIGTSTP, &stop_sa, NULL) == -1) {
        perror("sigaction");
    }

    struct sigaction cont_sa{};
    cont_sa.sa_handler = &handle_sigcont;
    cont_sa.sa_flags = SA_RESTART;
    if (sigaction(SIGCONT, &cont_sa, NULL) == -1) {
        perror("sigaction");
    }
}