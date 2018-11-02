#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#define _GNU_SOURCE

int main(int argc, char **argv)
{
	// Get processor information from /proc/cpuinfo
	FILE *cpuinfo = fopen("/proc/cpuinfo", "rb");
	char *arg = 0;
	size_t size = 0;
	while(getdelim(&arg, &size, 0, cpuinfo) != -1)
	{
		puts(arg);
	}
	free(arg);
	fclose(cpuinfo);

	// Get number of cores in a computer
	int num_cpus = sysconf( _SC_NPROCESSORS_ONLN );
	printf("%d\n",num_cpus);
	return 0;
}
