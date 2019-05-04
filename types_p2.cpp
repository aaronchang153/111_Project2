#include "types_p2.h"
#include "utils.h"



void Person::set_gender(int data) { gender = data; }
int Person::get_gender(void)      { return gender; }

void Person::set_order(unsigned long data) { order = data; }
unsigned long Person::get_order(void)      { return order; }

void Person::set_use_order(unsigned long data) { use_order = data; }
unsigned long Person::get_use_order(void)      { return use_order; }

void Person::set_time(long data) { time_to_stay_ms = data; }
int Person::ready_to_leave(void) {
	struct timeval t_curr;
	gettimeofday(&t_curr, NULL);

	if (get_elasped_time(t_start, t_curr) >= time_to_stay_ms) { return 1; }
	else { return 0; }
}




void Person::start(void) {
	gettimeofday(&t_start, NULL);
	printf("(%lu)th person enters the fittingroom: \n", order);
	printf(" - (%lu) milliseconds after the creation\n", get_elasped_time(t_create, t_start));
}

void Person::complete(void) {
	gettimeofday(&t_end, NULL);
	printf("(%lu)th person comes out of the fittingroom: \n", order);
	printf(" - (%lu) milliseconds after the creation\n", get_elasped_time(t_create, t_end));
	printf(" - (%lu) milliseconds after using the fittingroom\n", get_elasped_time(t_start, t_end));
}

Person::Person() {
	gettimeofday(&t_create, NULL);
}


Fittingroom::Fittingroom(){
	status = EMPTY;
	stalls = NULL;
	occupied = NULL;
}

Fittingroom::~Fittingroom(){
	if(stalls != NULL)
		delete[] stalls;
	if(occupied != NULL)
		delete[] occupied;
	stalls = NULL;
	occupied = NULL;
}

void Fittingroom::add_to_array(Person &p){
	if(stalls != NULL && occupied != NULL && status != FULL){
		for(int i = 0; i < num_stalls; i++){
			if(!occupied[i]){
				stalls[i] = p;
				occupied[i] = 1;
				occupied_count++;
				break;
			}
		}
	}
}

void Fittingroom::remove_from_array(int pos){
	if(stalls != NULL && occupied != NULL && status != EMPTY){
		// if pos is in range
		if(pos >= 0 && pos < num_stalls){
			occupied[pos] = 0; // no longer occupied
			occupied_count--;
			// don't need to mess with stalls since it can be overwritten
		}
	}
}

void Fittingroom::set_num_stalls(int n){
	// this can only be called once since nothing will change stalls after this
	if(stalls == NULL && occupied == NULL){
		stalls = new Person[n];
		occupied = new char[n];
		num_stalls = n;
		for(int i = 0; i < n; i++)
			occupied[i] = 0; //initialize all stalls to unoccupied
		occupied_count = 0;
	}
}

// You need to use this function to print the Fittingroom's status
void Fittingroom::print_status(void) {
	printf("Print fittingroom status\n");
}

int Fittingroom::get_status(void){
	return status;
}

void Fittingroom::add_person(Person &p) {
	if(status != FULL){
		if(status == EMPTY){
			add_to_array(p);
			status = (p.get_gender() == MALE) ? MANPRESENT : WOMANPRESENT;
		}
		// if the person is the same gender as everyone else in the stalls
		else if((status == MANPRESENT && p.get_gender() == MALE) ||
			(status == WOMANPRESENT && p.get_gender() == FEMALE)){
			add_to_array(p);
		}

		// check if that addition made all the stalls full
		if(occupied_count == num_stalls){
			add_to_array(p);
			status = FULL;
		}
	}
}

int Fittingroom::remove_ready(void){
	int result = 0;

	// check every stall to see if the person is ready to leave and remove if they are
	for(int i = 0; i < num_stalls; i++){
		if(occupied[i] && stalls[i].ready_to_leave()){
			remove_from_array(i);
			result = 1;
		}
	}

	if(occupied_count == 0)
		status = EMPTY;
	else if(result == 1 && status == FULL){ // if anyone was removed and the stalls used to all be full
		for(int i = 0; i < num_stalls; i++){
			if(occupied[i]){
				status = (stalls[i].get_gender() == MALE) ? MANPRESENT : WOMANPRESENT;
			}
		}
	}
}