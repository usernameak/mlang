#include "util.h"
#include "fcntl.h"
#include "unistd.h"
#include "sys/stat.h"
#include <iostream>
#include <cstdint>
#include <cstring>
#include <cstdlib>

int Util::ReadFile (char const *name, uint8_t **buffer) {
	int handle, count, length;
	struct stat fileinfo;
	uint8_t *buf;

	handle = open (name, O_RDONLY, 0666);
	if (handle == -1) {
		std::cerr << "Couldn't read file " << name << std::endl;
		return -1;
	}
	// [BL] Use stat instead of fstat for v140_xp hack
	if (stat (name,&fileinfo) == -1) {
		std::cerr << "Couldn't read file " << name << std::endl;
		return -2;
	}
	length = fileinfo.st_size;
	buf = new uint8_t[length];
	count = read (handle, buf, length);
	close (handle);

	if (count < length) {
		std::cerr << "Couldn't read file " << name << std::endl;
		return -3;
	}

	*buffer = buf;
	return length;
}

char* Util::CopyCString(const char *in) {
	char *out = (char*) malloc(strlen(in) + 1);
	char *p = out;
	while ((*p++ = *in++) != '\0');
	*p = 0;
	return out;
}
