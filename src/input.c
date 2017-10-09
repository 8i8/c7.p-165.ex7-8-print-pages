#include "structs.c"
#include <termios.h>
#include <ctype.h>
#include <string.h>
#include <unistd.h>

/**
 * get_flags:	Get input from and count flags.
 */
int get_flags(char *argv)
{
	size_t i;
	int c;
	i = 0;

	while ((c = *(argv+(++i)) != isspace(c))) {
		switch (c) {
			case 'n': printf("Hello World.");
				break;
			default:
				printf("usage: %s <file1> <file2> ...\n", argv);
		}
	}
	return 1;
}

/**
 * get_files:	Get input from each argv that is not a flag.
 */
void get_files(char *argv, struct Folio *pf, struct Nav *nav, int input)
{
	if ((pf = scan_files(pf, nav, argv, input)) == NULL)
		printf("usage: %s <file1> <file2> ...\n", argv);
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
void turn_page(struct Folio *pf, struct Nav *nav, short dir, short end)
{
	if (dir == LEFT) {
		if (!next_file(nav, LEFT)) {
			pf[nav->f_active].cur_page = pf[nav->f_active].total_pages;
			pf[nav->f_active].head = pf[nav->f_active].map_pos[pf[nav->f_active].map_pt];
			write_screen(&pf[nav->f_active], dir, end);
		}
	} else if (dir == RIGHT) {
		if (!next_file(nav, RIGHT)) {
			pf[nav->f_active].cur_page = 1;
			pf[nav->f_active].head = pf[nav->f_active].map_pos[0];
			write_screen(&pf[nav->f_active], dir, end);
		}
	}
}

/**
 * get_input:	Keyboard input.
 */
void get_input(struct Folio *portfolio, struct Nav *nav, int c)
{
	if (c == '\033') {
		readchar();
		c = readchar();
	}

	switch (c)
	{
		case START: write_screen(&portfolio[nav->f_active], START, NO);
			  break;
		case 'k': if ((write_screen(&portfolio[nav->f_active], UP, NO)))
				  turn_page(portfolio, nav, LEFT, YES);
			  break;
		case 'A': if ((write_screen(&portfolio[nav->f_active], UP, NO)))
				  turn_page(portfolio, nav, LEFT, YES);
			  break;
		case 'j': if ((write_screen(&portfolio[nav->f_active], DOWN, NO)))
				  turn_page(portfolio, nav, RIGHT, YES);
			  break;
		case 'B': if ((write_screen(&portfolio[nav->f_active], DOWN, NO)))
				  turn_page(portfolio, nav, RIGHT, YES);
			  break;
		case 'h': next_file(nav, LEFT);
			  write_screen(&portfolio[nav->f_active], LEFT, NO);
			  break;
		case 'C': next_file(nav, RIGHT);
			  write_screen(&portfolio[nav->f_active], RIGHT, NO);
			  break;
		case 'l': next_file(nav, RIGHT);
			  write_screen(&portfolio[nav->f_active], RIGHT, NO);
			  break;
		case 'D': next_file(nav, LEFT);
			  write_screen(&portfolio[nav->f_active], LEFT, NO);
			  break;
		default: 
			  break;
	}
}

/**
 * navigate:	Set file->head to the position required for printing to screen.
 */
int navigate(struct Folio *file, const short move, const short last)
{
	size_t start;
	start = file->cur_pos;

	/* OFFSET, account for the cursor and static display elements */
	switch (move)
	{
		case START: start = 0, file->cur_page = 1;
			break;
		case UP: if (file->cur_page > 1) {
				if (last)
					file->head = file->map_pos[(file->cur_page)-1];
			 	else
					file->head = file->map_pos[(--file->cur_page)-1];
			} else
				return 1;
			break;
		case DOWN: if (file->cur_page < file->total_pages)
				if (last)
					file->head = file->map_pos[(file->cur_page)-1];
				else
					file->head = file->map_pos[(++file->cur_page)-1];
			else
				return 1;
			break;
		default:
			break;
	}

	file->cur_pos = start;

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

