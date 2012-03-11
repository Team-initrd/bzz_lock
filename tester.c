#include "buzzlock.h"

// from buzzlock.so
int init_bzz(void);

int main(int argc, char *argv[])
{
	printf("%d\n", init_bzz());
	
	return 0;
}
