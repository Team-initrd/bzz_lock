#include "buzzlock.h"

void init_bzz(bzz_t *lock, int num_threads, useconds_t timeout)
{
	printf("init_bzz: %d %d %d\n", *lock, num_threads, timeout);

	// allocate memory
}

void bzz_color(int color, bzz_t *lock)
{
	
	if (color == BZZ_BLACK) {
		printf("bzz_color: BLACK %d\n", *lock);
	} else if (color == BZZ_GOLD) {
		printf("bzz_color: GOLD %d\n", *lock);
	}
}

void bzz_lock(bzz_t *lock)
{
	// TODO: wait on bzz_thread's condition variable
	// TODO: needs to happen differently if nothing has the lock
	printf("bzz_lock: %d\n", *lock);
}

void bzz_release(bzz_t *lock)
{
	// TODO: find next thread to unlock and signal its condition
	printf("bzz_release: %d\n", *lock);
}

void bzz_kill(bzz_t *lock)
{
	printf("bzz_kill: %d\n", *lock);
}
