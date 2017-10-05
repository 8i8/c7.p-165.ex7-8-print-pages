#include "structs.c"
#include <unistd.h>
#include <sys/ioctl.h>
#include <string.h>

#define OFFSET		1		/* Offset cursor and header line */
#define FOUR_BYTES	30		/* Test for UTF-8 width */
#define THREE_BYTES	14
#define TWO_BYTES	6

enum inout { OUT, IN };

static struct Screen screen;

/**
 * get_dimensions:	Set screen size from ioctl.
 */
int get_dimensions(struct Screen *sc)
{
	struct winsize win;
	if ((ioctl(0, TIOCGWINSZ, &win)) == -1)
		return 0;
	sc->col = win.ws_col;
	sc->row = win.ws_row;
	sc->len = sc->col * sc->row * 4;	// 4 to allow for UTF-8
	sc->current_len = 0;
	return 1;
}

/**
 * clear_screen:
 */
void clear_screen(void)
{
	write(1,"\033[H\033[J", 6);	/* Clear screen */
}

/**
 * advance_to:	Advance to the specified line.
 */
char *advance_to(struct Window *file, size_t move)
{
	size_t i, j;
	char *fp;
	static size_t start;
	fp = file->content;

	/* OFFSET, account for the cursor and static display elements */
	switch (move)
	{
		case START: start = 0;
			break;
		case UP: if (start >= screen.row-OFFSET)
				start -= screen.row-OFFSET;
			break;
		case DOWN: if (start < (file->lines)-screen.row-OFFSET
					   && file->lines > screen.row-OFFSET)
				start += screen.row-OFFSET;
			break;
		default:
			break;
	}

	/* advance */
	for (i = 0, j = 0; i < file->len && j < start; i++)
		if (*(fp++) == '\n')
			j++;
	return fp;
}

/**
 * utf8_word_length:	Return multi-char length (-1) in bytes, read from the
 * initial UTF-8 char.
 */
unsigned utf8_wordlength(unsigned char a)
{
	if	(a >> 3 == FOUR_BYTES)
		return 3;
	else if (a >> 4 == THREE_BYTES)
		return 2;
	else if (a >> 5 == TWO_BYTES)
		return 1;
	return 0;
}

/**
 * test_utf8:	Keep track of UTF-8 char count and status.
 */
unsigned test_utf8(unsigned char a)
{
	static short unsigned count;

	if (count) {
		if(--count) 
			return 0;
		else
			return 1;
	} else if (a >> 7) {
		count = utf8_wordlength(a);
		if (count)
			return 0;
	}
	return 1;
}

/**
 * page_write:	Write one page of file into screen struct, essentially the
 * screen struct contains a char* string that is printed to screen when the
 * command is given, the folio struct is a files textual content, copied
 * whilst truncating any lines that are longer than the screen is wide.
 * Maintaining the position by way of the line number is the advance_to()
 * function, used to scroll page by page through the document.
 */
void page_write(struct Window *file, size_t line)
{
	size_t i, row, col;
	char *f_pt, *d_pt, *count;
	d_pt = count = screen.display;
	f_pt = advance_to(file, line);
	row = col = 0;

	/* -OFFSET for cursor line and page header */
	for (i = 0 ; i < screen.len && row < screen.row-OFFSET; i++)
		if (*f_pt != '\0') {
			if (col < screen.col) {
				col += test_utf8((unsigned)*f_pt);
				*d_pt++ = *f_pt++;
			} else {
				if (*f_pt == '\n')
					*d_pt++ = *f_pt++;
				else
					f_pt++;
			}
			if (*(f_pt-1) == '\n')
				row++, col = 0;
		} else
			*d_pt++ = '\n', row++;

	d_pt += sprintf(d_pt, "rows -> %u cols -> %u : ", screen.row, screen.col);
	screen.current_len = d_pt - count;
}

/**
 * blit_screen:	Write content of screen struct to STDOUT.
 */
void blit_screen(void)
{
	write(1, screen.display, screen.current_len);
}

/**
 * init_screen:	assign screen memory.
 */
struct Screen *init_screen(void)
{
	if (!(get_dimensions(&screen)))
		printf("error:	get_dimensions failed in init_screen\n");

	screen.display = malloc((screen.len * sizeof(char))+1);
	//clear_screen();

	return &screen;
}

void free_screen(void)
{
	free(screen.display);
}

