#ifndef __TYPES_P2_H
#define __TYPES_P2_H

#include <stdio.h>
#include <stdlib.h>
#include <iostream>
#include <sys/time.h>
#include <string>
#include <vector>
#include <unistd.h>

#define MALE   0
#define FEMALE 1

#define EMPTY        0
#define WOMANPRESENT 1
#define MANPRESENT   2
#define FULL         3


class Person
{

	int gender; // 0: male 1: female
	std::string str_gender;
	struct timeval t_create;
	struct timeval t_start;
	struct timeval t_end;
	long time_to_stay_ms;


	unsigned long order;
	unsigned long use_order;

public:
	Person();

	void set_gender(int data);
	int get_gender(void);

	void set_order(unsigned long data);
	unsigned long get_order(void);

	void set_use_order(unsigned long data);
	unsigned long get_use_order(void);

	void set_time(long data);
	int ready_to_leave(void);

	void start(void);
	void complete(void);
};


// Class for the fittingroom
// You may need to add more class member variables and functions
class Fittingroom {

private:
	int status;
	int num_stalls;

	// std::vector was giving me some trouble so back to array it is then
	Person *stalls; // the actual people and their information
	char *occupied; // keeps track of whether or not each stall is occupied
	int occupied_count; // number of stalls occupied
	void add_to_array(Person &p);
	void remove_from_array(int pos);

public:
	Fittingroom();
	~Fittingroom();

	void set_num_stalls(int n);

	// You need to use this function to print the Fittingroom's status
	void print_status(void);
	int get_status(void);


	// Call by reference
	// This is just an example. You can implement any function you need
	void add_person(Person &p);

	// Remove everyone who's ready to leave
	// returns 1 if anyone was removed and 0 otherwise
	int remove_ready(void);
};

#endif
