#include "print-files.h"

struct Window {
	FILE *fp;
	char *name;			/* argv[] */
	char *content;			/* Pointer to files content on the heap */
	char *c_pt;			/* Same as the above to free memory */
	char *head;			/* Current pointer position in file */
	char **map_pos;			/* array of new line addresses */
	size_t map_pt;			/* length of map_pos */
	size_t lines;			/* Line count of file */
	size_t len;			/* Lenth in char of file */
	size_t cur_page;		/* Current page number */
	size_t cur_pos;			/* Current line number */
	size_t total_pages;		/* Total number of pages */
};

struct Screen{
	unsigned int col;
	unsigned int row;
	unsigned int len;
	unsigned int current_len;
	char *display;
};

