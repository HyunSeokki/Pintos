#ifndef USERPROG_PROCESS_H
#define USERPROG_PROCESS_H

#include "threads/thread.h"

tid_t process_execute (const char *file_name);
int process_wait (tid_t);
void process_exit (void);
void process_activate (void);


/****************************************************************/
/* Choi Add */
char** parse_filename(const char *file_name, int *cnt);
void construct_ESP(char **args, int args_cnt, void **esp);

/* Add end */
/****************************************************************/

#endif /* userprog/process.h */
