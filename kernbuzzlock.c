#include "buzzlock.h"

#define __NR_bzz 286

void init_bzz(bzz_t *lock, int num_threads, useconds_t timeout)
{
	struct {
		bzz_t lock;
		int num_threads;
		useconds_t timeout;
	} arg;
	
	//arg.lock = lock;
	arg.num_threads = num_threads;
	arg.timeout = timeout;
	
	if (syscall(__NR_bzz, SYSBZZ_INIT, (void *) &arg) < 0)
		fprintf(stderr, "init_bzz error\n");
	*lock = arg.lock; 
}

void bzz_color(int color, bzz_t *lock)
{
	struct {
		int color;
		bzz_t *lock;
	} arg;
	
	arg.color = color;
	arg.lock = lock;
	
	if (syscall(__NR_bzz, SYSBZZ_COLOR, (void *) &arg) < 0)
		fprintf(stderr, "bzz_color error\n");
}

void bzz_lock(bzz_t *lock)
{
	if (syscall(__NR_bzz, SYSBZZ_LOCK, (void *) lock) < 0)
		fprintf(stderr, "bzz_lock error\n");
}

void bzz_release(bzz_t *lock)
{
	if (syscall(__NR_bzz, SYSBZZ_RELEASE, (void *) lock) < 0)
		fprintf(stderr, "bzz_release error\n");
}

void bzz_kill(bzz_t *lock)
{
	if (syscall(__NR_bzz, SYSBZZ_KILL, (void *) lock) < 0)
		fprintf(stderr, "bzz_kill error\n");
}
