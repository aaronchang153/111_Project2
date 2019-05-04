#ifndef __P2_THREADS_H
#define __P2_THREADS_H

#include <stdio.h>
#include <stdlib.h>
#include <iostream>
#include <sys/time.h>
#include <string>
#include <vector>
#include <queue>
#include <unistd.h>
#include "types_p2.h"

void *threadfunc(void *param);

// Thread for...
//  ...producing people to send to fitting rooms
void *producer_thread(void *param);

//  ...checking if anyone is ready to leave the fitting room and removing them
void *removing_thread(void *param);

//  ...sending people to the fitting room if there's space
void *sending_thread(void *param);


#endif
