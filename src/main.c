/*
 * K&R edition 2
 *
 * Exercise 7-8. Write a program to print a set of files, starting each new one
 * on a new page, with a title and a running page count for each file.
 *
 * <<< more_or_less >>>
 *
 * The program is designed to open several text files simultaneously, the user
 * can navigate these file using either the vim style nav bindings or the arrow
 * keys, j, k, <down> <up>, scroll up and down, and h, l <left> <right> jump to
 * either the next or the previous file.
 *
 * essentially the screen struct.hontains a char* string which is printed to
 * screen after each navigation command given, the folio struct stores each
 * file, which supplied at runtime as argv[] inputs, the textual content
 * scanned and put into memory, this is copied into the screen struct, in part
 * or in its entirety if the file is small enough, truncating any lines that
 * are longer than the screen is wide.
 */
#include "structs.h"

//#include <signal.h>
//
//typedef void (*refresh)(int);
//static refresh refresh_pf = (void (*)(int)) refresh_all;
//
//void refresh_all(int i)
//{
//	check_resize(1);
//	refresh_portfolio(portfolio, nav);
//}

int main(int argc, char *argv[])
{
	struct Folio *portfolio;
	struct Nav *nav = NULL;
	int i, c, flags, f_count, tabwidth;
	c = START;
	tabwidth = 8;

	/* f_count flags */
	for (i = 1, flags = 0; i < argc; i++)
		if (*argv[i] == '-')
			flags += get_flags(argv[i]);

	/* f_count portfolio */
	if (argc > 1)
	{
		set_tabwidth(tabwidth);
		init_screen();
		f_count = argc-1-flags;
		portfolio = init_folio(f_count);
		nav = init_nav(nav);

		for (i = 1; i < argc; i++)
			if (*argv[i] != '-')
				read_arg(argv[i], portfolio, nav, f_count);

		do {
			//signal(SIGWINCH, refresh_pf);
			if(check_resize())
				refresh_portfolio(portfolio, nav, tabwidth);
			get_input(portfolio, nav, c, tabwidth);
			blit_screen();
		} while ((c = readchar()) != EOF && c != 'q');

		free_folio(portfolio, f_count);
		free_screen();
		free_nav(nav);
	} else
		printf("usage: %s <file1> <file2> ...\n", argv[0]);

	return 0;
}

