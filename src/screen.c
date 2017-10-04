#include "structs.c"
#include <unistd.h>
#include <sys/ioctl.h>
#include <string.h>

#define OFFSET	1	/* Offset cursor and header line */
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
	sc->len = sc->col * sc->row;
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

	/* OFFSET to account for the cursor and static display elements */
	switch (move)
	{
		case START: start = 0;
			break;
		case UP: if (start >= screen.row-OFFSET)
				start -= screen.row-OFFSET;
			break;
		case DOWN: if (start < (file->lines)-screen.row-OFFSET)
				start += screen.row-OFFSET;
			break;
		default:
			break;
	}

	/* advance to start */
	for (i = 0, j = 0; i < file->len && j < start; i++)
		if (*(fp++) == '\n')
			j++;
	return fp;
}

/**
 * test_utf8:	Keep track of UTF-8 char count and status.
 */
int test_utf8(char a)
{
	static short prev;

	if (a >> 7 && !prev) {
		prev = IN;
		return 1;
	} else if (a >> 7 && prev) {
		prev = OUT;
		return 0;
	}
	return 1;
}

/**
 * write_page:	Write one page of file into screen struct.
 */
void write_page(struct Window *file, size_t line)
{
	size_t i, j, k, l;
	int io;
	char *f_pt, *d_pt;
	d_pt = screen.display;
	f_pt = advance_to(file, line);

	i = 0; // Iteration.
	j = 0; // Row count.
	k = 0; // Count of char used in line, for no wrap.
	l = 0; // Total length for write().
	io = 0; // In UTF-8 char, do not stop line yet.

	for ( ; i < screen.len && j < screen.row-OFFSET; i++)

		/* -OFFSET cursor line and page header */
		if (*f_pt != '\0') {
			/* No line wrap */
			if (k < screen.col+23)
			{
				if (!(k += test_utf8(*f_pt)))
					io = IN;
				else
					io = OUT;
				*d_pt++ = *f_pt++, l++;
			}
			else if (k >= screen.col && io == IN)
			{
				if (k += test_utf8(*f_pt)) {
					io = OUT;
					if (*f_pt == '\n')
						*d_pt++ = *f_pt++, l++;
				} else
					*d_pt++ = *f_pt++, l++;
			}
			else if (*f_pt == '\n')
				*d_pt++ = *f_pt++, l++;
			else
				f_pt++;
			if (*f_pt == '\n')
				j++, k = 0;
		} else
			*d_pt++ = '\n', j++, l++;

	/* New line for cursor input */
	*d_pt++ = '\n', l++;

	l += sprintf(d_pt, "rows -> %u: ", screen.row);

	screen.current_len = l;
}

/**
 * blit_screen:	Write content of screen struct to the terminal.
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

	screen.display = malloc((screen.len * sizeof(int))+1);
	clear_screen();

	return &screen;
}

void free_screen(void)
{
	free(screen.display);
}

