#include <linux/linkage.h>

#define SYSBZZ_INIT 0
#define SYSBZZ_COLOR 1
#define SYSBZZ_LOCK 2
#define SYSBZZ_RELEASE 3
#define SYSBZZ_KILL 4

asmlinkage long sys_bzz(int argid, void *arg)
{
	switch (argid)
	{
		case SYSBZZ_INIT:
			break;
		case SYSBZZ_COLOR:
			break;
		case SYSBZZ_LOCK:
			break;
		case SYSBZZ_RELEASE:
			break;
		case SYSBZZ_KILL:
			break;
		default:
			return -1;
	}
	
	return 0;
}
