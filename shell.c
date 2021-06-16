// PID: 730446034
// I pledge the COMP211 honor code.

// ----------------------------------------------
// These are the only libraries that can be 
// used. Under no circumstances can additional 
// libraries be included.
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <sys/stat.h>
#include <string.h>
#include "shell.h"

const char* valid_builtin_commands[] = {"cd", "exit", NULL};


void parse( char* line, command_t* p_cmd ) {
    // TO-DO: COMPLETE THIS FUNCTION BODY
    int argc_value = 0; // To count how many arguments and command was typed in the line.

    /* Allocating memory initially when the path is NULL */
    if ( p_cmd->path == NULL ) {
        p_cmd->path = malloc( sizeof(char) * 100 );
    }

    /* Allocating memory initially when argv at index 0 is NULL */
    if ( p_cmd->argv[0] == NULL ) {
        p_cmd->argv[0] = malloc( sizeof(char) * 100 );
    }

    /* Using strtok() function to split a string by a delimiter */
    char* separate_line = strtok( line, " \n" );

    /* Looping through until the string reaches to the end of the line */
    while ( separate_line != NULL ) {

        /* Allocating memory initially for argv at a certain index of argc_value to avoid segmentation fault */
        if ( p_cmd->argv[argc_value] == NULL ) {
            p_cmd->argv[argc_value] = malloc( sizeof(char) * 100 );
        }

        strcpy( p_cmd->argv[argc_value], separate_line ); // Copying the current string into argument value at the current argc_value.
        argc_value++; // Incrementing argc_value so strcpy() on line 46 can allocate into the correct array.
        separate_line = strtok( NULL, " \n" ); // Ending the current string value to continue onto the next string.
    }

    /* Since argument value at argv[0] will always be the path, copy the string values at argv[0] to assign correct values in p_cmd->path struct */
    strcpy( p_cmd->path, p_cmd->argv[0] );

    p_cmd->argc = argc_value; // Assigning argc_value to the correct p_cmd struct (p_cmd->argc).
    p_cmd->argv[argc_value] = NULL; // Null terminator for the argument values pointer. Since the last argc_value will always be NULL, it is important to explictly say NULL at that index.

    int result = is_builtin( p_cmd ); // Checking is p_cmd is a built-in command.

    /* Runs through this loop if p_cmd is not in array */
    if ( result == FALSE ) {
        int abs_path = find_fullpath( p_cmd->path, p_cmd ); // Looking for the full path of cmd by using find_fullpath() function.

        /* Runs the loop is full path is not found and setting p_cmd structs to the correct values */
        if ( abs_path == FALSE ) {
            p_cmd->argc = ERROR;
            p_cmd->argv[0] = NULL;
        }
    }

    /* Checks if the entered line is a new line and setting p_cmd structs to the correct values accordingly */
    if ( *line == '\n' ) {
        p_cmd->path = NULL;
        p_cmd->argc = SUCCESSFUL;
        p_cmd->argv[0] = NULL;
    }

    return;

} // end parse function


int find_fullpath( char* command_name, command_t* p_cmd ) {
    // TO-DO: COMPLETE THIS FUNCTION BODY
    int exists = FALSE;

    char* path_env_variable;
    path_env_variable = getenv( "PATH" ); // Retrieving the folders defined in "PATH". Basically the full qualified path of the command_name.

    /* Clone path_env_variable since "PATH" will shorten after it goes through each string using strtok. The purpose is to keep "PATH" the same throughout. */
    char path_clone[200];
    strcpy( path_clone, path_env_variable );

    struct stat buffer;
    char* separate_path = strtok(path_clone, ":"); // Using strtok() function to split path by a ":" delimiter.

    /* Looping through until the string split by delimiter reaches to the end of the path */
    while ( separate_path != NULL ) {
        char fullpath[200]; // Initialized char array of size 200 to store all the information about the current string and to see if directory or file exists.
        strcpy( fullpath, separate_path ); // Copying the current string to fullpath array.
        strcat( fullpath, "/" ); // Concatenating current string with "/" so the typed command_name can be after the path. (ex : instead of /binls -> /bin/ls).
        strcat( fullpath, command_name ); // Concatenating the command_name to the current path.

        exists = stat( fullpath, &buffer );

        /* Checks if directory or file exists. If it does, it sets p_cmd->path to the fully qualified path */
        if ( exists == 0 && (S_IFDIR & buffer.st_mode) ) { 
            strcpy( p_cmd->path, fullpath ); 
            exists = TRUE; // Since directory exists, exists becomes TRUE.
            break; // Breaking out of the while loop to avoid unnecessary loops afterwards.
        } else if ( exists == 0 && (S_IFREG & buffer.st_mode) ) { 
            strcpy( p_cmd->path, fullpath );
            exists = TRUE;
            break; // Breaking out of the while loop to avoid unnecessary loops afterwards.
        } else {
            strcpy( p_cmd->path, command_name ); // If file or directory does not exist, set p_cmd->path to just the command_name itself.
            exists = FALSE;
        }

        separate_path = strtok( NULL, ":" ); // Ending the current string value to continue onto the next string.
    }

    return exists;

} // end find_fullpath function


