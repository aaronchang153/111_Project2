#include <stdio.h>
#include <stdlib.h>
#include <iostream>
#include <sys/time.h>
#include <string>
#include <vector>
#include <queue>
#include <unistd.h>
#include <pthread.h>
#include "types_p2.h"
#include "p2_threads.h"
#include "utils.h"

pthread_cond_t  cond  = PTHREAD_COND_INITIALIZER;
pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;
std::queue<Person> man_queue;
std::queue<Person> woman_queue;
Fittingroom fitting_room;
char all_people_sent; // global flag to indicate when there are no more people being produced
struct timeval t_global_start;

int main(int argc, char** argv)
{
	if(argc != 3 || !(atoi(argv[1]) > 0 && atoi(argv[2]) > 0)){
		fprintf(stderr, "[USAGE] ./p2_exec <each_gender_count> <num_stalls>\n");
		return -1;
	}

	srand((unsigned) time(NULL));

	// This is to set the global start time
	gettimeofday(&t_global_start, NULL);

	int gender_count = atoi(argv[1]);
	int num_stalls = atoi(argv[2]);

	// don't need a lock for this since no threads have been created yet
	fitting_room.set_num_stalls(num_stalls);
	all_people_sent = 0;

	pthread_t tid[3];

	pthread_create(&tid[0], NULL, producer_thread, &gender_count);
	pthread_create(&tid[1], NULL, removing_thread, NULL);
	pthread_create(&tid[2], NULL, sending_thread, NULL);

	for(int i = 0; i < 3; i++)
		pthread_join(tid[i], NULL);

	return 0;


}

