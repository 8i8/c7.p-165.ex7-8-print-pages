#include "structs.h"
#include <unistd.h>
#include <limits.h>
#include <math.h>

#define BUFFER1		10000

static unsigned int num_of_files;
static struct Folio *portfolio = NULL;

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
	folio->page_pt = 0;
	folio->page_count = 0;
	folio->lines = 0;
	folio->len = 0;
	return folio;
}

/**
 * init_folio:	Assign memory for array of folio structs.
 */
struct Folio *init_folio(unsigned int num)
{
	char *msg = "error: malloc failed in init_folio() ~ Folio.\n";
	struct Folio *book, *folio_pt;
	size_t i;
	num_of_files = num;

	if ((portfolio = book = folio_pt = malloc(num * sizeof(struct Folio))) == NULL)
		write(2, msg, strlen(msg));

	for (i = 0; i < num; i++, book++)
		book = define_folio(book);

	return folio_pt;
}

/**
 * get_portfolio:	Returns a pointer to the portfolio.
 */
struct Folio *get_portfolio(void)
{
	return (portfolio) ? portfolio : NULL;
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
	char *msg1 = "error:	malloc failed to assign memory to temp in read_folio().\n";
	char *msg2 = "error:	The file pointer supplied to read_folio() is NULL.\n";
	char *msg3 = "error:	malloc failed to allocate map_pos in read_folio().\n";
	size_t i;
	int c, d, rows;
	char **temp, *f_pt;

	rows = get_rows();

	if ((temp = malloc(BUFFER1*sizeof(char*))) == NULL)
		write(2, msg1, strlen(msg1));

	if (folio->fp == NULL)
		write(2, msg2, strlen(msg2));

	folio->len = file_size(folio->fp);
	folio->head = folio->c_pt = f_pt = malloc((folio->len * sizeof(int))+1);

	/* Whilst copying the file into memory store the address of the first
	 * line of every new page in an array of char* */
	i = 0, folio->lines = 1;
	temp[i++] = f_pt;

	while ((c = fgetc(folio->fp)) != EOF) {
		d = c;
		if (c == '\n' && folio->lines++ % (rows-OFFSET) == 0)
			temp[i++] = f_pt+1;	/* +1 skip over the '\n' */
		*f_pt++ = c;
	}
	if (d != '\n')
		folio->lines++;

	*f_pt = '\0';
	fclose(folio->fp);

	folio->page_count = i;
	folio->page_pt = 0;

	/* store map of page addresses */
	if ((folio->map_pos = malloc(folio->page_count * sizeof(char*))) == NULL)
		write(2, msg3, strlen(msg3));

	for (i = 0; i < folio->page_count; i++)
		folio->map_pos[i] = temp[i];
	free(temp);

	return 0;
}

/**
 * translate_page_pt:	Set current page to closest equivalent.
 */
struct Folio *translate_page_pt(
		struct Folio *folio,
		size_t old_page_count,
		size_t old_page_pt)
{

	int p_pt;

	p_pt = round((float)folio->page_count / (float)old_page_count * (float)(old_page_pt+1)-1);

	if (p_pt < (int)folio->page_count && p_pt >= 0)
		folio->page_pt = p_pt;
	else if (p_pt > 0)
		folio->page_pt = 0;
	else if (p_pt >= (int)folio->page_count)
		folio->page_pt = folio->page_count-1;

	return folio;
}

/**
 * refresh_folio:	Reset all page start pointers in page array.
 */
void refresh_folio(struct Folio *folio)
{
	char *msg1 = "error:	malloc failed to assign memory to temp in refresh_folio().\n";
	char *msg2 = "error:	malloc failed to allocate map_pos in refresh_folio().\n";
	char **temp, *f_pt;
	size_t i, line, old_page_count, old_page_pt;
	int c, rows;

	old_page_count = folio->page_count;
	old_page_pt = folio->page_pt;

	rows = get_rows();
	f_pt = folio->c_pt;

	if ((temp = malloc(BUFFER1*sizeof(char*))) == NULL)
		write(2, msg1, strlen(msg1));

	/* Get the address of each new page */
	i = line = 0;
	temp[i++] = f_pt++;

	while ((c = *f_pt++))
		if (c == '\n' && ++line % (rows-OFFSET) == 0)
			temp[i++] = f_pt;

	folio->page_count = i;

	/* store map of page addresses */
	free(folio->map_pos);
	if ((folio->map_pos = malloc(folio->page_count * sizeof(char*))) == NULL)
		write(2, msg2, strlen(msg2));

	for (i = 0; i < folio->page_count; i++)
		folio->map_pos[i] = temp[i];
	free(temp);

	/* Put page pointer to appropriate page */
	folio = translate_page_pt(folio, old_page_count, old_page_pt);
}

/**
 * refresh_portfolio:	Reset all pointers to page beginnings for entire
 * portfolio.
 */
void refresh_portfolio(struct Folio *pf, struct Nav *nav)
{
	size_t i;
	for (i = 0; i < nav->f_count; i++)
		refresh_folio(&pf[i]);
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

