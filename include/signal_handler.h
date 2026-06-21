#ifndef SIGNAL_HANDLER_H
#define SIGNAL_HANDLER_H

extern volatile sig_atomic_t g_interrupted;

void handle_signal();
void handle_sigtstp(int);
void handle_sigint(int);

#endif
