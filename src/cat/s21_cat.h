#ifndef S21_CAT
#define S21_CAT

typedef struct {
  int number_non_blank;
  int symbol_end;
  int number;
  int squeeze_blank;
  int tab_as;
  int flag_v;
} Flags;

int CheckFlag(int argc, char *argv[]);
Flags CatFlags(int argc, char *argv[]);
int EmptyFile(int argc, char *argv[]);
void ActivatingFlags(char *file_name, Flags flag);

#endif