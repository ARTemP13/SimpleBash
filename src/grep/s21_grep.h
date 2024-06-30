#ifndef S21_GREP
#define S21_GREP
#define _GNU_SOURCE
#include <getopt.h>
#include <regex.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

typedef struct {
  int flag_e;
  int flag_i;
  int flag_v;
  int flag_c;
  int flag_l;
  int flag_n;
  int flag_h;
  int flag_s;
  int flag_f;
  int flag_o;
} Flags;

Flags GrepFlags(int argc, char *argv[]);
int CountFile(int argc, char *argv[]);
void ActivatingFlags(Flags flag, int count_file, char *file_name, int count,
                     char *line, char *pattern, int *flag2);
void RefreshPatterns(int *count, int count_patterns, Flags flag,
                     char patterns[300][150], regex_t *preg, char *line,
                     int *flagL, int *count_flaglc, int *count_flagc,
                     int count_file, char *file_name);
void OpenFile(char *file_name, Flags flag, int count_file,
              char patterns[300][150], int count_patterns);
int CountLetters(char *line);
void ReadFlagF(int argc, char *argv[], char patterns[300][150],
               int *count_patterns);
void ReadFlagE(int argc, char *argv[], char (*patterns)[150],
               int *count_patterns);
void Grep(int argc, char *argv[], Flags flags);

#endif