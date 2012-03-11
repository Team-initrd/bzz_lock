#include "buzzlock.h"

void init_bzz(bzz_t *lock, int num_threads, useconds_t timeout)
{
	printf("init_bzz: %d %d %d\n", *lock, num_threads, timeout);
}

void bzz_color(int color, bzz_t *lock)
{
	if (!color)
		printf("bzz_color: BLACK %d\n", *lock);
	else
		printf("bzz_color: GOLD %d\n", *lock);
}

void bzz_lock(bzz_t *lock)
{
	printf("bzz_lock: %d\n", *lock);
}

void bzz_release(bzz_t *lock)
{
	printf("bzz_release: %d\n", *lock);
}

void bzz_kill(bzz_t *lock)
{
	printf("bzz_kill: %d\n", *lock);
}
