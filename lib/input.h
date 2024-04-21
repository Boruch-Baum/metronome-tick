#define MAX_COMMAND_LEN 68 // MAX_PRESET_NAME_LEN + 7 from "preset "

void prepare_for_input(void);
int get_input(void);
void get_command(char*, int, char*);
void print_error(const char *restrict, ...);
