#include "atp.h"
#include <vector>
#include <algorithm>
#include <ctime>

void greedy_lr(ATI &ati) {
	bool schedule_changed = true;
	while(schedule_changed) {
		schedule_changed = false;
		for(int i = 0; i <= ati.num_times; i++) {
			if(ati.g[ati.timenodes[i]].closed)
				continue;
			close_timeslot(ati, ati.timenodes[i]);
			if(!feasible_schedule_exists(ati)) {
				open_timeslot(ati, ati.timenodes[i]);
			} else {
				ati.g[ati.timenodes[i]].closed = true;
				schedule_changed = true;
			}
		}
	}
}

void read_jobdata_stdin(ATI &ati) {
	int r, d, p;
	std::cin >> ati.cap;
	while(std::cin >> r >> d >> p)
		add_job(ati, r, d, p);
}

void create_tight_3example(ATI &ati, int g) {
	/* g-2 rigid jobs of length g in range [x, x + g)
	g unit jobs in range [x, x + g - 1)
	g unit jobs in range (x, x + g - 1]
	one job of length g in range [x - g, x + g - 1]
	one job of length g in range [x, x + 2g] */

	int x = g;	// start at 0
	for(int i = 0; i < g - 2; i++) {
		add_job(ati, x, x + g - 1, g);
	}
	for(int i = 0; i < g; i++) {
		add_job(ati, x, x + g - 1, 1);
		add_job(ati, x + 1, x + g, 1);
	}
	add_job(ati, x - g, x + g - 1, g);
	add_job(ati, x, x + 2 * g, g);
}

void timespec_diff(struct timespec &start, struct timespec &stop, struct timespec &result) {
	if ((stop.tv_nsec - start.tv_nsec) < 0) {
		result.tv_sec = stop.tv_sec - start.tv_sec - 1;
		result.tv_nsec = stop.tv_nsec - start.tv_nsec + 1000000000;
	} else {
		result.tv_sec = stop.tv_sec - start.tv_sec;
		result.tv_nsec = stop.tv_nsec - start.tv_nsec;
	}
}

int main(int argc, char **argv) {
	ATI ati;

	ati_init(ati);
	read_jobdata_stdin(ati);
	ati_jobadd_complete(ati);

	// details(ati);

	timespec start, end, diff;

	// obtain minimal feasible schedule and print the number of slots used
	clock_gettime(CLOCK_REALTIME, &start);
	greedy_lr(ati);
	clock_gettime(CLOCK_REALTIME, &end);

	timespec_diff(start, end, diff);
	std::cout << diff.tv_sec << ":" << diff.tv_nsec << std::endl;

	int soln = get_num_open_timeslots(ati);
	std::cout << soln << std::endl;
	// print_open_timeslots(ati);

	return 0;
}

