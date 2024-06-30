#include "s21_cat.h"

#include <getopt.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

int CheckFlag(int argc, char *argv[]) {
  int count = 0;
  for (int i = 1; i < argc; i++) {
    if ((argv[i])[0] == '-') {
      count++;
    } else {
      i = argc + 1;
    }
  }
  return count;
}

Flags CatFlags(int argc, char *argv[]) {
  (void)argc, (void)argv;
  struct option long_option[] = {
      {"number-nonblank", 0, NULL, 'b'},
      {"number", 0, NULL, 'n'},
      {"squeeze-blank", 0, NULL, 's'},
      {NULL, 0, NULL, 0},
  };
  Flags flags = {0, 0, 0, 0, 0, 0};
  int count = CheckFlag(argc, argv);
  const char *short_options = "bevEnstT";
  int getopt_flags = getopt_long(argc, argv, short_options, long_option, NULL);
  if (count != 0) {
    while (getopt_flags != -1) {
      if (getopt_flags == 'b') flags.number_non_blank = 1;
      if (getopt_flags == 'e') {
        flags.symbol_end = 1;
        flags.flag_v = 1;
      }
      if (getopt_flags == 'v') flags.flag_v = 1;
      if (getopt_flags == 'E') flags.symbol_end = 1;
      if (getopt_flags == 'n') flags.number = 1;
      if (getopt_flags == 's') flags.squeeze_blank = 1;
      if (getopt_flags == 't') {
        flags.tab_as = 1;
        flags.flag_v = 1;
      }
      if (getopt_flags == 'T') flags.tab_as = 1;
      getopt_flags = getopt_long(argc, argv, short_options, long_option, NULL);
    }
  }

  return flags;
}

int EmptyFile(int argc, char *argv[]) {
  int flag2 = 1;
  for (int i = 1; i < argc; i++) {
    if ((argv[i])[0] == '-') {
      flag2++;
    } else {
      i = 100;
    }
  }
  return flag2;
}

void ActivatingFlags(char *file_name, Flags flag) {
  FILE *file;
  if ((file = fopen(file_name, "r")) != NULL) {
    char letter = 'k', prev_letter = '\n', prev2_letter = 'k';
    int count = 1;
    while ((letter = fgetc(file)) != EOF) {
      if (flag.squeeze_blank == 1) {
        if (letter == '\n' && prev_letter == '\n' && prev2_letter == '\n')
          continue;
      }
      if (flag.number == 1 && flag.number_non_blank != 1) {
        if (prev_letter == '\n' || count == 1) {
          printf("%6d\t", count);
          count++;
        }
      }
      if (flag.number_non_blank == 1) {
        if ((prev_letter == '\n' && letter != '\n') ||
            (count == 1 && letter != '\n')) {
          printf("%6d\t", count);
          count++;
        }
      }
      if (flag.tab_as == 1) {
        if (letter == 9) {
          printf("^I");
          prev2_letter = prev_letter;
          prev_letter = letter;
          continue;
        }
      }
      if (flag.symbol_end == 1) {
        if (letter == '\n') printf("$");
      }
      if (flag.flag_v == 1) {
        if (letter >= 0 && letter < 32 && letter != '\n' && letter != '\t') {
          printf("^%c", letter + 64);
        } else if (letter == 127) {
          printf("^?");
        } else {
          printf("%c", letter);
        }
      } else {
        printf("%c", letter);
      }
      prev2_letter = prev_letter;
      prev_letter = letter;
    }
    fclose(file);
  }
}
int main(int argc, char *argv[]) {
  int count = 0;
  Flags flags = CatFlags(argc, argv);
  count = EmptyFile(argc, argv);
  for (int i = count; i < argc; i++) {
    if (access(argv[i], F_OK) != -1) {
      ActivatingFlags(argv[i], flags);
    }
  }
  return 0;
}