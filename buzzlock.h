#include <unistd.h>
#include <sys/types.h>
#include <stdio.h>
#include <stdint.h>

#define BZZ_BLACK 0
#define BZZ_GOLD 1

typedef uint32_t bzz_t;

void init_bzz(bzz_t *, int, useconds_t);
void bzz_color(int, bzz_t *);
void bzz_lock(bzz_t *);
void bzz_release(bzz_t *);
void bzz_kill(bzz_t *);
