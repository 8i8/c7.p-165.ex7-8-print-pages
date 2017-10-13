/*
 * K&R edition 2
 *
 * Exercise 7-8. Write a program to print a set of files, starting each new one
 * on a new page, with a title and a running page count for each file.
 *
 * 	more_or_less
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

int main(int argc, char *argv[])
{
	struct Folio *portfolio;
	struct Nav *nav = NULL;
	int i, c, flags, f_count, tabwidth;
	tabwidth = TABWIDTH;
	c = START;

	/* read flags */
	for (i = 1, flags = 0; i < argc; i++)
		if (*argv[i] == '-')
			flags += get_flags(argv[i]);

	/* read files */
	if (argc > 1)
	{
		set_tabwidth(tabwidth);
		f_count = argc-1-flags;

		init_screen();
		init_listen();
		portfolio = init_folio(f_count);
		nav = init_nav(nav);

		for (i = 1; i < argc; i++)
			if (*argv[i] != '-')
				read_arg(argv[i], portfolio, nav, f_count);
		do
		{
			get_input(portfolio, nav, c, tabwidth);
			blit_screen();
		}
		while ((c = readchar()) != EOF && c != 'q');

		free_folio(portfolio, f_count);
		free_screen();
		free_nav(nav);
	} else
		error(0, 0, " usage ~ %s <file1> <file2> ...\n", argv[0]);

	return 0;
}

