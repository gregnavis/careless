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
	char *careless_path,
		source_path[BUFFER_SIZE],
		base_path[BUFFER_SIZE],
		target_path[BUFFER_SIZE];

	careless_path = getenv("CARELESS_PATH");
	if (!careless_path) {
		fprintf(stderr, "CARELESS_PATH is undefined\n");
		return -1;
	}

	if (AT_FDCWD != dirfd) {
		fprintf(stderr, "dirfd != AT_FDCWD\n");
		return -1;
	}

	absolute_path(pathname, source_path, sizeof(source_path));
	fprintf(stderr, "source_path = %s\n", source_path);
	snprintf(target_path,
		sizeof(target_path),
		"%s/%d.%d/%s",
		careless_path,
		getpid(),
		time(NULL),
		source_path);
	fprintf(stderr, "target_path = %s\n", target_path);
	basedir(target_path, base_path, sizeof(base_path));
	fprintf(stderr, "base_path = %s\n", base_path);
	mkdirp(base_path);

	fprintf(stderr, "rename(%s, %s) = %d\n", source_path, target_path, rename(source_path, target_path));

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
