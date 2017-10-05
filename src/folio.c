#include "structs.c"

static unsigned int num_of_files;

struct Window *define_folio(struct Window *folio)
{
	folio->fp = NULL;
	folio->name = NULL;
	folio->lines = 0;
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

	if ((pt = book = malloc(num * sizeof(struct Window))) == NULL)
		printf("error: malloc failed in init_folio().\n");

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
	int c, d;
	char* pt;

	if (folio->fp == NULL) {
		printf("error:	fp recieved by read_file() is NULL.\n");
		return 1;
	}

	folio->len = file_size(folio->fp);
	folio->content = folio->c_pt = pt = malloc((folio->len * sizeof(int))+1);

	while ((c = fgetc(folio->fp)) != EOF) {
		d = c;
		*pt++ = c ;
		if (c == '\n')
			folio->lines++;
	}

	if (d != '\n')
		folio->lines++;

	*pt = '\0';

	fclose(folio->fp);
	return 0;
}

/**
 * scan_files:	Treat every file in argv[] list.
 */
struct Window *scan_files(struct Window *portfolio, char* file_name, int num_of_files)
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

	return portfolio;
}

/**
 * free_folio:	free files memory.
 */
void free_folio(struct Window *files, size_t num)
{
	size_t i;
	for (i = 0; i < num; i++)
		free(files[i].c_pt);
	free(files);
}

