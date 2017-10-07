#include "structs.c"
#include <termios.h>
#include <string.h>
#include <unistd.h>

/**
 * init_nav:	Sends out nav struct to main for use as a marker for
 * navigational information.
 */
struct Nav *init_nav(struct Nav *nav)
{
	nav = malloc(sizeof(struct Nav));
	nav->f_count = 0;
	nav->f_active = 0;

	return nav;
}

/**
 * readchar:	Direct input using raw mode, to deal with arrow keys.
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
 * next_file:	Set next file number in nav struct.
 */
void next_file(struct Nav *nav, short next)
{
	if (next == RIGHT && nav->f_active < nav->f_count-1)
		nav->f_active++;
	else if (next == LEFT && nav->f_active > 0)
		nav->f_active--;
}

/**
 * get_input:	Keyboard input.
 */
void get_input(struct Window *portfolio, struct Nav *nav, int c)
{
	short turn_page;

	if (c == '\033') {
		readchar();
		c = readchar();
	}

	switch (c)
	{
		case START: page_content(&portfolio[nav->f_active], START);
			  break;
		case 'k': if ((turn_page = page_content(&portfolio[nav->f_active], UP))) {
				next_file(nav, LEFT);
				page_content(&portfolio[nav->f_active], LEFT);
			  }
			  break;
		case 'A': if ((turn_page = page_content(&portfolio[nav->f_active], UP))) {
				next_file(nav, LEFT);
				get_input(portfolio, nav, LEFT);
			  }
			  break;
		case 'j': if ((turn_page = page_content(&portfolio[nav->f_active], DOWN))) {
				next_file(nav, RIGHT);
				page_content(&portfolio[nav->f_active], RIGHT);
			  }
			  break;
		case 'B': if ((turn_page = page_content(&portfolio[nav->f_active], DOWN))) {
				next_file(nav, RIGHT);
				page_content(&portfolio[nav->f_active], RIGHT);
			  }
			  break;
		case 'h': next_file(nav, LEFT);
			  page_content(&portfolio[nav->f_active], LEFT);
			  break;
		case 'C': next_file(nav, RIGHT);
			  page_content(&portfolio[nav->f_active], RIGHT);
			  break;
		case 'l': next_file(nav, RIGHT);
			  page_content(&portfolio[nav->f_active], RIGHT);
			  break;
		case 'D': next_file(nav, LEFT);
			  page_content(&portfolio[nav->f_active], LEFT);
			  break;
		default: 
			  break;
	}
}

/**
 * navigate:	Advance to the specified line. TODO this code could be made
 * more efficient by holding the last position of the char star pointer and
 * moving from there.
 */
int navigate(struct Window *file, short move)
{
	size_t start;
	start = file->cur_pos;

	/* OFFSET, account for the cursor and static display elements */
	switch (move)
	{
		case START: start = 0, file->cur_page = 1;
			break;
		case UP: if (file->cur_page > 1)
				file->head = file->map_pos[(--file->cur_page)-1];
			else
				return 1;
			break;
		case DOWN: if (file->cur_page < file->total_pages)
				file->head = file->map_pos[(++file->cur_page)-1];
			else
				return 2;
			break;
		default:
			break;
	}

	file->cur_pos = start;

	return 0;
}

/**
 * free_nav:
 */
void free_nav(struct Nav *nav)
{
	free(nav);
}

