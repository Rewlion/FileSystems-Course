#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <dirent.h>
#include <errno.h>

const char* proc = "/proc/";
const char* statf = "/stat";

struct stat_info
{
	int pid;
	char* comm;
	char state;
	int ppid;
	int pgrp;	
};

char* get_stat_file(const char* pid)
{
	const size_t total_size = strlen(proc) + strlen(pid) + strlen(statf) + 1;
	char* stat_file = (char*)calloc(total_size,sizeof(char));
	strcpy(stat_file, proc);
	strcat(stat_file, pid);
	strcat(stat_file, statf);

	return stat_file;
}

void print_stat_info(struct stat_info* stat_info)
{
	if(stat_info)
	{
		printf("%c %d %d %d %s \n", stat_info->state, stat_info->pid, stat_info->ppid, stat_info->pgrp, stat_info->comm);
	}
}

struct stat_info* allocate_stat_info()
{
	struct stat_info* stat_info = (struct stat_info*)calloc(1, sizeof(struct stat_info));
                   	  stat_info->comm = (char*)calloc(1024,sizeof(char));

    return stat_info;
}

struct stat_info* get_stat_info_for_pid(const char* pid)
{
	char*             stat_file_name = get_stat_file(pid);
	FILE*             stat_file = fopen(stat_file_name, "r");
	struct stat_info* stat_info = allocate_stat_info();

	if((stat_file != NULL) && (stat_info != NULL))
	{
		fscanf(stat_file, "%d %s %c %d %d", 
			&stat_info->pid, 
			stat_info->comm, 
			&stat_info->state, 
			&stat_info->ppid, 
			&stat_info->pgrp);

		return stat_info;
	}
	else
		return NULL;
}

void print_stat_info_for_pid(const char* pid)
{
	struct stat_info* stat_info = get_stat_info_for_pid(pid);
	if(stat_info != NULL)				
		print_stat_info(stat_info);
	else
		printf("Error: cannot get stat info for pid:%s", pid);
}

int check_if_name_is_pid(const char* name)
{
	size_t len = strlen(name);
	for(size_t i = 0; i < len; ++i)
	{
		if((name[i] < '0') || name[i] > '9')
			return 0;
	}

	return 1;
}

int main(int argc, char** argv)
{
	DIR* proc_dir = opendir(proc);
	
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
}