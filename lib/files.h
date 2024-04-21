#include <stdio.h>

#define MAX_LINE_LEN 64

void get_xdg_path(char*, char*, char*, char*);
FILE *read_xdg_file(char*, char*, char*);
void append_xdg_file(char*, char*, char*, char*);
int mktemp_in_tmpdir(char*);
void rename_file(char*, char*);
