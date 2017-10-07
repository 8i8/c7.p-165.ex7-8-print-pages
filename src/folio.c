#include "structs.c"
#include <limits.h>

#define BUFFER1		10000

static unsigned int num_of_files;

struct Window *define_folio(struct Window *folio)
{
	folio->fp = NULL;
	folio->name = NULL;
	folio->content = NULL;
	folio->c_pt = NULL;
	folio->head = NULL;
	folio->lines = 0;
	folio->len = 0;
	folio->cur_page = 0;
	folio->cur_pos = 0;
	folio->total_pages = 0;
	return folio;
}

/**
 * init_folio:	Assign memory for array.
 */
struct Window *init_folio(unsigned int num)
{
	struct Window *book, *pt;
	size_t i;
	num_of_files = num;

	if ((book = pt = malloc(num * sizeof(struct Window))) == NULL)
		printf("error: malloc failed in init_folio() ~ Window.\n");

	for (i = 0; i < num; i++, book++)
		book = define_folio(book);

	return pt;
}

/**
 * file_size:	Read and return file length.
 */
static size_t file_size(FILE *fp)
{
	size_t len;
	fseek(fp, 0L, SEEK_END);
	len = (unsigned)ftell(fp);
	rewind(fp);
	return len;
}

/**
 * read_file:	Copy file into programs heap memory.
 */
int read_file(struct Window *folio)
{
	size_t i;
	int c, d, rows;
	char *pt;
	char **temp;

	rows = get_row();

	if ((temp = malloc(BUFFER1*sizeof(char*))) == NULL)
		printf("error:	mallloc failed to assign memory to temp in read_file().");

	if (folio->fp == NULL) {
		printf("error:	fp recieved by read_file() is NULL.\n");
		return 1;
	}
	folio->len = file_size(folio->fp);
	folio->head = folio->c_pt = pt = malloc((folio->len * sizeof(int))+1);

	i = 0;
	temp[i++] = pt;
	while ((c = fgetc(folio->fp)) != EOF) {
		d = c;
		if ( c  == '\n') {
			if (folio->lines % rows == 0)
				temp[i++] = pt;
			folio->lines++;
		}
		*pt++ = c;
	}

	if (d != '\n') {
		folio->lines++;
	}

	/* add one to correct index offset */
	folio->lines++;
	*pt = '\0';
	fclose(folio->fp);

	/* store map of new lines */
	folio->map_pos = malloc(folio->lines * sizeof(char*));
	for (i = 0; i < folio->lines; i++)
		folio->map_pos[i] = temp[i];
	free(temp);

	folio->total_pages = (folio->lines / get_row()) + 1;
	folio->cur_page = 1;

	return 0;
}

/**
 * scan_files:	Treat every file in argv[] list.
 */
struct Window *scan_files(struct Window *portfolio, struct Nav *nav, char* file_name, int num_of_files)
{
	static int b_pt;
	
	if (b_pt < num_of_files) {
		if ((portfolio[b_pt].fp = fopen(file_name, "r")) == NULL)
			return NULL;
		portfolio[b_pt].name = file_name;
		if (read_file(&portfolio[b_pt++]))
			printf("error:	read_file error in scan_files.\n");
	} else
		printf("error: to many files for current configuration.\n");

	nav->f_count = b_pt;

	return portfolio;
}

/**
 * free_folio:	free files memory.
 */
void free_folio(struct Window *files, size_t num)
{
	size_t i;
	for (i = 0; i < num; i++) {
		free(files[i].c_pt);
		free(files[i].map_pos);
	}
	free(files);

}

