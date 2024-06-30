#include "s21_grep.h"

Flags GrepFlags(int argc, char *argv[]) {
  (void)argc, (void)argv;

  Flags flags = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0};
  const char *short_options = "eivclnhsfo";
  int getopt_flags = getopt_long(argc, argv, short_options, NULL, NULL);
  while (getopt_flags != -1) {
    if (getopt_flags == 'e') flags.flag_e = 1;
    if (getopt_flags == 'i') flags.flag_i = 1;
    if (getopt_flags == 'v') flags.flag_v = 1;
    if (getopt_flags == 'c') flags.flag_c = 1;
    if (getopt_flags == 'l') flags.flag_l = 1;
    if (getopt_flags == 'n') flags.flag_n = 1;
    if (getopt_flags == 'h') flags.flag_h = 1;
    if (getopt_flags == 's') flags.flag_s = 1;
    if (getopt_flags == 'f') flags.flag_f = 1;
    if (getopt_flags == 'o') flags.flag_o = 1;
    getopt_flags = getopt_long(argc, argv, short_options, NULL, NULL);
  }
  return flags;
}

int CountFile(int argc, char *argv[]) {
  int count_file = 2;
  int count_flagFE = 0;
  int count_flags = 0;
  for (int i = 0; i < argc; i++) {
    int j = 1;
    while (argv[i][j] != '\0') {
      if (argv[i][0] == '-' && (argv[i][j] == 'e' || argv[i][j] == 'f')) {
        count_flagFE++;
      }
      j++;
    }
  }
  for (int i = 0; i < argc; i++) {
    if (argv[i][0] == '-') {
      count_flags++;
    }
  }
  for (int i = 1; i < argc; i++) {
    if (argv[i][0] == '-') {
      count_file = i + 1;
      continue;
    }
    if (argv[i][0] != '-') break;
  }
  int flag1 = 0;
  if (count_flags != 0) flag1 = -1;
  if (count_flagFE >= 1) flag1 = 0;
  count_file += count_flagFE;
  if ((argc - count_file) <= 0) {
    printf("No file");
    exit(1);
  }
  return (argc - count_file + flag1);
}

void ActivatingFlags(Flags flag, int count_file, char *file_name, int count,
                     char *line, char *pattern, int *flag2) {
  if (flag.flag_n == 1) {
    if (count_file > 1 && (*flag2) == 0) printf("%s:", file_name);
    if ((*flag2) == 0) printf("%d:", count);
  }
  if (flag.flag_o == 1 && flag.flag_v == 0) {
    if (count_file > 1 && flag.flag_n == 0 && (*flag2) == 0)
      printf("%s:", file_name);
    for (int i = 0; i < (int)strlen(line); i++) {
      int count_symbol = 0;
      for (int j = 0; j < (int)strlen(pattern); j++) {
        if (pattern[j] == line[i + j]) {
          count_symbol++;
        } else {
          break;
        }
      }
      if (count_symbol == (int)strlen(pattern)) printf("%s\n", pattern);
    }
  }
  if ((flag.flag_o == 0 || flag.flag_v == 1) && *flag2 == 0) {
    if (count_file > 1 && flag.flag_n == 0) printf("%s:", file_name);
    int flag3 = 0;
    for (int i = 0; i < (int)strlen(line); i++)
      if (line[i] == '\n') flag3++;
    printf("%s", line);
    if (flag3 == 0) printf("\n");
  }
  (*flag2)++;
}

void RefreshPatterns(int *count, int count_patterns, Flags flag,
                     char patterns[300][150], regex_t *preg, char *line,
                     int *flagL, int *count_flaglc, int *count_flagc,
                     int count_file, char *file_name) {
  (*count)++;
  int flag2 = 0;
  regmatch_t match;
  for (int i = 0; i < count_patterns; i++) {
    int cflags = (flag.flag_i == 1) ? REG_ICASE : REG_NOSUB;
    if (regcomp(preg, patterns[i], cflags) != 0) {
      printf("error\n");
      exit(1);
    }
    if (flag.flag_v == 0) {
      if (regexec(preg, line, 1, &match, 0) == 0) {
        if (flag2 == 0) (*count_flagc)++;
        *flagL = 1;
        *count_flaglc = 1;
        if (flag.flag_c == 0 && flag.flag_l == 0) {
          ActivatingFlags(flag, count_file, file_name, *count, line,
                          patterns[i], &flag2);
        }
        if (flag.flag_o == 0) {
          regfree(preg);
          break;
        }
      }
    } else {
      if (regexec(preg, line, 1, &match, 0) != 0) {
        if (flag2 == 0) (*count_flagc)++;
        *flagL = 1;
        *count_flaglc = 1;
        if (flag.flag_c == 0 && flag.flag_l == 0) {
          ActivatingFlags(flag, count_file, file_name, *count, line,
                          patterns[i], &flag2);
        }
        if (flag.flag_o == 0) {
          regfree(preg);
          break;
        }
      }
    }
    regfree(preg);
  }
}

