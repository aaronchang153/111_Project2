#include "types_p2.h"
#include "p2_threads.h"
#include "utils.h"


void Person::set_gender(int data) {
	gender = data;
	str_gender = (data == MALE) ? "Man" : "Woman";
}
int Person::get_gender(void)      { return gender; }

void Person::set_order(unsigned long data) { order = data; }
unsigned long Person::get_order(void)      { return order; }

void Person::set_use_order(unsigned long data) { use_order = data; }
unsigned long Person::get_use_order(void)      { return use_order; }

void Person::set_time(long data) { time_to_stay_ms = data; }
long Person::get_time() { return time_to_stay_ms; }
int Person::ready_to_leave(void) {
	struct timeval t_curr;
	gettimeofday(&t_curr, NULL);

	if (get_elasped_time(t_start, t_curr) >= time_to_stay_ms) { return 1; }
	else { return 0; }
}




void Person::start(void) {
	gettimeofday(&t_start, NULL);
}

void Person::complete(void) {
	gettimeofday(&t_end, NULL);
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
		// linear search for an open stall
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

int Fittingroom::get_status(void){
	return status;
}

std::string Fittingroom::get_status_string(void){
	switch(status){
	case EMPTY:
		return "EMPTY";
	case MANPRESENT:
		return "MENPRESENT";
	case WOMANPRESENT:
		return "WOMENPRESENT";
	case FULL:
		return "FULL";
	default:
		return "UNDEFINED";
	};
}

int Fittingroom::get_occupied_count(void){
	return occupied_count;
}

int Fittingroom::get_men_present(void){
	return gender_flag ? 0 : occupied_count;
}

int Fittingroom::get_women_present(void){
	return gender_flag ? occupied_count : 0;
}

void Fittingroom::add_person(Person &p) {
	if(status != FULL){
		if(status == EMPTY){
			p.start();
			add_to_array(p);
			if(p.get_gender() == MALE){
				status = MANPRESENT;
				gender_flag = 0;
			}
			else{
				status = WOMANPRESENT;
				gender_flag = 1;
			}
		}
		// if the person is the same gender as everyone else in the stalls
		else if((status == MANPRESENT && p.get_gender() == MALE) ||
			(status == WOMANPRESENT && p.get_gender() == FEMALE)){
			p.start();
			add_to_array(p);
		}

		// check if that addition made all the stalls full
		if(occupied_count == num_stalls){
			status = FULL;
		}

		if(p.get_gender() == MALE)
			man_enters(p);
		else
			woman_enters(p);
	}
}

int Fittingroom::remove_ready(void){
	int result = 0;

	// check every stall to see if the person is ready to leave and remove if they are
	for(int i = 0; i < num_stalls; i++){
		if(occupied[i] && stalls[i].ready_to_leave()){
			stalls[i].complete();
			remove_from_array(i);
			if(occupied_count == 0)
				status = EMPTY;
			else if(status == FULL) // if the stalls used to all be full
				status = (stalls[i].get_gender() == MALE) ? MANPRESENT : WOMANPRESENT;

			if(stalls[i].get_gender() == MALE)
				man_leaves(stalls[i]);
			else
				woman_leaves(stalls[i]);

			result = 1;
		}
	}


	return result;
}
