#include <stdio.h>
#include <stdlib.h>

enum move { START, UP, DOWN };

struct Window *init_folio(unsigned int num);
struct Window *scan_files(struct Window *book, char* file_name, int num_of_files);
void free_folio(struct Window *book, size_t num);
void print_folio(struct Window *f);
struct Screen *init_screen(void);
void free_screen(void);
void page_write(struct Window *file, size_t line);
void blit_screen(void);
void get_input(struct Window *files, int c);
int readchar(void);
