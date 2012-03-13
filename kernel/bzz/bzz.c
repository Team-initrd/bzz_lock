#include <linux/linkage.h>

asmlinkage long sys_bzz(int argid, void *arg)
{
	return argid++;
}
