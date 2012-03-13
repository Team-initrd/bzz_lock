#include <unistd.h>
#include <sys/types.h>
#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <pthread.h>
#include <sys/time.h>
#include <time.h>
#include <sys/syscall.h>

#define BZZ_BLACK 0
#define BZZ_GOLD 1

#ifdef BZZ_KERNEL_MODE
// TODO: kernel mode structures
typedef uint32_t bzz_t;

#else
typedef struct _bzz_thread {
	pthread_cond_t cond;
	int color;
	struct timeval time_created;
	pid_t tid;
	struct _bzz_thread* next;
} bzz_thread;

typedef struct {
	bzz_thread* gold_threads;
	bzz_thread* gold_end;
	bzz_thread* black_threads;
	bzz_thread* black_end;
	bzz_thread* unqueued_threads;
	bzz_thread* current_locked;
	useconds_t timeout;
	pthread_mutex_t mutex;
} bzz_t;
#endif

void init_bzz(bzz_t *lock, int num_threads, useconds_t timeout);
void bzz_color(int color, bzz_t *lock);
void bzz_lock(bzz_t *lock);
void bzz_release(bzz_t *lock);
void bzz_kill(bzz_t *lock);
