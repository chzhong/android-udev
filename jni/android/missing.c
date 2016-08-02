
#include <string.h>

void *mempcpy(void * __restrict s1, const void * __restrict s2, size_t n)
{
	register char *r1 = s1;
	register const char *r2 = s2;

	while (n) {
		*r1++ = *r2++;
		--n;
	}

	return r1;
}

char *stpcpy(register char * __restrict s1, const char * __restrict s2)
{
	while ((*s1++ = *s2++) != 0);
	return s1 - 1;
}

char *canonicalize_file_name(const char *path)
{
	char *resolved, *result=NULL;
	
	resolved = malloc(PATH_MAX);
	if (resolved != NULL) {
		result = realpath(path, resolved);
		if (result == NULL) free(resolved);
	}
	return result;
}

int getgrnam_r(const char *name, struct group *grp, char *buf, size_t buflen, struct group **result)
{
	struct group *g = getgrnam(name);

	if (g != NULL) {
		grp->gr_gid = g->gr_gid;
		strncpy(buf, g->gr_name, buflen);
		grp->gr_name = buf;
		grp->gr_passwd = NULL; /* not used in udev */
		grp->gr_mem = NULL; /* not used in udev */
		*result = grp;
		return 0;
	}
	*result = NULL;
	return -1;
}

#undef sysconf
long udev_sysconf(int name)
{
	switch(name) {
		case _SC_GETPW_R_SIZE_MAX: return 1024;
		default: return sysconf(name);
	}
}

int sched_getaffinity(pid_t pid, size_t setsize, cpu_set_t* set)
{
	return syscall(__NR_sched_getaffinity, pid, setsize, set);
}

typedef unsigned long int __cpu_mask;

int __sched_cpucount(size_t setsize, cpu_set_t* cpuset)
{ 
    int count = 0; 
    const __cpu_mask *p = cpuset->__bits; 
    // Note that we assume, like glibc's implementation does, that setsize 
    // is a multiple of sizeof(__cpu_mask). The CPU_ALLOC_SIZE() macro indeed 
    // always returns a multiple of sizeof(__cpu_mask). 
    const __cpu_mask *end = p + (setsize / sizeof(__cpu_mask)); 
 
    while (p < end) { 
        count +=  __builtin_popcountl(*p++); 
    } 
 
    return count; 
} 
 
cpu_set_t *__sched_cpualloc (size_t count) 
{ 
    return (cpu_set_t*) malloc (CPU_ALLOC_SIZE (count)); 
} 
 
void __sched_cpufree (cpu_set_t *set) 
{ 
    free (set); 
} 