void OpenFile(char *file_name, Flags flag, int count_file,
              char patterns[300][150], int count_patterns) {
  FILE *file;
  if ((file = fopen(file_name, "r")) != NULL) {
    char *line = NULL;
    regex_t preg_storage;
    regex_t *preg = &preg_storage;
    size_t length = 0;

    int count = 0, count_flagc = 0, count_flaglc = 0, flagL = 0;
    if (flag.flag_h == 1) count_file = 0;
    while (getline(&line, &length, file) != -1) {
      RefreshPatterns(&count, count_patterns, flag, patterns, preg, line,
                      &flagL, &count_flaglc, &count_flagc, count_file,
                      file_name);
    }
    if (flag.flag_c == 1 && flag.flag_l == 0) {
      if (count_file > 1) printf("%s:", file_name);
      printf("%d\n", count_flagc);
    }
    if (flag.flag_c == 1 && flag.flag_l == 1) {
      if (count_file > 1) printf("%s:", file_name);
      if (count_flaglc == 1) printf("%d\n", 1);
      if (count_flaglc == 0) printf("%d\n", 0);
    }
    if (flag.flag_l == 1 && flagL == 1) {
      printf("%s\n", file_name);
    }
    if (line) free(line);
    fclose(file);
  } else {
    if (flag.flag_s != 1) printf("File no read");
  }
}

int CountLetters(char *line) {
  int count_letters = 0;
  int i = 0;
  while (line[i] != '\0') {
    i++;
    count_letters++;
  }
  return count_letters;
}

void ReadFlagF(int argc, char *argv[], char patterns[300][150],
               int *count_patterns) {
  int count_file = CountFile(argc, argv);
  int count_flagF = 0;
  int count_openfiles = 0;
  for (int i = 1; i < argc; i++) {
    int j = 1;
    while (argv[i][j] != '\0') {
      if (argv[i][0] == '-' && argv[i][j] == 'f') count_flagF++;
      j++;
    }
  }
  for (int i = 1; i < argc - count_file; i++) {
    if (argv[i][0] == '-') continue;
    if (access(argv[i], F_OK) != -1) {
      count_openfiles++;
      FILE *file = fopen(argv[i], "r");
      if (file != NULL) {
        size_t length = 0;
        char *line = NULL;
        while (getline(&line, &length, file) != -1) {
          int count1 = CountLetters(line);
          for (int j = 0; j < count1 - 1; j++) {
            patterns[*count_patterns][j] = line[j];
          }
          (*count_patterns)++;
        }
        if (line) free(line);
        fclose(file);
      }
    }
  }
  if (count_openfiles != count_flagF) {
    printf("No file");
    exit(1);
  }
}
void ReadFlagE(int argc, char *argv[], char (*patterns)[150],
               int *count_patterns) {
  int count_flags = 0;
  int count_flagE = 0;
  for (int i = 0; i < argc; i++) {
    if (argv[i][0] == '-') count_flags++;
    if (argv[i][0] == '-' && argv[i][1] == 'e') count_flagE++;
  }
  while (count_flagE > 0) {
    for (int j = 0; j < (int)strlen(argv[1 + count_flags]); j++) {
      patterns[*count_patterns][j] = argv[1 + count_flags][j];
    }
    (*count_patterns)++;
    count_flags++;
    count_flagE--;
  }
}

void Grep(int argc, char *argv[], Flags flags) {
  char **pattern = &argv[1];
  char **end = &argv[argc];

  int count_flagFE = 0;
  while (pattern != end && pattern[0][0] == '-') {
    if (pattern[0][1] == 'e' || pattern[0][1] == 'f') count_flagFE++;
    ++pattern;
  }
  pattern += count_flagFE;
  if (pattern == end) {
    printf("no pattern or file\n");
    exit(1);
  }
  int count_patterns = 0;
  char patterns[300][150] = {0};
  if (flags.flag_e == 1) {
    ReadFlagE(argc, argv, patterns, &count_patterns);
  } else if (flags.flag_f == 0) {
    for (int i = 1; i < argc; i++) {
      if (argv[i][0] == '-') continue;
      strcpy(patterns[count_patterns], argv[i]);
      break;
    }
    count_patterns++;
  }

  int count_file = CountFile(argc, argv);
  pattern = &argv[argc - count_file];
  if (flags.flag_f == 1) {
    ReadFlagF(argc, argv, patterns, &count_patterns);
  }
  for (char **file_name = pattern; file_name != end; ++file_name) {
    if (file_name[0][0] == '-') continue;
    if (access(*file_name, F_OK) != -1) {
      OpenFile(*file_name, flags, count_file, patterns, count_patterns);
    } else {
      if (flags.flag_s != 1) printf("no file\n");
      if (count_file <= 1) exit(1);
    }
  }
}

int main(int argc, char *argv[]) {
  (void)argc, (void)argv;
  if (argc >= 3) {
    Flags flags = GrepFlags(argc, argv);
    Grep(argc, argv, flags);
  } else {
    printf("error");
  }
  return 0;
}