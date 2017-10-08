#include <stdio.h>
#include <stdlib.h>

#define OFFSET		1		/* Offset cursor and header line */

enum move { START, UP, DOWN, RIGHT, LEFT };
enum question { NO, YES };

/* init */
struct Screen *init_screen(void);
struct Nav *init_nav(struct Nav *nav);
struct Folio *init_folio(const unsigned int num);

/* screen.c */
void blit_screen(void);
void free_screen(void);
int write_screen(struct Folio *file, const short key_press, const short last);
int get_row(void);

/* folio.c */
struct Folio *scan_files(
		struct Folio *portfolio,
		struct Nav *nav,
		char* file_name,
		const int num_of_files);
void open_last_page(struct Folio *file);
void free_folio(struct Folio *files, const size_t num);

/* input.c */
void get_input(struct Folio *portfolio, struct Nav *nav, int c);
int readchar(void);
void free_nav(struct Nav *nav);
int navigate(struct Folio *file, const short move, const short last);

