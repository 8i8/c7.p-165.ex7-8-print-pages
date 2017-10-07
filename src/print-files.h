#include <stdio.h>
#include <stdlib.h>

#define OFFSET		1		/* Offset cursor and header line */

enum move { START, UP, DOWN, RIGHT, LEFT };

struct Nav {
	size_t f_count;			/* File index */
	size_t f_active;		/* File curently being read */
};

struct Window *init_folio(unsigned int num);
struct Window *scan_files(struct Window *portfolio, struct Nav *nav, char* file_name, int num_of_files);
void free_folio(struct Window *files, size_t num);
void print_folio(struct Window *f);
struct Screen *init_screen(void);
struct Nav *init_nav(struct Nav *nav);
int navigate(struct Window *file, short move);
void free_screen(void);
int page_content(struct Window *file, short key_press);
void blit_screen(void);
void get_input(struct Window *portfolio, struct Nav *nav, int c);
int readchar(void);
int get_row(void);
void free_nav(struct Nav *nav);
