#include "print-files.h"

struct Window {
	FILE *fp;
	char *name;
	char *content;
	char *c_pt;
	size_t lines;
	size_t len;
};

struct Screen{
	unsigned int col;
	unsigned int row;
	unsigned int len;
	unsigned int current_len;
	char *display;
};

