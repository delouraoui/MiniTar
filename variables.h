#ifndef H_VARIABLES
#define H_VARIABLES 
#define BUFSIZE 1024
#include <limits.h>
#include <stdlib.h>
#include <stdio.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <string.h>
#include <errno.h>
#include <err.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <libgen.h>
#include <grp.h>
#include <unistd.h>
#include <dirent.h>
#include <utime.h>
#include <fcntl.h>
#include <time.h>
#include <pwd.h>
typedef struct entete{
	size_t path_length;
	off_t file_length;
	mode_t mode;
	time_t m_time;
	time_t a_time;
	char checksum[32];
	char path[PATH_MAX];
} entete;
#endif
