#include "utils.h"

char* search_command(const char *command){
    char* path = getenv("PATH");
    /*  From man getenv:
     * The getenv() function obtains the current value of the environment variable,
     name.  The application should not modify the string pointed to by the getenv()
     function.
    */
    char* path_copy = strdup(path);// Duplicate the string pointed to by path
    //
    char* path_token = strtok(path_copy, ":");
    char* command_path = calloc(1, MAX_PATH_LENGTH); // Allocate memory for the command path
    while (path_token != NULL){
        path_token = strtok(NULL, ":");
        if (path_token == NULL){
            break;
        }
        snprintf(command_path, MAX_PATH_LENGTH, "%s/%s", path_token, command);

        /*
        *   The access() system call checks the accessibility of the file named by
         the path argument for the access permissions indicated by the mode
         argument.  The value of mode is either the bitwise-inclusive OR of the
         access permissions to be checked (R_OK for read permission, W_OK for
         write permission, and X_OK for execute/search permission), or the
         existence test (F_OK).
        */
        if (access(command_path, X_OK) == 0){ // Check if the file exists and is executable
            return command_path;
        }
    }
    return NULL;
}
