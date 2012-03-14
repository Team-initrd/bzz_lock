#include <linux/linkage.h>
#include <linux/vmalloc.h>
#include <linux/kernel.h>
#include <linux/sched.h>
#include <linux/init.h>
#include <linux/time.h>
#include <asm/uaccess.h>

#define SYSBZZ_INIT 0
#define SYSBZZ_COLOR 1
#define SYSBZZ_LOCK 2
#define SYSBZZ_RELEASE 3
#define SYSBZZ_KILL 4
#define BZZ_GOLD 1
#define BZZ_BLACK 0

typedef struct _bzz_thread {
        int color;
        struct timeval time_created;
        struct task_struct *task;
        struct _bzz_thread *next;
} bzz_thread;

typedef struct {
        bzz_thread* gold_threads;
        bzz_thread* gold_end;
        bzz_thread* black_threads;
        bzz_thread* black_end;
        bzz_thread* unqueued_threads;
        bzz_thread* current_locked;
        int timeout;
        struct mutex *mutexxx;
} bzz_t;

typedef struct {
	bzz_t *lock;
	int num_threads;
	int timeout;
} bzz_init_args;

typedef struct {
	bzz_t *lock;
	int color;
} bzz_color_args;

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

bzz_thread* get_unqueued_thread(bzz_t *lock, struct task_struct* task)
{
	bzz_thread* curr = lock->unqueued_threads;
	bzz_thread* last = NULL;

	while(curr && curr->task != task) {
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
	printk("sub: %ld, %ld\n", result->tv_sec, result->tv_usec);
	/* Return 1 if result is negative. */
	return x->tv_sec < y->tv_sec;
}

int start_next_thread(bzz_t *lock)
{
	bzz_thread *next_thread = NULL;
	struct timeval timediff;
	struct timeval current_time;
	
	do_gettimeofday(&current_time);
	
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

	// non-expired gold
	if (next_thread == NULL && lock->gold_threads) {
		next_thread = lock->gold_threads;
		lock->gold_threads = next_thread->next;
		if (lock->gold_end == next_thread)
			lock->gold_end = NULL;
	}


	if (next_thread == NULL) {
		lock->current_locked = NULL;
		return 0;
	}

	lock->current_locked = next_thread;
	wake_up_process(next_thread->task);
	next_thread->next = NULL;
	add_thread(lock, next_thread, 0);

	return 1;
	
}

bzz_thread* alloc_bzz_thread(int color, struct task_struct *task)
{
	bzz_thread* new_thread = vmalloc(sizeof(bzz_thread));

	new_thread->task = task;
	new_thread->color = color;
	do_gettimeofday(&new_thread->time_created);

	return new_thread;
}


void init_bzz(bzz_t **lock_ptr, int num_threads, int timeout)
{
	bzz_t *lock = vmalloc(sizeof(bzz_t));
	*lock_ptr = lock;
        lock->gold_threads = NULL;
        lock->gold_end = NULL;
        lock->black_threads = NULL;
        lock->black_end = NULL;
        lock->current_locked = NULL;
        lock->timeout = timeout;
	lock->mutexxx = vmalloc(sizeof(struct mutex));
        mutex_init(lock->mutexxx);

        printk("init_bzz: %p %d %d\n", lock, num_threads, timeout);
}

void bzz_color(int color, bzz_t *lock)
{
	bzz_thread* new_thread = alloc_bzz_thread(color, current);
	mutex_lock(lock->mutexxx);
	add_thread(lock, new_thread, 0);
	mutex_unlock(lock->mutexxx);

	if (color == BZZ_BLACK) {
		printk("bzz_color: BLACK %p\n", new_thread->task);
	} else if (color == BZZ_GOLD) {
		printk("bzz_color: GOLD %p\n", new_thread->task);
	}
}

void bzz_lock(bzz_t *lock)
{
	// wait on bzz_thread's condition variable
	// needs to happen differently if nothing has the lock
	bzz_thread* to_lock;
	mutex_lock(lock->mutexxx);
	to_lock = get_unqueued_thread(lock, current);
	if (to_lock == NULL) {
		printk("ERROR: Thread color not initialized. TID:%p\n", current);
		mutex_unlock(lock->mutexxx);
		return;
	}

	// Not currently locked
	if (lock->current_locked == NULL) {
		lock->current_locked = to_lock;
	} else {
		queue_thread(lock, to_lock);
		mutex_unlock(lock->mutexxx);
		set_current_state(TASK_INTERRUPTIBLE);
		schedule();
		mutex_lock(lock->mutexxx);
		lock->current_locked = to_lock;
	}

	mutex_unlock(lock->mutexxx);

	printk("bzz_lock: %p, color: %d\n", to_lock->task, to_lock->color);
}

void bzz_release(bzz_t *lock)
{
	// find next thread to unlock and signal its condition
	printk("bzz_release: %p tid: %p\n", lock, current);
	mutex_lock(lock->mutexxx);
	if (lock->current_locked->task != current) {
		printk("ERROR: You don't have the lock.\n");
		return;
	}

	start_next_thread(lock);
	mutex_unlock(lock->mutexxx);
}

void bzz_kill(bzz_t *lock)
{
	bzz_thread *thd;
	int i = 0;
	
        printk("bzz_kill: %p\n", lock);
	
	// mainly for debugging (has yet to be encountered)
	if (lock->gold_threads || lock->black_threads || lock->gold_end || lock->black_end || lock->current_locked)
	{
		printk("Thread lists not empty, aborting\n");
		return;
	}
	
	// free all unqueued threads
	while ((thd = lock->unqueued_threads))
	{
		printk("freeing: %p %d\n", thd->task, i++);
		lock->unqueued_threads = lock->unqueued_threads->next;
		vfree(thd);
	}
	
	vfree(lock->mutexxx);
	vfree(lock);
}

asmlinkage long sys_bzz(int argid, void *arg)
{
	bzz_init_args init_args;
	bzz_color_args color_args;
	bzz_t *lock_ptr;
	printk(KERN_DEBUG "Running sys_bzz: argid %d\n", argid);
	switch (argid)
	{
		case SYSBZZ_INIT:
			copy_from_user(&init_args, arg, sizeof(bzz_init_args));
			init_bzz(&init_args.lock, init_args.num_threads, init_args.timeout);
			copy_to_user(arg, &init_args, sizeof(bzz_init_args));
			printk("SYSBZZ_INIT: %d %d\n", init_args.num_threads, init_args.timeout);
			break;
		case SYSBZZ_COLOR:
			copy_from_user(&color_args, arg, sizeof(bzz_color_args));
			bzz_color(color_args.color, color_args.lock);
			printk("SYSBZZ_COLOR\n");
			break;
		case SYSBZZ_LOCK:
			copy_from_user(&lock_ptr, arg, sizeof(bzz_t*));
			bzz_lock(lock_ptr);
			printk("SYSBZZ_LOCK\n");
			break;
		case SYSBZZ_RELEASE:
			copy_from_user(&lock_ptr, arg, sizeof(bzz_t*));
			bzz_release(lock_ptr);
			printk("SYSBZZ_RELEASE\n");
			break;
		case SYSBZZ_KILL:
			copy_from_user(&lock_ptr, arg, sizeof(bzz_t*));
			bzz_kill(lock_ptr);
			copy_to_user(arg, &lock_ptr, sizeof(bzz_t*));
			printk("SYSBZZ_KILL\n");
			break;
		default:
			return -1;
	}
	
	return 0;
}
