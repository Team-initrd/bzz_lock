#include "buzzlock.h"

void queue_thread(bzz_t *lock, bzz_thread *thread)
{
	if (thread->color == BZZ_BLACK) {
		if (lock->black_threads == NULL || lock->black_end == NULL) {
			lock->black_threads = thread;
			lock->black_end = thread;
			return;
		}
		lock->black_end->next = thread;
		lock->black_end = thread;

	} else if (thread->color == BZZ_GOLD) {
		if (lock->gold_threads == NULL || lock->gold_end == NULL) {
			lock->gold_threads = thread;
			lock->gold_end = thread;
			return;
		}
		lock->gold_end->next = thread;
		lock->gold_end = thread;

	}
}

void remove_thread(bzz_t *lock, bzz_thread *thread)
{

}

// found online, no licence
int timeval_subtract (struct timeval *result, struct timeval *x, struct timeval *y)
{
	/* Perform the carry for the later subtraction by updating y. */
	if (x->tv_usec < y->tv_usec) {
		int nsec = (y->tv_usec - x->tv_usec) / 1000000 + 1;
		y->tv_usec -= 1000000 * nsec;
		y->tv_sec += nsec;
	}
	if (x->tv_usec - y->tv_usec > 1000000) {
		int nsec = (x->tv_usec - y->tv_usec) / 1000000;
		y->tv_usec += 1000000 * nsec;
		y->tv_sec -= nsec;
	}

	/* Compute the time remaining to wait.
	 *           tv_usec is certainly positive. */
	result->tv_sec = x->tv_sec - y->tv_sec;
	result->tv_usec = x->tv_usec - y->tv_usec;

	/* Return 1 if result is negative. */
	return x->tv_sec < y->tv_sec;
}

int start_next_thread(bzz_t *lock)
{
	bzz_thread *next_thread = NULL;
	struct timeval timediff;
	struct timeval current_time;
	gettimeofday(&current_time, NULL);
	if (lock->gold_threads) {
		timeval_subtract(&timediff, &current_time, &lock->gold_threads->time_created);
		if (timediff->tv_usec > lock->timeout || timediff->tv_sec > (lock->timeout / 1000000)) {
			// oldest gold thread is over threashold
			next_thread = lock->gold_threads;
			lock->gold_threads = next_thread->next;
			if (lock->gold_end == next_thread)
				lock->gold_end = NULL;
		}
	}

	// Get black thread
	if (next_thread == NULL) {
		//
	}
	
}

bzz_thread* alloc_bzz_thread(int color, pid_t tid)
{
	bzz_thread* new_thread = malloc(sizeof(bzz_thread));

	pthread_cond_init(&new_thread->cond, NULL);
	new_thread->color = color;
	gettimeofday(&new_thread->time_created, NULL);
	new_thread->tid = tid;

	return new_thread;
}

void init_bzz(bzz_t *lock, int num_threads, useconds_t timeout)
{
	lock->gold_threads = NULL;
	lock->gold_end = NULL;
	lock->black_threads = NULL;
	lock->black_end = NULL;
	lock->current_locked = NULL;
	lock->timeout = timeout;
	pthread_mutex_init(lock->mutex, NULL);
	
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
