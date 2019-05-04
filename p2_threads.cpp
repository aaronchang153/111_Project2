#include "p2_threads.h"
#include "utils.h"

extern pthread_cond_t  cond;
extern pthread_mutex_t mutex;
extern std::queue<Person> man_queue;
extern std::queue<Person> woman_queue;
extern Fittingroom fitting_room;

void *threadfunc(void *param)
{

	int status;
	printf(" [Thread] Start\n");

	printf(" [Thread] Locks\n");
	status = pthread_mutex_lock(&mutex);

    printf(" [Thread] Blocked\n");
    status = pthread_cond_wait(&cond, &mutex);

	printf(" [Thread] Starts again.\n");
	for (int i=0; i<3; i++) {
		printf(" [Thread] Complete thread after (%d) seconds\n", (3-i));
		usleep(MSEC(1000));
	}

	printf(" [Thread] Unlocks\n");
	status = pthread_mutex_unlock(&mutex);
	printf(" [Thread] Complete\n");
}

void *producer_thread(void *param){
	int status;
	
	// total number of each gender to create
	int each_gender_cnt = *((int *) param);
	// current number of men/women produced
	int man_count = 0;
	int woman_count = 0;

	for(int i = 0; i < 2 * each_gender_cnt; i++){
		Person p;
		// if we've already made the max number of men
		if(man_count == each_gender_cnt)
			p.set_gender(FEMALE);
		// if we've already made the max number of women
		else if(woman_count == each_gender_cnt)
			p.set_gender(MALE);
		// general case
		else
			p.set_gender(rand() % 2); // MALE = 0, FEMALE = 1
		
		p.set_time((rand() % 8) + 3); // set stay time to 3-10 ms

		// add this new person to their corresponding queue
		// acquire lock
		status = pthread_mutex_lock(&mutex);

		if(p.get_gender == MALE)
			man_queue.push(p);
		else
			woman_queue.push(p);

		// release lock
		status = pthread_mutex_unlock(&mutex);
		
		usleep(MSEC((rand() % 5) + 1)); // wait 1-5 ms before making another person
	}

	return NULL;
}

void *removing_thread(void *param){
	int status;

	// acquire lock
	status = pthread_mutex_lock(&mutex);

	// remove everyone that's ready to leave
	// remove_ready returns non-zero if anyone was removed
	if(fitting_room.remove_ready())
		pthread_cond_signal(&cond);

	// release lock
	status = pthread_mutex_unlock(&mutex);

	return NULL;
}

void *sending_thread(void *param){
	int status;

	// acquire lock
	status = pthread_mutex_lock(&mutex);

	// if all rooms are full wait until removing_thread removes someone
	if(fitting_room.get_status() == FULL)
		pthread_cond_wait(&cond, &mutex);

	// add person to the queue if necessary/possible
	if(fitting_room.get_status() == MANPRESENT){
		if(!man_queue.empty()){
			fitting_room.add_person(man_queue.front());
			man_queue.pop();
		}
	}
	else if(fitting_room.get_status() == WOMANPRESENT){
		if(!woman_queue.empty()){
			fitting_room.add_person(woman_queue.front());
			woman_queue.pop();
		}
	}
	else if(fitting_room.get_status() == EMPTY){
		// add a person from the queue with more people in it
		if(!(man_queue.empty() && woman_queue.empty())){
			if(man_queue.size() > woman_queue.size()){
				fitting_room.add_person(man_queue.front());
				man_queue.pop();
			}
			else{
				fitting_room.add_person(woman_queue.front());
				woman_queue.pop();
			}
		}
	}

	// release lock
	status = pthread_mutex_unlock(&mutex);

	return NULL;
}