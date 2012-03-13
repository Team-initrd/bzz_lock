#include <unistd.h>
#include <sys/types.h>
#include <stdio.h>
#include <stdint.h>
#include <pthread.h>
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
	suseconds_t time_created;
	pid_t tid;
	struct _bzz_thread* next;
} bzz_thread;

typedef struct {
	bzz_thread* yellow_threads;
	bzz_thread* black_threads;
	bzz_thread* current_locked;
	pthread_mutex_t mutex;
} bzz_t;
#endif

void init_bzz(bzz_t *, int, useconds_t);
void bzz_color(int, bzz_t *);
void bzz_lock(bzz_t *);
void bzz_release(bzz_t *);
void bzz_kill(bzz_t *);
