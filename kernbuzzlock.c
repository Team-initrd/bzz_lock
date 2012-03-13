#include "buzzlock.h"

#define __NR_bzz 286

void init_bzz(bzz_t *lock, int num_threads, useconds_t timeout)
{
	long ret;
	
	ret = syscall(__NR_bzz, num_threads, NULL);

	printf("kernel say: %ld\n", ret);
}

void bzz_color(int color, bzz_t *lock)
{
	
}

void bzz_lock(bzz_t *lock)
{

}

void bzz_release(bzz_t *lock)
{

}

void bzz_kill(bzz_t *lock)
{

}
