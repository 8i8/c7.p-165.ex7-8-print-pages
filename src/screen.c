#include "structs.c"
#include <unistd.h>
#include <sys/ioctl.h>

#define FOUR_BYTES	30		/* Test for UTF-8 width */
#define THREE_BYTES	14
#define TWO_BYTES	6

enum inout { OUT, IN };

static struct Screen screen;

/**
 * get_dimensions:	Get screen current dimentions from ioctl.
 */
int get_dimensions(struct Screen *sc)
{
	struct winsize win;
	if ((ioctl(0, TIOCGWINSZ, &win)) == -1)
		return 0;
	sc->col = win.ws_col;
	sc->row = win.ws_row;
	sc->len = sc->col * sc->row * 4;	// * 4 for UTF-8 char
	sc->current_len = 0;
	return 1;
}

/**
 * get_row: Returns screen row count.
 */
int get_row(void)
{
	return screen.row;
}

/**
 * utf8_word_length:	Return multi-char length (-1) in bytes, read from the
 * initial UTF-8 char.
 */
unsigned utf8_wordlength(const unsigned char a)
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
unsigned test_utf8(const unsigned char a)
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
 * write_screen:	Write one page of file into screen struct.
 */
int write_screen(
		struct Folio *file,
		const short key_pressed,
		const short is_last)
{
	struct Screen *sc = &screen;
	size_t i, row, col;
	char *f_pt, *d_pt, *count;
	row = col = 0;

	if ((i = navigate(file, key_pressed, is_last)))
		return i;

	d_pt = count = sc->display;
	f_pt = file->head;


	/* -OFFSET for cursor line and page header */
	for (i = 0 ; i < sc->len && row < sc->row-OFFSET; i++)
		if (*f_pt != '\0') {
			if (col < sc->col) {
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

	d_pt += sprintf(d_pt, "%s ~ Page %lu of %lu",
					file->f_name,
					file->cur_page,
					file->total_pages);
	sc->current_len = d_pt - count;

	return 0;
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

	return &screen;
}

/**
 * free_screen:	Get your free screen here.
 */
void free_screen(void)
{
	free(screen.display);
}

