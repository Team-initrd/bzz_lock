#include "buzzlock.h"

int main(int argc, char *argv[])
{
	bzz_t lock;
	
	init_bzz(&lock, 420, 666);
	printf("%p\n", lock);
	
	return 0;
}
