#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <pwd.h>

/* read_shells - process the shells file and assemble a list of options
 *   each line of the file will be of the format:
 *       <user> <shell name> /path/to/shell --arguments
 *   the first shell listed for the user will be the default
 *   users may be interspersed in any order
 */
int read_shells (char* shells_path, char ***shell_name, char ***shell_cmds) {
    FILE *shells_file = fopen(shells_path, "r");
    if(!shells_file) {
        fprintf(stderr, "error: unable to open shells file\n");
        exit(1);
    }

    char *login_name = getpwuid(geteuid())->pw_name;
    if(!login_name) {
        fprintf(stderr, "error: unable to determine user name\n");
        exit(1);
    }

    unsigned int num_shells = 0;
    shell_name[0] = 0;
    shell_cmds[0] = 0;

    while(!feof(shells_file)) {
        char *line = calloc(4097, sizeof(char));
        char *user = calloc(1024, sizeof(char));
        char *name = calloc(1024, sizeof(char));
        char *cmd  = calloc(2048, sizeof(char));
        fgets(line, 4096, shells_file);
        int elems = sscanf(line, "%s %s %[^\r\n]", user, name, cmd);
        if(elems == 3 && strcmp(user, login_name) == 0 && line[0] != '#') {
            num_shells++;
            shell_name[0] = realloc(shell_name[0], num_shells * sizeof(char*));
            shell_cmds[0] = realloc(shell_cmds[0], num_shells * sizeof(char*));
            shell_name[0][num_shells - 1] = name;
            shell_cmds[0][num_shells - 1] = cmd;
        } else {
            free(name);
            free(cmd);
        }
        free(user);
        free(line);
    }

    fclose(shells_file);
    return num_shells;
}

/* read_choice - read an integer from the user
 *   if it's out of bounds [0, num_shells) we return 0
 */
int read_choice (int num_shells) {
    char *choice_str = calloc(65, sizeof(char));
    fgets(choice_str, 64, stdin);

    if(choice_str[0] == '\n') {
        free(choice_str);
        return 0;
    }

    int choice_num;
    int elems = sscanf(choice_str, "%d", &choice_num);
    free(choice_str);

    if(elems == 1 && 0 <= choice_num && choice_num < num_shells) {
        return choice_num;
    }

    printf("incorrect choice -- defaulting to 0\n");
    return 0;
}

int main (int argc, char** argv) {
    int ii;
    int    num_shells  = 0;
    char **shell_name  = 0;
    char **shell_cmds  = 0;
    num_shells = read_shells("/etc/shsel", &shell_name, &shell_cmds);

    if(num_shells == 0) {
        printf("no shells for you -- go away\n");
        exit(1);
    }

    printf("available shells:\n");
    for(ii = 0; ii < num_shells; ii++) {
        printf("  %2d - %s\n", ii, shell_name[ii]);
    }
    printf("select shell [%d]: ", 0);

    int shell_choice = read_choice(num_shells);

    system(shell_cmds[shell_choice]);

    for(ii = 0; ii < num_shells; ii++) {
        free(shell_name[ii]);
        free(shell_cmds[ii]);
    }
    free(shell_name);
    free(shell_cmds);

    return 0;
}
