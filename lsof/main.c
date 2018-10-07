#include "stat_info.h"

#include <dirent.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <assert.h>

const char* proc_dir_name = "/proc/";
const char* fd_dir_name = "/fd";
const char* stat_file_name = "/stat";

const size_t fd_file_path_size = 1024;
const size_t fd_name_size = 16;

struct fd_info
{
	struct stat_info* pid_stat_info;
	char* file_path;
	char* fd;
};

struct fd_info* allocate_fd_info()
{
	struct fd_info* fd_info = (struct fd_info*)calloc(1, sizeof(struct fd_info));
					fd_info->file_path = (char*)calloc(fd_file_path_size, sizeof(char));
					fd_info->fd = (char*)calloc(fd_name_size, sizeof(char));

	return fd_info;
}

char* get_fd_dir_path(const char* pid)
{
	const size_t total_size = strlen(proc_dir_name) + strlen(pid) + strlen(fd_dir_name) + 1;
	char* fd_dir_path = (char*)calloc(total_size,sizeof(char));
	strcpy(fd_dir_path, proc_dir_name);
	strcat(fd_dir_path, pid);
	strcat(fd_dir_path, fd_dir_name);
	
	return fd_dir_path;
}

char* get_fd_path(struct dirent* fd_dir_entry, const char* fd_dir_path)
{
	const size_t fd_name_len = strlen(fd_dir_entry->d_name);
    char* fd_path = (char*)calloc(strlen(fd_dir_path) + 1/*/*/ + fd_name_len + 1, sizeof(char));

    strcpy(fd_path, fd_dir_path);
    strcat(fd_path, "/");
    strcat(fd_path, fd_dir_entry->d_name);

    return fd_path;
}

void fill_fd_info(struct fd_info* fd_info, struct dirent* fd_dir_entry, const char* fd_dir_path)
{
	char* fd_path = get_fd_path(fd_dir_entry, fd_dir_path);
	
	readlink(fd_path, fd_info->file_path, fd_file_path_size);	
	strncpy(fd_info->fd, fd_dir_entry->d_name, fd_name_size);

	free(fd_path);
}

void print_fd_info(struct fd_info* fd_info)
{
	if(fd_info->pid_stat_info != NULL)
	{
		printf("%-20s %5d %5d", 
			fd_info->pid_stat_info->comm,
			fd_info->pid_stat_info->pid,
			fd_info->pid_stat_info->pgrp);
	}
	else
	{
		printf("%-20s %5s %5s", "???", "???", "???");
	}
	
	printf("%5s %s\n", fd_info->fd, fd_info->file_path);
}

void print_openned_files_info_for_pid(const char* pid)
{
	struct fd_info* fd_info = allocate_fd_info();
					fd_info->pid_stat_info = get_stat_info_for_pid(pid);

	const char*     fd_dir_path = get_fd_dir_path(pid);	
	DIR*            fd_dir = opendir(fd_dir_path);

	if(fd_dir != NULL)
	{
		struct dirent*  fd_dir_entry = NULL;
		while((fd_dir_entry = readdir(fd_dir)) != NULL)
		{
			if((strcmp(".",fd_dir_entry->d_name) && strcmp("..",fd_dir_entry->d_name)))
			{
				fill_fd_info(fd_info, fd_dir_entry, fd_dir_path);
				print_fd_info(fd_info);
			}
		}
	}
	else
	{
		fd_info->fd = "???";
		fd_info->file_path = "???";
		print_fd_info(fd_info);
	}
	
}

int main(int argc, char** argv)
{
	DIR*           proc_dir = opendir(proc_dir_name);
	struct dirent* dir_entry = NULL;
		
	printf("%-20s %5s %5s %5s %s \n",
		"COMMAND",
		"PID",
		"PGRP",
		"FD",
		"FILE");

	while((dir_entry = readdir(proc_dir)) != NULL)
	{
		if(check_if_name_is_pid(dir_entry->d_name))			
			print_openned_files_info_for_pid(dir_entry->d_name);
	}
}