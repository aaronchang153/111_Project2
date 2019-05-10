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


// Thread for...
//  ...producing people to send to fitting rooms
void *producer_thread(void *param);

//  ...checking if anyone is ready to leave the fitting room and removing them
void *removing_thread(void *param);

//  ...sending people to the fitting room if there's space
void *sending_thread(void *param);


/**
 * Helper Functions for Printing Stuff
 * Should be called only if the calling function has the lock
 */

// Used by producer thread
void man_wants_to_enter(Person &p);
void woman_wants_to_enter(Person &p);

// Used by sending thread
void man_enters(Person &p);
void woman_enters(Person &p);

// Used by removing thread
void man_leaves(Person &p);
void woman_leaves(Person &p);

#endif
