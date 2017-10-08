#include <stdio.h>
#include <stdlib.h>

#define OFFSET		1		/* Offset cursor and header line */

enum move { START, UP, DOWN, RIGHT, LEFT };
enum question { NO, YES };

/* init */
struct Screen *init_screen(void);
struct Nav *init_nav(struct Nav *nav);
struct Window *init_folio(const unsigned int num);

/* screen.c */
void blit_screen(void);
void free_screen(void);
int page_content(struct Window *file, const short key_press, const short last);
void open_last_page(struct Window *file);
int get_row(void);

/* folio.c */
struct Window *scan_files(
		struct Window *portfolio,
		struct Nav *nav,
		char* file_name,
		const int num_of_files);
void free_folio(struct Window *files, const size_t num);

/* input.c */
void get_input(struct Window *portfolio, struct Nav *nav, int c);
int readchar(void);
void free_nav(struct Nav *nav);
int navigate(struct Window *file, const short move, const short last);

