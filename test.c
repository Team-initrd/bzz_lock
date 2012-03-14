#include <stdio.h>
#include <stdlib.h>
#include <omp.h>
#include <time.h>
#include <sys/time.h>
#include <unistd.h>
#include "buzzlock.h"

#define THREADCOUNT 100
#define GOLD 30
#define BLACK THREADCOUNT-GOLD
#define THREADPOOL 5   

#define TIMEOUT 10000  // in us
#define ACTIVITY 5 // in us
bzz_t GTLOCK;

int ng, rt;

void thread(){
	struct timeval t1,t2;
	double elapsedTime;
	int ID = omp_get_thread_num();

	if(ID<=ng) bzz_color(BZZ_GOLD, &GTLOCK); 
	else  bzz_color(BZZ_BLACK, &GTLOCK);     // SET COLOR OF LOCK HERE Depending on condiion
	gettimeofday(&t1,NULL);
	if (rt) usleep(rand() % rt); // random sleep
	bzz_lock(&GTLOCK);
	usleep(ACTIVITY); // ACTIVITY
	bzz_release(&GTLOCK);
	gettimeofday(&t2,NULL);

	elapsedTime = (t2.tv_sec - t1.tv_sec);      // sec 
	elapsedTime += (t2.tv_usec - t1.tv_usec) / 1000000.0;   //  us

	if(ID<=ng) printf("GOLD = %f, #%d\n",elapsedTime, omp_get_thread_num());
	else printf("BLACK = %f, #%d\n",elapsedTime, omp_get_thread_num());


}


int main(int argc, char *argv[]){
	
	int tc, to;
	rt = 0;
		
	// seed random number generator
	srand(time(NULL));
	
	if (argc < 4) {
		printf("usage: test <threadcount> <numgold> <timeout> [presleepmax]\n");
		return 1;
	}
	
	tc = atoi(argv[1]);
	ng = atoi(argv[2]);
	to = atoi(argv[3]);
	if (argc == 5) rt = atoi(argv[4]);
	
	if (ng > tc) {
		printf("number of gold threads cannot exceed total threads!\n");
		return 1;
	}
	
	printf("thread count: %d\n", tc);
	printf("number gold: %d\n", ng);
	printf("thread timeout: %d\n", to);
	printf("random max: %d\n", rt);
	
	omp_set_num_threads(tc);
	init_bzz(&GTLOCK,THREADPOOL,to);
	printf("pointer: %p\n", GTLOCK);

	/*struct timespec time;
clock_gettime(CLOCK_THREAD_CPUTIME_ID, &time);
printf("Time: %ld, %ld\n", time.tv_sec, time.tv_nsec);
clock_gettime(CLOCK_THREAD_CPUTIME_ID, &time);
printf("Time: %ld, %ld\n", time.tv_sec, time.tv_nsec);
clock_gettime(CLOCK_THREAD_CPUTIME_ID, &time);
printf("Time: %ld, %ld\n", time.tv_sec, time.tv_nsec);
clock_gettime(CLOCK_THREAD_CPUTIME_ID, &time);
printf("Time: %ld, %ld\n", time.tv_sec, time.tv_nsec);*/

	#pragma omp parallel
	{
		thread();
	}

	bzz_kill(&GTLOCK);
	
	return 0;
}
