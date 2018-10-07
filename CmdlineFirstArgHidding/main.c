
#include <sys/prctl.h>
#include <string.h>

// setcap cap_sys_resource=pe [exe] before the execution.

size_t get_size_of_arg_line(int argc, char** argv)
{
	size_t size = 0;
	for(int i = 0; i < argc; ++i)
		size += strlen(argv[i]) + 1;

	return size;
}

int main(int argc, char** argv)
{	
	if(argc == 1)
		return 0;

	const size_t arg_line_size = get_size_of_arg_line(argc,argv);
	const size_t rest_part_size = arg_line_size - strlen(argv[0]) - 1;

	prctl(PR_SET_MM, PR_SET_MM_ARG_START, argv[1], 0, 0);
	prctl(PR_SET_MM, PR_SET_MM_ARG_END, argv[1] + rest_part_size, 0, 0);
}
