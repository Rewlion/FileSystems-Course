#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <dirent.h>
#include <errno.h>
#include <ctype.h>

struct stat_info
{
	int pid;
	char comm[1024];
	char state;
	int ppid;
	int pgrp;	
};

char* get_stat_file(const char* pid)
{	
	const char* frm   = "/proc/%s/stat";
	const int str_len = snprintf(NULL, 0, frm, pid) + 1;
	char* stat_file   = calloc(str_len, 1);
	snprintf(stat_file, str_len, frm, pid);

	return stat_file;
}

void print_stat_info(const struct stat_info* stat_info)
{	
	printf("%c %d %d %d %s \n", stat_info->state, stat_info->pid, stat_info->ppid, stat_info->pgrp, stat_info->comm);	
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

void print_stat_info_for_pid(const char* pid)
{
	struct stat_info* si = get_stat_info_for_pid(pid);
	
	if(si != NULL)				
		print_stat_info(si);
	else
		printf("Error: cannot get stat info for pid:%s", pid);

	free(si);
}

int check_if_name_is_pid(const char* name)
{
	const size_t len = strlen(name);
	
	for(int i = 0; i < len; ++i)
		if(!isdigit(name[i]))
			return 0;
	
	return 1;
}

int main(int argc, char** argv)
{
	DIR* proc_dir = opendir("/proc/");
	
	if(proc_dir != NULL)
	{
		printf("state pid ppid pgrp comm \n");

		struct dirent* dir_entry = NULL;
		while((dir_entry = readdir(proc_dir)) != NULL)
		{
			if(check_if_name_is_pid(dir_entry->d_name))			
				print_stat_info_for_pid(dir_entry->d_name);
		}
	}
	else
		printf("Error: cannot access /proc dir: %s\n", strerror(errno));

	closedir(proc_dir);
}