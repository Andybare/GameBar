#include "Constants.h"
size_t FileCallback(FILE* f, char* ptr, size_t size, size_t nmemb) {
	return fwrite(ptr, size, nmemb, f);
};