int execute( command_t* p_cmd ) {
    // TO-DO: COMPLETE THIS FUNCTION BODY
    int status = SUCCESSFUL;
    int child_process_status;
    pid_t child_pid;
    child_pid = fork(); // Forking the child_pid.

    /* Runs through the loop if p_cmd->argv has values in it and is not NULL */
    if ( p_cmd->argv > 0 ) {
        if ( child_pid == 0 ) { // Runs this if statement if fork() occurred without errors.
            execv( p_cmd->path, p_cmd->argv ); // Executes the file with stored path and argument values.
            child_process_status = SUCCESSFUL; // child_process_status becomes SUCCESSFUL since fork() call was successful.
            exit( 1 ); // Exits the loop.
        } else {
            wait( &child_process_status ); // Parent pid waits for the child process status.
        }
    } else if ( p_cmd->argc == ERROR ) { // If command was not found, it runs this if statement and prints out there was an error.
        perror( "Execute terminated with an error condition!\n" );
        child_process_status = ERROR; // Assigns child_process_status to -1 (ERROR) since fork() did not occur.
    } 

    return status;

} // end execute function


int is_builtin( command_t* p_cmd ) {

    int cnt = 0;

    while ( valid_builtin_commands[cnt] != NULL ) {

        if ( equals( p_cmd->path, valid_builtin_commands[cnt] ) ) {

            return TRUE;

        }

        cnt++;

    }

    return FALSE;

} // end is_builtin function


int do_builtin( command_t* p_cmd ) {

    // only builtin command is cd

    if ( DEBUG ) printf("[builtin] (%s,%d)\n", p_cmd->path, p_cmd->argc);

    struct stat buff;
    int status = ERROR;

    if ( p_cmd->argc == 1 ) {

        // -----------------------
        // cd with no arg
        // -----------------------
        // change working directory to that
        // specified in HOME environmental 
        // variable

        status = chdir( getenv("HOME") );

    } else if ( ( stat( p_cmd->argv[1], &buff ) == 0 && ( S_IFDIR & buff.st_mode ) ) ) {


        // -----------------------
        // cd with one arg 
        // -----------------------
        // only perform this operation if the requested
        // folder exists

        status = chdir( p_cmd->argv[1] );

    } 

    return status;

} // end do_builtin function



void cleanup( command_t* p_cmd ) {

    int i=0;

    while ( p_cmd->argv[i] != NULL ) {
        free( p_cmd->argv[i] );
        i++;
    }

    free( p_cmd->argv );
    free( p_cmd->path );	

} // end cleanup function


int equals( char* str1, const char* str2 ) {
    int len[] = {0,0};

    char* b_str1 = str1;
    const char* b_str2 = str2;

    while( (*str1) != '\0' ) { 
        len[0]++;
        str1++;
    }

    while( (*str2) != '\0' ) {
        len[1]++;
        str2++;
    }

    if ( len[0] != len[1] ) {

        return FALSE;

    } else {

        while ( (*b_str1) != '\0' ) {

            if ( tolower( (*b_str1)) != tolower((*b_str2)) ) {

                return FALSE;

            }

            b_str1++;
            b_str2++;

        }

    } 

    return TRUE;


} // end compare function definition
