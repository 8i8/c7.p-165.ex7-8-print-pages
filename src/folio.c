#include "structs.h"
#include <unistd.h>
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
	folio->lines = 0;
	folio->len = 0;
	folio->page_pt = 0;
	folio->page_count = 0;
	return folio;
}

/**
 * init_folio:	Assign memory for array of folio structs.
 */
struct Folio *init_folio(unsigned int num)
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
 * read_folio:	Copy file into programs heap memory.
 */
int read_folio(struct Folio *folio)
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
	folio->head = folio->c_pt = malloc((folio->len * sizeof(int))+1);
	pt = folio->head;

	/* Whilst copying the file into memory store the address of the first
	 * line of every new page in an array of char* */
	i = 0, folio->lines = 1;
	temp[i++] = pt;

	while ((c = fgetc(folio->fp)) != EOF) {
		d = c;
		if (c == '\n' && folio->lines++ % (rows-OFFSET) == 0)
			temp[i++] = pt+1;	/* +1 skip over the '\n' */
		*pt++ = c;
	}
	if (d != '\n')
		folio->lines++;

	*pt = '\0';
	fclose(folio->fp);

	folio->page_count = i;
	folio->page_pt = 0;

	/* store map of page addresses */
	if ((folio->map_pos = malloc(folio->page_count * sizeof(char*))) == NULL) {
		printf("error:	malloc failed to allocate map_pos in %s.\n", __func__);
		exit(1);
	}

	for (i = 0; i < folio->page_count; i++)
		folio->map_pos[i] = temp[i];
	free(temp);

	return 0;
}

/**
 * shift_page:	Move current page to closest equivalent in new index.
 */
struct Folio *transfer_page_pt(
		struct Folio *folio,
		unsigned old_count,
		unsigned old_page)
{
	float c;

	c = (float)old_count/old_page+1;
	c = folio->page_count/c;
	folio->page_pt = (unsigned)c;
	folio->page_pt++;

	return folio;
}

/**
 * refresh_pages:	Reset all page start pointers in page array.
 */
void refresh_folio(struct Folio *folio)
{
	char **temp;
	size_t i, j, old_pt, old_count;
	int c, rows;

	old_count = folio->page_count;
	old_pt = folio->page_pt;
	rows = get_row();
	folio->head = folio->c_pt;

	if ((temp = malloc(BUFFER1*sizeof(char*))) == NULL) {
		printf("error:	malloc failed to assign memory to temp in %s.", __func__);
		exit(1);
	}

	/* Get the address of each new page */
	i = 0, j = 1;
	temp[i++] = folio->head++;

	while ((c = *folio->head++))
		if (c == '\n' && j++ % (rows-OFFSET) == 0)
			temp[i++] = folio->head;

	folio->page_count = i;

	free(folio->map_pos);
	/* store map of page addresses */
	if ((folio->map_pos = malloc(folio->page_count * sizeof(char*))) == NULL) {
		printf("error:	malloc failed to allocate map_pos in %s.\n", __func__);
		exit(1);
	}

	for (i = 0; i < folio->page_count; i++)
		folio->map_pos[i] = temp[i];
	free(temp);

	/* Put page pointer to appropriate page */
	folio = transfer_page_pt(folio, old_count, old_pt);
}

/**
 * refresh_portfolio:	Reset all pointers to page beginnings for entire
 * portfolio.
 */
void refresh_portfolio(struct Folio *pf, struct Nav *nav, short tabwidth)
{
	size_t i = 0;
	for (i = 0; i < nav->f_count; i++)
		refresh_folio(&pf[i]);
	write(1, "\n", 1);
	write_screen(&pf[nav->f_active], tabwidth, STATIC, CONT);
	blit_screen();
}

/**
 * free_folio:	Free leaves everywhere.
 */
void free_folio(struct Folio *files, size_t num)
{
	size_t i;
	for (i = 0; i < num; i++) {
		free(files[i].c_pt);
		free(files[i].map_pos);
	}
	free(files);
}

