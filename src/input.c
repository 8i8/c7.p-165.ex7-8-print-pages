#include "structs.h"
#include <termios.h>
#include <ctype.h>
#include <string.h>
#include <unistd.h>

/**
 * get_flags:	Get input from and count flags.
 */
int get_flags(const char *argv)
{
	char c;

	while ((c = *++argv) && !isspace(c)) {
		switch (c) {
			case 'n': printf("Hello World.\n");
				break;
			default:
				printf("error: flag '%c' unknown.\n", c);
				exit(1);
				break;
		}
	}
	return 1;
}

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
 * read_arg:	Try to open and store file pointers in an array of structs.
 */
void read_arg(char *file_name, struct Folio *pf, struct Nav *nav, int num)
{
	static int f_pt;

	if (f_pt < num) {
		if ((pf[f_pt].fp = fopen(file_name, "r")) == NULL) {
			printf("error:	%s is not a valid file, in %s.\n", file_name, __func__);
			exit(1);
		}
		pf[f_pt].name = file_name;
		pf[f_pt].f_name = set_filename(file_name);
		read_folio(&pf[f_pt++]);
	} else {
		printf("error: to many files for current configuration in %s.\n", __func__);
		exit(1);
	}

	nav->f_count = f_pt;
}

/**
 * init_nav:	Returns nav struct to main for use in parsing navigational
 * information.
 */
struct Nav *init_nav(struct Nav *nav)
{
	nav = malloc(sizeof(struct Nav));
	nav->f_count = 0;
	nav->f_active = 0;

	return nav;
}

/**
 * readchar:	Read stdin in raw mode, to deal with arrow keys.
 */
int readchar(void)
{
	static struct termios term, oterm;
	char str[1];
	str[0] = 0;
	if ((tcgetattr(0, &oterm)) != 0)
		return -1;
	memcpy(&term, &oterm, sizeof(term));
	term.c_lflag &= ~(ICANON | ECHO);
	term.c_cc[VMIN] = 1;
	term.c_cc[VTIME] = 0;
	if ((tcsetattr(0, TCSANOW, &term)) != 0 )
		return -1;
	read(0, str, 1);
	write(1, "\n", 1);
	if ((tcsetattr(0, TCSANOW, &oterm)) != 0 )
		return -1;
	return str[0];
}

/**
 * next_file:	Set the desired file number in nav struct, used when changing
 * the dislayed file.
 */
short next_file(struct Nav *nav, short next)
{
	if (next == RIGHT) {
		if (nav->f_active < nav->f_count-1)
			nav->f_active++;
		else
			return 1;
	} else if (next == LEFT) {
		if (nav->f_active > 0)
			nav->f_active--;
		else
			return 1;
	}
	return 0;
}

/**
 * turn_page:	Called when the file navigation reaches an extremity, if there
 * is a file before or after the present, in the direction being traveled, move
 * to the relevant page in that file, eiter the first or last page of that file.
 */
void turn_page(
		struct Folio *pf,
		struct Nav *nav,
		short tab,
		short dir,
		short end)
{
	if (dir == LEFT) {
		if (!next_file(nav, LEFT)) {
			pf[nav->f_active].page_pt = pf[nav->f_active].page_count-1;
			pf[nav->f_active].head = pf[nav->f_active].map_pos[pf[nav->f_active].page_count-1];
			write_screen(&pf[nav->f_active], tab, dir, end);
		}
	} else if (dir == RIGHT) {
		if (!next_file(nav, RIGHT)) {
			pf[nav->f_active].page_pt = 0;
			pf[nav->f_active].head = pf[nav->f_active].map_pos[0];
			write_screen(&pf[nav->f_active], tab, dir, end);
		}
	}
}

/**
 * get_input:	Keyboard input.
 */
void get_input(struct Folio *portfolio, struct Nav *nav, int c, short tab)
{
	if (c == '\033') {
		readchar();
		c = readchar();
	}

	switch (c)
	{
		case START: write_screen(&portfolio[nav->f_active], tab, START, CONT);
			  break;
		case 'k': if ((write_screen(&portfolio[nav->f_active], tab, UP, CONT)))
				  turn_page(portfolio, nav, tab, LEFT, STOP);
			  break;
		case 'A': if ((write_screen(&portfolio[nav->f_active], tab, UP, CONT)))
				  turn_page(portfolio, nav, tab, LEFT, STOP);
			  break;
		case 'j': if ((write_screen(&portfolio[nav->f_active], tab, DOWN, CONT)))
				  turn_page(portfolio, nav, tab, RIGHT, STOP);
			  break;
		case 'B': if ((write_screen(&portfolio[nav->f_active], tab, DOWN, CONT)))
				  turn_page(portfolio, nav, tab, RIGHT, STOP);
			  break;
		case 'h': next_file(nav, LEFT);
			  write_screen(&portfolio[nav->f_active], tab, LEFT, CONT);
			  break;
		case 'C': next_file(nav, RIGHT);
			  write_screen(&portfolio[nav->f_active], tab, RIGHT, CONT);
			  break;
		case 'l': next_file(nav, RIGHT);
			  write_screen(&portfolio[nav->f_active], tab, RIGHT, CONT);
			  break;
		case 'D': next_file(nav, LEFT);
			  write_screen(&portfolio[nav->f_active], tab, LEFT, CONT);
			  break;
		default: 
			  break;
	}
}

/**
 * navigate:	Set file->head to the position required for printing to screen.
 */
int navigate(struct Folio *file, short move, short last)
{
	/* OFFSET, account for the cursor and static display elements */
	switch (move)
	{
		case START: file->page_pt = 0;
			break;
		case UP: if (file->page_pt > 0) {
				if (!last)
					file->head = file->map_pos[--file->page_pt];
			} else
				return 1;
			break;
		case DOWN: if (file->page_pt < file->page_count-1) {
				if (!last)
					file->head = file->map_pos[++file->page_pt];
			} else
				return 1;
			break;
		default:
			break;
	}

	return 0;
}

/**
 * free_nav:	That is right, the nav are underattack and have been trapped
 * here in this function. Your task ... Should you accept it, to free all the
 * navi; By smply remembering to call this function.
 */
void free_nav(struct Nav *nav)
{
	free(nav);
}

