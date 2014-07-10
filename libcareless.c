/* The definition below is required to define RTLD_NEXT by including dlfcn.h */
#define _GNU_SOURCE

#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <string.h>
#include <errno.h>

#include <fcntl.h>
#include <time.h>
#include <sys/stat.h>
#include <unistd.h>

#define BUFFER_SIZE 4*4096

#define unused(argument) ((void) argument)

static void basedir(const char *, char *, size_t);
static void join_path(const char *, const char *, char *, size_t);
static void fdpath(int, char *, size_t);
static void mkdirp(const char *);
static void chop(char *, char);
static int is_dir(const char *);
static void move(const char *, const char *);
static void run(const char *, ...);

int unlinkat(int dirfd, const char *pathname, int flags)
{
	char *operation_path,
		root_path[BUFFER_SIZE],
		source_path[BUFFER_SIZE],
		base_path[BUFFER_SIZE],
		target_path[BUFFER_SIZE];

	unused(flags);

	operation_path = getenv("CARELESS_OPERATION_PATH");
	if (!operation_path) {
		fprintf(stderr, "CARELESS_OPERATION_PATH is undefined\n");
		return -1;
	}

	if (*pathname == '/') {
		strncpy(source_path, pathname, sizeof(source_path));
	} else if (AT_FDCWD == dirfd) {
		getcwd(root_path, sizeof(root_path));
		join_path(root_path, pathname, source_path, sizeof(source_path));
	} else {
		fdpath(dirfd, root_path, sizeof(root_path));
		join_path(root_path, pathname, source_path, sizeof(source_path));
	}

	snprintf(target_path,
		sizeof(target_path),
		"%s/%s",
		operation_path,
		source_path);
	basedir(target_path, base_path, sizeof(base_path));
	mkdirp(base_path);

	if (!is_dir(source_path) || !is_dir(target_path)) {
		move(source_path, target_path);
	} else {
		rmdir(source_path);
	}

	return 0;
}

static void basedir(const char *path, char *output, size_t size)
{
	char *p;

	strncpy(output, path, size);

	chop(output, '/');

	p = strrchr(output, '/');
	if (!p) {
		strncpy(output, ".", 1);
		goto out;
	}

	*p = '\0';

	if (!*output) {
		strncpy(output, "/", 1);
	}

out:
	return;
}

static void join_path(const char *path1, const char *path2, char *output, size_t size)
{
	snprintf(output, size, "%s/%s", path1, path2);
}

static void fdpath(int fd, char *output, size_t size)
{
	char path[BUFFER_SIZE];

	snprintf(path, sizeof(path), "/proc/self/fd/%d", fd);
	readlink(path, output, size);
}

/* A lazy implementation of mkdir -p ;-) */
static void mkdirp(const char *path)
{
	run("mkdir -p \"%s\"", path);
}

static void chop(char *string, char to_remove)
{
	size_t length = strlen(string);
	if (to_remove == string[length]) {
		string[length] = '\0';
	}
}

static int is_dir(const char *path)
{
	struct stat stat_;

	if (-1 == stat(path, &stat_)) {
		return 0;
	}

	return S_ISDIR(stat_.st_mode);
}

static void move(const char *source_path, const char *target_path)
{
	run("mv \"%s\" \"%s\"", source_path, target_path);
}

static void run(const char *command_format, ...)
{
	va_list args;
	char command[BUFFER_SIZE];

	va_start(args, command_format);
	vsnprintf(command, sizeof(command), command_format, args);
	va_end(args);
}
