#include "utils.h"


long get_elasped_time(struct timeval& start, struct timeval& end)
{

	long mtime = 0;
    long seconds  = end.tv_sec  - start.tv_sec;
    long useconds = end.tv_usec - start.tv_usec;

    mtime = ((seconds) * 1000 + useconds/1000.0) + 0.5;

	return mtime;
}

std::string num_to_string(int n){
    if(n < 1)
        return "";
    
    char tmp[5];

    switch(n){
        case 1:
        sprintf(tmp, "1st");
        break;
        case 2:
        sprintf(tmp, "2nd");
        break;
        case 3:
        sprintf(tmp, "3rd");
        break;
        default:
        sprintf(tmp, "%dth", n);
    };

    std::string result(tmp);
    return result;
}
