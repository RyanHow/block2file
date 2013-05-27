/*
  block2file
  Copyright (C) 2013 Zbit Pty Ltd (ryan@zbit.net.au)

  This program can be distributed under the terms of the GNU GPL.

  gcc -Wall block2file.c `pkg-config fuse --cflags --libs` -o block2file
*/

#define FUSE_USE_VERSION 26

#include <fuse.h>
#include <stdio.h>
#include <string.h>
#include <errno.h>
#include <fcntl.h>
#include <linux/fs.h>

static char *blk_dev = "/dev/sda";
static const char *block2file_path = "/blkdev.hdd";

static int block2file_getattr(const char *path, struct stat *stbuf)
{
	int res = 0;
	long file_size_in_bytes = 0;
	int file = 0;
	
	memset(stbuf, 0, sizeof(struct stat));
	if (strcmp(path, "/") == 0) {
		stbuf->st_mode = S_IFDIR | 0755;
		stbuf->st_nlink = 2;
	} else if (strcmp(path, block2file_path) == 0) {
		stbuf->st_mode = S_IFREG | 0444;
		stbuf->st_nlink = 1;
		file = open(blk_dev, 0);
		ioctl(file, BLKGETSIZE64, &file_size_in_bytes);
		close(file);
		stbuf->st_size = file_size_in_bytes;
	} else
		res = -ENOENT;

		
		
	return res;
}

static int block2file_readdir(const char *path, void *buf, fuse_fill_dir_t filler,
			 off_t offset, struct fuse_file_info *fi)
{
	(void) offset;
	(void) fi;

	if (strcmp(path, "/") != 0)
		return -ENOENT;

	filler(buf, ".", NULL, 0);
	filler(buf, "..", NULL, 0);
	filler(buf, block2file_path + 1, NULL, 0);

	return 0;
}

static int block2file_open(const char *path, struct fuse_file_info *fi)
{
	if (strcmp(path, block2file_path) != 0)
		return -ENOENT;

	if ((fi->flags & 3) != O_RDONLY)
		return -EACCES;

	return 0;
}

static int block2file_read(const char *path, char *buf, size_t size, off_t offset,
		      struct fuse_file_info *fi)
{
	int file = 0;
	size_t len;
	(void) fi;
	if(strcmp(path, block2file_path) != 0)
		return -ENOENT;

	len = strlen("Dummy");
	file = open(blk_dev, 0);
	ioctl(file, BLKGETSIZE64, &len);
	if (offset < len) {
		if (offset + size > len)
			size = len - offset;
		// Note: returns ssize_t which is signed wheresa size_t is not. So may return -1 on error.
		pread(file, buf, size, offset);
	} else
		size = 0;

	close(file);

	return size;
}

static struct fuse_operations block2file_oper = {
	.getattr	= block2file_getattr,
	.readdir	= block2file_readdir,
	.open		= block2file_open,
	.read		= block2file_read,
};

int main(int argc, char *argv[])
{
	blk_dev = argv[1];
	argc = argc - 1;
	int i;
	for (i = 1; i < argc; i++) {
		argv[i] = argv[i + 1];
	}
	return fuse_main(argc, argv, &block2file_oper, NULL);
}
