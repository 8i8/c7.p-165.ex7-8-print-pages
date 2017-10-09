#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define OFFSET		1		/* Offset cursor and header line */

enum move { START, UP, DOWN, RIGHT, LEFT };
enum question { NO, YES };

/* init */
struct Screen *init_screen(void);
struct Nav *init_nav(struct Nav *nav);
struct Folio *init_folio(const unsigned int num);

/* screen.c */
int check_resize(void);
void blit_screen(void);
void free_screen(void);
void set_tabwidth(short width);
int write_screen(
		struct Folio *file,
		const short tab,
		const short key_pressed,
		const short is_last);
int get_row(void);

/* folio.c */
struct Folio *write_to_heap(
		struct Folio *portfolio,
		struct Nav *nav,
		char* file_name,
		const int num_of_files);
void free_folio(struct Folio *files, const size_t num);

/* input.c */
int get_flags(const char *argv);
void read_arg(char *argv, struct Folio *pf, struct Nav *nav, int input);
void get_input(struct Folio *portfolio, struct Nav *nav, int c, const short tab);
int readchar(void);
void free_nav(struct Nav *nav);
int navigate(struct Folio *file, const short move, const short last);

