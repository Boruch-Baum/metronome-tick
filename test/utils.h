#include "../lib/files.h"

#define MAX_ERROR_LEN (MAX_LINE_LEN + 18 + 24) // error len + 24 from "Invalid configuration: \n"

void write_file(char*, char*);
void copy_file(char*, char*);
int fork_function(char*, void (*)(void));
