#include "stat_info.h"

#include <dirent.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <assert.h>

#define FD_NAME_SIZE (size_t)16
#define FD_PATH_SIZE (size_t)1024
#define COMMAND_SIZE (size_t)1024

struct fd_info
{
	char comm[COMMAND_SIZE];
	int  ppid;
	int  pgrp;	
	char file_path[FD_PATH_SIZE];
	char fd[FD_NAME_SIZE];
};

char* get_fd_dir_path(const char* pid)
{
	const char* frm = "/proc/%s/fd";
	const int   l = snprintf(NULL, 0, frm, pid) + 1;
	char*       p = calloc(l, 1);
	snprintf(p, l, frm, pid);
	
	return p;
}

char* get_fd_path(struct dirent* fd_dir_entry, const char* fd_dir_path)
{
	const char* frm = "%s/%s";
	const int   l = snprintf(NULL, 0, frm, fd_dir_path, fd_dir_entry->d_name);
	char*       p = calloc(l, 1);
	snprintf(p, l, frm, fd_dir_path, fd_dir_entry->d_name);	

    return p;
}

void print_fd_info(struct fd_info* fd_info)
{
	printf("%-20s %5d %5d %5s %s\n", 
			fd_info->comm,
			fd_info->ppid,
			fd_info->pgrp,
			fd_info->fd,
			fd_info->file_path);
}

void fill_fd_info(struct fd_info* fd_info, struct dirent* fd_dir_entry, const char* fd_dir_path)
{
	char* fp = get_fd_path(fd_dir_entry, fd_dir_path);
	
	readlink(fp, fd_info->file_path, FD_PATH_SIZE);	
	strncpy(fd_info->fd, fd_dir_entry->d_name, FD_NAME_SIZE);

	free(fp);
}

void fill_stat_info(struct fd_info* fd_info, const char* pid)
{
	struct stat_info* si = get_stat_info_for_pid(pid);	

	if(si != NULL)
	{
		strncpy(fd_info->comm, si->comm, COMMAND_SIZE);		
		fd_info->ppid = si->ppid;
		fd_info->pgrp = si->pgrp;
	}
	else
	{
		sprintf(fd_info->comm, "?");		
		fd_info->ppid = -1;
		fd_info->pgrp = -1;
	}

	free(si);
}

void print_openned_files_info_for_pid(const char* pid)
{
	struct fd_info fi = {};
	fill_stat_info(&fi, pid);

	char* dp = get_fd_dir_path(pid);	
	DIR*  fd_dir = opendir(dp);	

	if(fd_dir != NULL)
	{
		struct dirent* de = NULL;
		while((de = readdir(fd_dir)) != NULL)
		{
			if((strcmp(".",de->d_name) && strcmp("..",de->d_name)))
			{
				fill_fd_info(&fi, de, dp);
				print_fd_info(&fi);
			}
		}
	}
	else
	{
		sprintf(fi.fd, "?");
		sprintf(fi.file_path, "?");
		
		print_fd_info(&fi);
	}

	closedir(fd_dir);
	free(dp);	
}

int main(int argc, char** argv)
{
	DIR* proc_dir = opendir("/proc/");
	
	if(proc_dir != NULL)
	{
		printf("%-20s %5s %5s %5s %s \n",
			"COMMAND",
			"PID",
			"PGRP",
			"FD",
			"FILE");

		struct dirent* dir_entry = NULL;

		while((dir_entry = readdir(proc_dir)) != NULL)
		{
			if(check_if_name_is_pid(dir_entry->d_name))			
				print_openned_files_info_for_pid(dir_entry->d_name);
		}
	}

	closedir(proc_dir);
}