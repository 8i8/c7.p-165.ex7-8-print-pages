#include "structs.h"
#include <unistd.h>
#include <limits.h>

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

	rows = get_row();

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
 * shift_page:	Set current page to closest equivalent that is either equal to, or
 * lower than the previous value.
 */
struct Folio *translate_page_pt(struct Folio *folio, char *old_address)
{
	short detect = 0;
	if (folio->map_pos[folio->page_pt] < old_address) {
		while (folio->map_pos[folio->page_pt] < old_address)
			if (folio->page_pt < folio->page_count)
				++folio->page_pt;
			else
				detect = 1;
		if (!detect)
			--folio->page_pt;
	} else
		while (folio->map_pos[folio->page_pt] > old_address)
			if (folio->page_pt > 0)
				--folio->page_pt;

	return folio;
}

/**
 * refresh_pages:	Reset all page start pointers in page array.
 */
void refresh_folio(struct Folio *folio)
{
	char *msg1 = "error:	malloc failed to assign memory to temp in refresh_folio().\n";
	char *msg2 = "error:	malloc failed to allocate map_pos in refresh_folio().\n";
	char **temp, *f_pt, *old_address;
	size_t i, line;
	int c, rows;

	old_address = folio->map_pos[folio->page_pt];
	rows = get_row();
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
	folio = translate_page_pt(folio, old_address);
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

