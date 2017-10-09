#include "structs.h"
#include <limits.h>

#define BUFFER1		10000

static unsigned int num_of_files;

/**
 * define_folio:	Fill a folio with naught.
 */
struct Folio *define_folio(struct Folio *folio)
{
	folio->fp = NULL;
	folio->name = NULL;
	folio->f_name = NULL;
	folio->c_pt = NULL;
	folio->head = NULL;
	folio->map_pos = NULL;
	folio->map_pt = 0;
	folio->lines = 0;
	folio->len = 0;
	folio->cur_page = 0;
	folio->cur_pos = 0;
	folio->total_pages = 0;
	return folio;
}

/**
 * init_folio:	Assign memory for array of folio structs.
 */
struct Folio *init_folio(const unsigned int num)
{
	struct Folio *book, *pt;
	size_t i;
	num_of_files = num;

	if ((book = pt = malloc(num * sizeof(struct Folio))) == NULL) {
		printf("error: malloc failed in %s ~ Folio.\n", __func__);
		exit(1);
	}

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
void read_file(struct Folio *folio)
{
	size_t i;
	int c, d, rows;
	char *pt;
	char **temp;

	rows = get_row();

	if ((temp = malloc(BUFFER1*sizeof(char*))) == NULL) {
		printf("error:	malloc failed to assign memory to temp in %s.", __func__);
		exit(1);
	}

	if (folio->fp == NULL) {
		printf("error:	The file pointer supplied to %s is NULL.\n", __func__);
		exit(1);
	}

	folio->len = file_size(folio->fp);
	folio->head = folio->c_pt = pt = malloc((folio->len * sizeof(int))+1);
	folio->lines = 1;

	/* Whilst copying the file into memory store the address of the first
	 * line of every new page in an array of char* */
	i = 0;
	temp[i++] = pt;

	while ((c = fgetc(folio->fp)) != EOF) {
		d = c;
		if (c == '\n' && folio->lines++ % (rows-OFFSET) == 0)
			temp[i++] = pt+1;	/* +1 skip over the '\n' */
		*pt++ = c;
	}

	folio->map_pt = i-1;	/* -1 for index */
	if (d != '\n')
		folio->lines++;

	*pt = '\0';
	fclose(folio->fp);
	folio->total_pages = (folio->lines / get_row())+1;
	folio->cur_page = 1;

	/* store map of page addresses */
	if ((folio->map_pos = malloc(folio->total_pages * sizeof(char*))) == NULL) {
		printf("error:	malloc failed to allocate map_pos in %s.\n", __func__);
		exit(1);
	}

	for (i = 0; i < folio->total_pages; i++)
		folio->map_pos[i] = temp[i];
	free(temp);
}

/**
 * refresh_pages:	Reset all page start pointers in page array.
 */
//void refresh_pages(struct Folio folio*)
//{
//	char *temp;
//	int rows;
//
//	if ((temp = malloc(BUFFER1*sizeof(char*))) == NULL) {
//		printf("error:	malloc failed to assign memory to temp in %s.", __func__);
//		exit(1);
//	}
//
//	rows = get_row();
//}

/**
 * set_filename:	Remove path from filename.
 */
char *set_filename(char* file_name)
{
	char *name;

	if ((name = strrchr(file_name, '/')) != NULL)
		return ++name;

	return file_name;
}

/**
 * write_to_heap:	Treat every file in argv[] list.
 */
struct Folio *write_to_heap(
		struct Folio *portfolio,
		struct Nav *nav,
		char* file_name,
		const int num_of_files)
{
	static int b_pt;

	if (b_pt < num_of_files) {
		if ((portfolio[b_pt].fp = fopen(file_name, "r")) == NULL) {
			printf("error:	%s is not a valid file, in %s.\n", file_name, __func__);
			exit(1);
		}
		portfolio[b_pt].name = file_name;
		portfolio[b_pt].f_name = set_filename(file_name);
		read_file(&portfolio[b_pt++]);
	} else {
		printf("error: to many files for current configuration in %s.\n", __func__);
		exit(1);
	}

	nav->f_count = b_pt;

	return portfolio;
}

/**
 * free_folio:	Free leaves everywhere.
 */
void free_folio(struct Folio *files, const size_t num)
{
	size_t i;
	for (i = 0; i < num; i++) {
		free(files[i].c_pt);
		free(files[i].map_pos);
	}
	free(files);
}

