more_or_less: src/main.c src/input.c src/screen.c src/structs.c src/folio.c
	gcc -g -Wall -Wextra -fsanitize=address -fno-omit-frame-pointer -o more_or_less src/main.c src/input.c src/screen.c src/structs.c src/folio.c
