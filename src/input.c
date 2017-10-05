#include "structs.c"
#include <termios.h>
#include <string.h>
#include <unistd.h>

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
 * get_input:	Keyboard input.
 */
void get_input(struct Window *files, int c)
{
	if (c == '\033') {
		readchar();
		c = readchar();
	}

	switch (c)
	{
		case START: page_write(&files[0], START);
			  break;
		case 'k': page_write(&files[0], UP);
			  break;
		case 'A': page_write(&files[0], UP);
			  break;
		case 'j': page_write(&files[0], DOWN);
			  break;
		case 'B': page_write(&files[0], DOWN);
			  break;
		default: 
			  break;
	}
}

