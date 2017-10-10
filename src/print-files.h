#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define OFFSET		1		/* Offset cursor and header line */

enum move { START, UP, DOWN, RIGHT, LEFT, STATIC };
enum question { CONT, STOP };

/* init */
struct Screen *init_screen(void);
struct Nav *init_nav(struct Nav *nav);
struct Folio *init_folio(unsigned int num);

/* screen.c */
int check_resize(void);
void blit_screen(void);
void free_screen(void);
void set_tabwidth(short width);
int write_screen(
		struct Folio *file,
		short tab,
		short key_pressed,
		short is_last);
int get_row(void);

/* folio.c */
struct Folio *write_to_heap(
		struct Folio *portfolio,
		struct Nav *nav,
		char* file_name,
		const int num_of_files);
int read_folio(struct Folio *folio);
void refresh_portfolio(struct Folio *pf, struct Nav *nav, short tabwidth);
void free_folio(struct Folio *files, size_t num);

/* input.c */
int get_flags(const char *argv);
void read_arg(char *argv, struct Folio *pf, struct Nav *nav, int input);
void get_input(struct Folio *portfolio, struct Nav *nav, int c, short tab);
int readchar(void);
void free_nav(struct Nav *nav);
int navigate(struct Folio *file, short move, short last);

