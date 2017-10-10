#include "print-files.h"

struct Nav {
	size_t f_count;			/* File index */
	size_t f_active;		/* File curently being read */
};

struct Folio {
	FILE *fp;
	char *name;			/* argv[] */
	char *f_name;			/* argv[] */
	char *c_pt;			/* Memory entry point */
	char *head;			/* Current pointer position in file */
	char **map_pos;		/* array of new line addresses */
	size_t page_pt;		/* Current page number */
	size_t page_count;	/* Total number of pages */
	size_t lines;			/* Line count of file */
	size_t len;			/* Lenth in char of file */
};

struct Screen{
	unsigned int col;
	unsigned int row;
	unsigned int len;
	unsigned int current_len;
	char *display;
};

