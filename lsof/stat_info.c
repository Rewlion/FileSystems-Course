#include "stat_info.h"

#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <dirent.h>
#include <ctype.h>

static char* get_stat_file(const char* pid)
{
	const char* frm   = "/proc/%s/stat";
	const int str_len = snprintf(NULL, 0, frm, pid) + 1;
	char* stat_file   = calloc(str_len, 1);
	snprintf(stat_file, str_len, frm, pid);

	return stat_file;
}

struct stat_info* get_stat_info_for_pid(const char* pid)
{
	struct stat_info* si = NULL;
	char*             fn = get_stat_file(pid);
	FILE*             ff = fopen(fn, "r");
	
	if(ff == NULL)
		goto out;

	si = calloc(1, sizeof(*si));
	if(si != NULL)
	{
		fscanf(ff, "%d %s %c %d %d", 
		&si->pid, 
		si->comm, 
		&si->state, 
		&si->ppid, 
		&si->pgrp);
	}

out:
	fclose(ff);
	free(fn);
	return si;
}

int check_if_name_is_pid(const char* name)
{
	const size_t len = strlen(name);
	
	for(int i = 0; i < len; ++i)
		if(!isdigit(name[i]))
			return 0;
	
	return 1;
}