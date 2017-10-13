#include "structs.h"
#include <string.h>
#include <unistd.h>
#include <sys/ioctl.h>
#include <signal.h>

#define FOUR_BYTES	30		/* Test for UTF-8 width */
#define THREE_BYTES	14
#define TWO_BYTES	6

static struct Screen screen;
static int tabwidth;

static refresh refresh_pf = (void (*)(int)) refresh_all;

/**
 * terminal_dimensions:	Get screen current dimentions from ioctl.
 */
int terminal_dimensions(struct Screen *sc)
{
	struct winsize win;

	if ((ioctl(0, TIOCGWINSZ, &win)) == -1)
		error(0, 0, "ioctl TIOCGWINSZ failed in %s().\n", __func__);

	sc->old_row = sc->row;
	sc->col = win.ws_col;
	sc->row = win.ws_row;
	sc->len = sc->col * sc->row * 4;	// * 4 for UTF-8 char
	sc->current_len = 0;
	return 1;
}

/**
 * init_screen:	assign screen memory.
 */
struct Screen *init_screen(void)
{
	if (!(terminal_dimensions(&screen)))
		error(0, 0, "terminal_dimensions failed in %s().\n", __func__);

	screen.display = malloc(screen.len+1);

	return &screen;
}

/**
 * init_listen:
 */
void init_listen(void)
{
	struct sigaction sa;
	sigemptyset(&sa.sa_mask);
	sa.sa_flags = 0;
	sa.sa_handler = refresh_pf;
	if (sigaction(SIGWINCH, &sa, NULL) == -1)
		error(0, 0, "sigaction failed in %s().\n", __func__);
}

/**
 * get_dimensions:	Check if terminal has been resized.
 */
int get_dimensions(void)
{
	if (!(terminal_dimensions(&screen)))
		error(0, 0, "terminal_dimensions failed in %s().\n", __func__);

	return 1;
}

/**
 * get_rows: Returns screen row count.
 */
int get_rows(void)
{
	return screen.row;
}

int get_old_rows(void)
{
	return screen.old_row;
}

/**
 * tab_check:
 */
short tab_check(short width)
{
	return (width < 100 && width >= 0) ? 1 : 0;
}

/**
 * set_tabwidth:	set the system tab width to specified ammount.
 */
void set_tabwidth(short width)
{
	char string[100] = { "tabs " };
	char num[2];
	tabwidth = width;

	if (!tab_check(tabwidth))
		error(0, 0, "tab width value in %s() is not accepted.\n", __func__);

	sprintf(num, "%d", width);
	system(strcat(string, num));
}

/**
 * get_tabwidth:	Returns the set tabstop width.
 */
short get_tabwidth(void)
{
	return (tab_check(tabwidth)) ? tabwidth : 0;
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
 * write_screen:	Write page into the screen struct.
 */
int write_screen(
		struct Folio *file,
		short tab,
		short key_pressed,
		short is_last)
{
	struct Screen *sc = &screen;
	size_t i, row, col;
	char *f_pt, *d_pt, *count;
	row = col = 0;

	if (!(i = navigate(file, key_pressed, is_last)))
		return i;

	d_pt = count = sc->display;
	f_pt = file->head;

	/* -OFFSET for cursor line and page header */
	for (i = 0 ; i < sc->len && row < sc->row-OFFSET; i++)
		if (*f_pt != '\0') {
			if (col < sc->col) {
				col += test_utf8((unsigned)*f_pt);
				if (*f_pt == '\t' && tab)
					col += tab-1;
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
					file->page_pt+1,
					file->page_count);
	sc->current_len = d_pt - count;

	return 1;
}

/**
 * blit_screen:	Write content of screen struct to stdout.
 */
void blit_screen(void)
{
	write(1, screen.display, screen.current_len);
}

/**
 * free_screen:	Get your free screen here.
 */
void free_screen(void)
{
	free(screen.display);
}

