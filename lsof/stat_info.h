#pragma once

struct stat_info
{
	int pid;
	char comm[1024];
	char state;
	int ppid;
	int pgrp;	
};

struct stat_info* get_stat_info_for_pid(const char* pid);
int               check_if_name_is_pid(const char* name);