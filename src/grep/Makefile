all: s21_grep

s21_grep : s21_grep.c s21_grep.h
	gcc -Wall -Werror -Wextra s21_grep.c -o s21_grep
clean :
	rm -rf *.o s21_grep
rebuild :
	make clean
	make