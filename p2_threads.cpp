#include "p2_threads.h"
#include "utils.h"

extern pthread_cond_t  cond;
extern pthread_mutex_t mutex;
extern std::queue<Person> man_queue;
extern std::queue<Person> woman_queue;
extern Fittingroom fitting_room;
extern char all_people_sent;
extern struct timeval t_global_start;


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
		if(man_count == each_gender_cnt){
			p.set_gender(FEMALE);
			p.set_order((woman_count++) + 1); // +1 cause person 0 is actually the 1st person and so on
		}
		// if we've already made the max number of women
		else if(woman_count == each_gender_cnt){
			p.set_gender(MALE);
			p.set_order((man_count++) + 1);
		}
		// general case
		else{
			p.set_gender(rand() % 2); // MALE = 0, FEMALE = 1
			if(p.get_gender() == MALE)
				p.set_order((man_count++) + 1);
			else
				p.set_order((woman_count++) + 1);
		}
		
		p.set_time((rand() % 8) + 3); // set stay time to 3-10 ms

		// add this new person to their corresponding queue
		// acquire lock
		status = pthread_mutex_lock(&mutex);

		if(p.get_gender() == MALE){
			man_queue.push(p);
			man_wants_to_enter(p);
		}
		else{
			woman_queue.push(p);
			woman_wants_to_enter(p);
		}

		// release lock
		status = pthread_mutex_unlock(&mutex);
		
		usleep(MSEC((rand() % 5) + 1)); // wait 1-5 ms before making another person
	}

	all_people_sent = 1;

	return NULL;
}

void *removing_thread(void *param){
	int status;

	// loop as long as there's still people left
	while(!man_queue.empty() || !woman_queue.empty() || (fitting_room.get_status() != EMPTY)
		|| !all_people_sent){

		// acquire lock
		status = pthread_mutex_lock(&mutex);

		// remove everyone that's ready to leave
		// remove_ready returns non-zero if anyone was removed
		if(fitting_room.remove_ready())
			pthread_cond_signal(&cond);

		// release lock
		status = pthread_mutex_unlock(&mutex);
	}

	return NULL;
}

void *sending_thread(void *param){
	int status;

	// loop as long as there's still people left
	while(!man_queue.empty() || !woman_queue.empty() || (fitting_room.get_status() != EMPTY)
		|| !all_people_sent){

		// acquire lock
		status = pthread_mutex_lock(&mutex);

		// if all rooms are full wait until removing_thread removes someone
		if(fitting_room.get_status() == FULL)
			pthread_cond_wait(&cond, &mutex);

		// add person to the queue if necessary/possible
		if(fitting_room.get_status() == MANPRESENT){
			if(!man_queue.empty()){
				Person p = man_queue.front();
				man_queue.pop();
				fitting_room.add_person(p);
			}
		}
		else if(fitting_room.get_status() == WOMANPRESENT){
			if(!woman_queue.empty()){
				Person p = woman_queue.front();
				woman_queue.pop();
				fitting_room.add_person(p);
			}
		}
		else if(fitting_room.get_status() == EMPTY){
			// add a person from the queue with more people in it
			if(!(man_queue.empty() && woman_queue.empty())){
				if(man_queue.size() > woman_queue.size()){
					Person p = man_queue.front();
					man_queue.pop();
					fitting_room.add_person(p);
				}
				else{
					Person p = woman_queue.front();
					woman_queue.pop();
					fitting_room.add_person(p);
				}
			}
		}

		// release lock
		status = pthread_mutex_unlock(&mutex);

		usleep(MSEC((rand() % 5) + 1)); // wait 1-5 ms before sending another person
	}

	return NULL;
}

/***** the helper functions that print stuff *****/

