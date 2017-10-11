#include "structs.h"
#include <termios.h>
#include <ctype.h>
#include <string.h>
#include <unistd.h>

static struct Nav *navigation;

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
 * remove_path:	Remove path from filename.
 */
char *remove_path(char* file_name)
{
	char *name;

	if ((name = strrchr(file_name, '/')) != NULL)
		return ++name;

	return file_name;
}

/**
 * read_arg:	Open and store all file pointers in an array of structs.
 * call to read_files() to put files into memory.
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
		pf[f_pt].f_name = remove_path(file_name);
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
	navigation = nav = malloc(sizeof(struct Nav));
	nav->f_count = 0;
	nav->f_active = 0;

	return nav;
}

/**
 * get_nav:	Returns a pointer to the global nav struct.
 */
struct Nav *get_nav(void)
{
	return (navigation) ? navigation : NULL;
}

/**
 * readchar:	Read stdin in raw mode, to deal with arrow keys.
 */
int readchar(void)
{
	static struct termios term, oterm;
	char str[1];
	str[0] = 0;
	if ((tcgetattr(0, &oterm)) != 0) {
		printf("error: failed to store oterm in %s.", __func__);
		exit(1);
	}
	memcpy(&term, &oterm, sizeof(term));
	term.c_lflag &= ~(ICANON | ECHO);
	term.c_cc[VMIN] = 1;
	term.c_cc[VTIME] = 0;
	if ((tcsetattr(0, TCSANOW, &term)) != 0 ) {
		printf("error: failed to set new state to term in %s.", __func__);
		exit(1);
	}
	read(0, str, 1);
	write(1, "\n", 1);
	if ((tcsetattr(0, TCSANOW, &oterm)) != 0 ) {
		printf("error: failed to reset state from oterm in %s.", __func__);
		exit(1);
	}
	return str[0];
}

/**
 * next_file:	Set the desired file number in nav struct, used when changing
 * the displayed file.
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
 * is a file before or after the current, in the direction being traversed,
 * move to the relevant page in that file, either the first or last dependant
 * on the direction.
 */
void turn_page(
		struct Folio *portfolio,
		struct Nav *nav,
		short tab,
		short dir,
		short end)
{
	struct Folio *folio;
	folio = &portfolio[nav->f_active];

	if (dir == LEFT) {
		if (!next_file(nav, LEFT)) {
			--folio;
			folio->page_pt = folio->page_count-1;
			folio->head = folio->map_pos[folio->page_pt];
			write_screen(folio, tab, dir, end);
		}
	} else if (dir == RIGHT) {
		if (!next_file(nav, RIGHT)) {
			++folio;
			folio->page_pt = 0;
			folio->head = folio->map_pos[folio->page_pt];
			write_screen(folio, tab, dir, end);
		}
	}
}

/**
 * get_input:	Keyboard input.
 */
void get_input(struct Folio *portfolio, struct Nav *nav, int c, short tab)
{
	struct Folio *folio;
	folio = &portfolio[nav->f_active];

	if (c == 033) {
		readchar();
		c = readchar();
	}

	switch (c)
	{
		case START: write_screen(folio, tab, START, CONT);
			  break;
		case 'k': if ((write_screen(folio, tab, UP, CONT)))
				  turn_page(portfolio, nav, tab, LEFT, STOP);
			  break;
		case 'A': if ((write_screen(folio, tab, UP, CONT)))
				  turn_page(portfolio, nav, tab, LEFT, STOP);
			  break;
		case 'j': if ((write_screen(folio, tab, DOWN, CONT)))
				  turn_page(portfolio, nav, tab, RIGHT, STOP);
			  break;
		case 'B': if ((write_screen(folio, tab, DOWN, CONT)))
				  turn_page(portfolio, nav, tab, RIGHT, STOP);
			  break;
		case 'h': if (!next_file(nav, LEFT))
			  	write_screen(--folio, tab, LEFT, CONT);
			  break;
		case 'D': if (!next_file(nav, LEFT))
			  	write_screen(--folio, tab, LEFT, CONT);
			  break;
		case 'C': if (!next_file(nav, RIGHT))
			  	write_screen(++folio, tab, RIGHT, CONT);
			  break;
		case 'l': if (!next_file(nav, RIGHT))
			  	write_screen(++folio, tab, RIGHT, CONT);
			  break;
		default: 
			  break;
	}
}

/**
 * navigate:	Set file->head to the position required for printing to screen.
 */
int navigate(struct Folio *folio, short move, short last)
{
	switch (move)
	{
		case START: folio->page_pt = 0;
			break;
		case UP: if (folio->page_pt > 0) {
				if (!last)
					folio->head = folio->map_pos[--folio->page_pt];
			} else
				return 1;
			break;
		case DOWN: if (folio->page_pt < folio->page_count-1) {
				if (!last)
					folio->head = folio->map_pos[++folio->page_pt];
			} else
				return 1;
			break;
		case LEFT: if (!last)
				folio->head = folio->map_pos[folio->page_pt];
			break;
		case RIGHT: if (!last)
				folio->head = folio->map_pos[folio->page_pt];
			break;
		case STATIC: folio->head = folio->map_pos[folio->page_pt];
			break;
		default:
			break;
	}
	return 0;
}

/**
 * refresh_all:	Reload after screen resize detected. This function assumes that
 * all values are set and that the code is working correctly, as it is called
 * only after a screen resize, TODO error protection should be added.
 */
void refresh_all(void)
{
	get_dimensions();
	refresh_portfolio(get_portfolio(), get_nav(), get_tabwidth());
}

/**
 * free_nav:	That is right, the nav are under attack, and are now trapped
 * here in this function; Your task ... Should you choose to accept, to free
 * all of them, by simply remember to call this function when you are done.
 */
void free_nav(struct Nav *nav)
{
	free(nav);
}

