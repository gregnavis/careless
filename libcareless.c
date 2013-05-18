/* The definition below is required to define RTLD_NEXT by including dlfcn.h */
#define _GNU_SOURCE

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <fcntl.h>
#include <time.h>
#include <unistd.h>

#define BUFFER_SIZE 4096

static void basedir(const char *, char *, size_t);
static void absolute_path(const char *, char *, size_t);
static void mkdirp(const char *);
static void chop(char *, char);

int unlinkat(int dirfd, const char *pathname, int flags)
{
	char *operation_path,
		source_path[BUFFER_SIZE],
		base_path[BUFFER_SIZE],
		target_path[BUFFER_SIZE];

	operation_path = getenv("CARELESS_OPERATION_PATH");
	if (!operation_path) {
		fprintf(stderr, "CARELESS_OPERATION_PATH is undefined\n");
		return -1;
	}

	if (AT_FDCWD != dirfd) {
		fprintf(stderr, "dirfd != AT_FDCWD; "
			"your version of rm may be unsupported\n");
		return -1;
	}

	absolute_path(pathname, source_path, sizeof(source_path));
	snprintf(target_path,
		sizeof(target_path),
		"%s/%s",
		operation_path,
		source_path);
	basedir(target_path, base_path, sizeof(base_path));
	mkdirp(base_path);

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

static void absolute_path(const char *relative_path, char *output, size_t size)
{
	char current_directory[BUFFER_SIZE];

	getcwd(current_directory, sizeof(current_directory));
	snprintf(output, size, "%s/%s", current_directory, relative_path);
}

/* A lazy implementation of mkdir -p ;-) */
static void mkdirp(const char *path)
{
	char command[BUFFER_SIZE];
	sprintf(command, "mkdir -p %s", path);
	system(command);
}

static void chop(char *string, char to_remove)
{
	size_t length = strlen(string);
	if (to_remove == string[length]) {
		string[length] = '\0';
	}
}