void man_wants_to_enter(Person &p){
	struct timeval t_curr;
	gettimeofday(&t_curr, NULL);

	int m_size = man_queue.size();
	int w_size = woman_queue.size();

	printf("[%ld ms] [Input] %s (Man) goes into the queue. Queue Status: Total %d (Men: %d, Women: %d)\n",
		get_elasped_time(t_global_start, t_curr),
		num_to_string(p.get_order()).c_str(),
		m_size + w_size,
		m_size,
		w_size);
}

void woman_wants_to_enter(Person &p){
	struct timeval t_curr;
	gettimeofday(&t_curr, NULL);

	int m_size = man_queue.size();
	int w_size = woman_queue.size();

	printf("[%ld ms] [Input] %s (Woman) goes into the queue. Queue Status: Total %d (Men: %d, Women: %d)\n",
		get_elasped_time(t_global_start, t_curr),
		num_to_string(p.get_order()).c_str(),
		m_size + w_size,
		m_size,
		w_size);
}

void man_enters(Person &p){
	struct timeval t_curr;
	gettimeofday(&t_curr, NULL);

	int m_size = man_queue.size();
	int w_size = woman_queue.size();

	printf("[%ld ms] [Queue] Send %s (Man) into the fitting room (Stay %ld ms), Queue Status: Total: %d (Men: %d, Women: %d)\n",
		get_elasped_time(t_global_start, t_curr),
		num_to_string(p.get_order()).c_str(),
		p.get_time(),
		m_size + w_size,
		m_size,
		w_size);
	
	printf("[%ld ms] [Fitting Room] %s (Man) goes into the fitting room (Stay %ld ms), Fitting Room State is (%s): Total: %d (Men: %d, Women: %d)\n",
		get_elasped_time(t_global_start, t_curr),
		num_to_string(p.get_order()).c_str(),
		p.get_time(),
		fitting_room.get_status_string().c_str(),
		fitting_room.get_occupied_count(),
		fitting_room.get_men_present(),
		fitting_room.get_women_present());
}

void woman_enters(Person &p){
	struct timeval t_curr;
	gettimeofday(&t_curr, NULL);

	int m_size = man_queue.size();
	int w_size = woman_queue.size();

	printf("[%ld ms] [Queue] Send %s (Woman) into the fitting room (Stay %ld ms), Queue Status: Total: %d (Men: %d, Women: %d)\n",
		get_elasped_time(t_global_start, t_curr),
		num_to_string(p.get_order()).c_str(),
		p.get_time(),
		m_size + w_size,
		m_size,
		w_size);
	
	printf("[%ld ms] [Fitting Room] %s (Woman) goes into the fitting room (Stay %ld ms), Fitting Room State is (%s): Total: %d (Men: %d, Women: %d)\n",
		get_elasped_time(t_global_start, t_curr),
		num_to_string(p.get_order()).c_str(),
		p.get_time(),
		fitting_room.get_status_string().c_str(),
		fitting_room.get_occupied_count(),
		fitting_room.get_men_present(),
		fitting_room.get_women_present());
}

void man_leaves(Person &p){
	struct timeval t_curr;
	gettimeofday(&t_curr, NULL);

	printf("[%ld ms] [Fitting Room] %s (Man) leaves the fitting room. Fitting Room State is (%s): Total: %d (Men: %d, Women: %d)\n",
		get_elasped_time(t_global_start, t_curr),
		num_to_string(p.get_order()).c_str(),
		fitting_room.get_status_string().c_str(),
		fitting_room.get_occupied_count(),
		fitting_room.get_men_present(),
		fitting_room.get_women_present());
}

void woman_leaves(Person &p){
	struct timeval t_curr;
	gettimeofday(&t_curr, NULL);

	printf("[%ld ms] [Fitting Room] %s (Woman) leaves the fitting room. Fitting Room State is (%s): Total: %d (Men: %d, Women: %d)\n",
		get_elasped_time(t_global_start, t_curr),
		num_to_string(p.get_order()).c_str(),
		fitting_room.get_status_string().c_str(),
		fitting_room.get_occupied_count(),
		fitting_room.get_men_present(),
		fitting_room.get_women_present());
}
