#define FUSE_USE_VERSION 31

#include <fuse.h>
#include <errno.h>
#include <string.h>

const char* file_name = "hello";
const char* file_content = "hello, world!";

int rew_getattr(const char* path, struct stat* stbuf, struct fuse_file_info* fuse_file_info)
{	
	memset(stbuf, 0, sizeof(struct stat));
	if(!strcmp(path, "/"))
	{
		stbuf->st_mode = S_IFDIR | 0755;
		stbuf->st_nlink = 2;
	}
	else if(!strcmp(path + 1, file_name))
	{
		stbuf->st_mode = S_IFREG | 0444;
		stbuf->st_nlink = 1;
		stbuf->st_size = strlen(file_content);
	}
	else
		return -ENOENT;

	return 0;	
}

int rew_readdir(const char* path, void* buf, fuse_fill_dir_t filler, off_t offset, struct fuse_file_info* fuse_file_info, enum fuse_readdir_flags flags)
{
	filler(buf, file_name, NULL, 0, 0);
	return 0;
}

int rew_open(const char* path, struct fuse_file_info* fuse_file_info)
{
	if(!strcmp(path+1,file_name))
	{
		return 0;
	}
	else
		return -ENOENT;
}

int rew_read(const char* path, char* buffer, size_t read_size, off_t offset, struct fuse_file_info* fuse_file_info)
{
	if(!strcmp(path+1, file_name))
	{
		const size_t content_len = strlen(file_content);

		if(offset > content_len)
			return 0;

		read_size = (offset + read_size) > content_len ? (content_len - offset) : read_size;
		memcpy(buffer + offset, file_content, read_size);

		return read_size;
	}
	else
		return -ENOENT;
}

int main(int argc, char** argv)
{	
	struct fuse_args args = FUSE_ARGS_INIT(argc, argv);
	
	struct fuse_operations fuse_operations = 
	{	
		.getattr = rew_getattr,
		.readdir = rew_readdir,
		.read = rew_read,
		.open = rew_open,
	};

	return fuse_main(args.argc, args.argv, &fuse_operations, NULL);
}
