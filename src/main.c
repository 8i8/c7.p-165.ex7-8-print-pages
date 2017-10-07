/*
 * Exercise 7-8. Write a program to print a set of files, starting each new one
 * on a new page, with a title and a running page count for each file.
 */
#include "structs.c"

int main(int argc, char *argv[])
{
	struct Window *portfolio;
	struct Nav *nav = NULL;
	int i, c, flags, input;

	getchar();

	/* input flags */
	for (i = 1, flags = 0; i < argc; i++)
		if (*argv[i] == '-') {
			c = *(argv[i]+1), flags++;
			switch (c) {
				case 'n': printf("Hello World.");
					break;
				default:
					printf("usage: %s <file1> <file2> ...\n", argv[0]);
			}
		}

	/* input portfolio */
	if (argc > 1)
	{
		init_screen();
		input = argc-1-flags;
		portfolio = init_folio(input);
		nav = init_nav(nav);

		for (i = 1; i < argc; i++)
			if (*argv[i] != '-')
				if ((portfolio = scan_files(portfolio, nav, argv[i], input)) == NULL)
					printf("usage: %s <file1> <file2> ...\n", argv[0]);
		c = START;

		do {
			get_input(portfolio, nav, c);
			blit_screen();
		} while ((c = readchar()) != EOF && c != 'q');

		free_folio(portfolio, input);
		free_screen();
		free_nav(nav);
	} else
		printf("usage: %s <file1> <file2> ...\n", argv[0]);

	return 0;
}

