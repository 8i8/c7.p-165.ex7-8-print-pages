a.out: src/main.c src/input.c src/screen.c src/structs.c src/folio.c
	gcc -g -Wall -Wextra -fsanitize=address -fno-omit-frame-pointer src/main.c src/input.c src/screen.c src/structs.c src/folio.c
