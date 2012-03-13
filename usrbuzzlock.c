#include "buzzlock.h"

pid_t gettid()
{
	return (pid_t)syscall(__NR_gettid);
}

void add_thread(bzz_t *lock, bzz_thread *thread, int queue)
{
	thread->next = NULL;
	if (queue == 0) {
		thread->next = lock->unqueued_threads;
		lock->unqueued_threads = thread;
		return;
	}

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

void queue_thread(bzz_t *lock, bzz_thread *thread)
{
	add_thread(lock, thread, 1);
}


bzz_thread* get_unqueued_thread(bzz_t *lock, pid_t tid)
{
	bzz_thread* curr = lock->unqueued_threads;
	bzz_thread* last = NULL;

	while(curr && curr->tid != tid) {
		last = curr;
		curr = curr->next;
	}

	if (last && curr)
		last->next = curr->next;
	if (curr && curr == lock->unqueued_threads)
		lock->unqueued_threads = curr->next;

	return curr;
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
printf("sub: %ld, %ld\n", result->tv_sec, result->tv_usec);
	/* Return 1 if result is negative. */
	return x->tv_sec < y->tv_sec;
}

int start_next_thread(bzz_t *lock)
{
	bzz_thread *next_thread = NULL;
	struct timeval timediff;
	struct timeval current_time;

	//lock->current_locked = NULL;
	gettimeofday(&current_time, NULL);
	if (lock->gold_threads) {
		timeval_subtract(&timediff, &current_time, &lock->gold_threads->time_created);
		if (timediff.tv_usec > lock->timeout || timediff.tv_sec > (lock->timeout / 1000000)) {
			// oldest gold thread is over threashold
			next_thread = lock->gold_threads;
			lock->gold_threads = next_thread->next;
			if (lock->gold_end == next_thread)
				lock->gold_end = NULL;
		}
	}

	// Get black thread
	if (next_thread == NULL && lock->black_threads) {
		next_thread = lock->black_threads;
		lock->black_threads = next_thread->next;
		if (lock->black_end == next_thread)
			lock->black_end = NULL;
	}

	if (next_thread == NULL) {
		lock->current_locked = NULL;
		return 0;
	}

	lock->current_locked = next_thread;
	pthread_cond_signal(&next_thread->cond);
	next_thread->next = NULL;
	add_thread(lock, next_thread, 0);

	return 1;
	
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
	pthread_mutex_init(&lock->mutex, NULL);
	
	printf("init_bzz: %p %d %d\n", lock, num_threads, timeout);

	// allocate memory
}

void bzz_color(int color, bzz_t *lock)
{
	bzz_thread* new_thread = alloc_bzz_thread(color, gettid());
	pthread_mutex_lock(&lock->mutex);
	add_thread(lock, new_thread, 0);
	pthread_mutex_unlock(&lock->mutex);

	if (color == BZZ_BLACK) {
		printf("bzz_color: BLACK %d\n", new_thread->tid);
	} else if (color == BZZ_GOLD) {
		printf("bzz_color: GOLD %d\n", new_thread->tid);
	}
}

void bzz_lock(bzz_t *lock)
{
	// wait on bzz_thread's condition variable
	// needs to happen differently if nothing has the lock
	pthread_mutex_lock(&lock->mutex);
	bzz_thread* to_lock = get_unqueued_thread(lock, gettid());
	if (to_lock == NULL) {
		printf("ERROR: Thread color not initialized. TID:%d\n", gettid());
		pthread_mutex_unlock(&lock->mutex);
		return;
	}

	// Not currently locked
	//pthread_mutex_lock(&lock->mutex);
	if (lock->current_locked == NULL) {
		lock->current_locked = to_lock;
	} else {
		queue_thread(lock, to_lock);
		pthread_cond_wait(&to_lock->cond, &lock->mutex);
		lock->current_locked = to_lock;
	}
	
	pthread_mutex_unlock(&lock->mutex);

	printf("bzz_lock: %d, color: %d\n", to_lock->tid, to_lock->color);
}

void bzz_release(bzz_t *lock)
{
	// find next thread to unlock and signal its condition
	printf("bzz_release: %p tid: %d\n", lock, gettid());
	pthread_mutex_lock(&lock->mutex);
	if (lock->current_locked->tid != gettid()) {
		printf("ERROR: You don't have the lock.\n");
		return;
	}

	//pthread_mutex_lock(&lock->mutex);
	start_next_thread(lock);
	pthread_mutex_unlock(&lock->mutex);
}

void bzz_kill(bzz_t *lock)
{
	printf("bzz_kill: %p\n", lock);
}
