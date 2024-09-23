#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <errno.h>
#include <stdint.h>
#include <string.h>
#include "better_write.h"

/*
    Better write
    Used for printing
    Remember to include actual function
*/


/*
    Custom string length function
    Remember to include actual function
*/

/*
    Check if -n is the argument
    add actual function
*/


/*
    Check if argument is a number
    Add actual function
*/

/*
    Convert string to int
    Add actual function
*/

/*To process when head has no arguments*/
int process_input(const char* filename, ssize_t max_num_lines){
    if(max_num_lines > (ssize_t)INT64_MAX){
        fprintf(stderr, "Error: cannot handle memory that large, %s\n", strerror(errno));
        return 1;
    }

    /*User wants negative lines or 0 lines*/
    if(max_num_lines < (ssize_t)1) return 0;

    /*Check where to read from*/
    int fd;
    if(filename == NULL){
        fd = STDIN_FILENO;
    }else{
        fd = open(filename, O_RDONLY);
        if(fd<0){
            fprintf(stderr,"Error opening file %s\n",strerror(errno));
            return 1;
        }
    }

    /*Initialize line buffer*/
    ssize_t read_result;
    size_t buffer_size = 4096;
    char *buffer = calloc(buffer_size, sizeof(char));
    if(buffer==NULL){
        fprintf(stderr,"Memory allocation failed, %s\n", strerror(errno));
        if(fd != STDIN_FILENO) close(fd);
        return 1;
    }

    /*Initialize line read requirements*/
    ssize_t chars_read = 0, lines_read = 0;
    size_t position = 0;
    char current_char;
    char *line_start = buffer;
    
    /*Read from file until number of requested lines*/
    while(lines_read < max_num_lines){

        /*Read current line*/
        read_result = read(fd, &current_char, 1);
        /*Error reading*/
        if(read_result < (ssize_t)0){
            fprintf(stderr,"Error reading file %s\n",strerror(errno));
            free(buffer);
            if(fd != STDIN_FILENO) close(fd);
            return 1;
        }

        /*End of file*/
        if(read_result == (ssize_t)0){
            if(position > 0){
                if(better_write(STDOUT_FILENO, line_start, position) < (ssize_t)0){
                    fprintf(stderr,"Error writing to stdout\n");
                    free(buffer);
                    if(fd != STDIN_FILENO) close(fd);
                    return 1;
                }
            }
            break;
        }

        /*Read char by char*/
        buffer[position++] = current_char;
        chars_read++;

        /*Check if buffer is full and reallocate more read space*/
        if(position == buffer_size){
            buffer_size += buffer_size;
            char * new_buffer = realloc(buffer, buffer_size);
            if(new_buffer == NULL){
                fprintf(stderr,"Memory allocation failed, %s\n",strerror(errno));
                free(buffer);
                if(fd != STDIN_FILENO) close(fd);
                return 1;
            }
            buffer = new_buffer;
        }

        /*Check if end of line*/
        if(current_char == '\n'){
            lines_read++;
            if(better_write(STDOUT_FILENO, line_start, position) < (ssize_t)0){
                fprintf(stderr,"Error writing to stdout\n");
                free(buffer);
                if(fd != STDIN_FILENO) close(fd);
                return 1;
            }
            position = 0;
        }
    }

    if(better_write(STDOUT_FILENO, "\n", 1) < (ssize_t)0){
        fprintf(stderr,"Error writing to stdout\n");
        free(buffer);
        if(fd != STDIN_FILENO) close(fd);
        return 1;
    }

    free(buffer);
    if(fd != STDIN_FILENO) close(fd);
    return 0;
}

/*
    Main logic
    Check arguments and call process_input
*/
int main(int argc, char const **argv){
    ssize_t num_lines = 10;
    const char *filename = NULL;

    /*No arguments*/
    if(argc == 1){
        return process_input(filename, num_lines);
    }

    /*1 argument, treat as filename*/
    if(argc == 2){
        filename = argv[1];
        return process_input(filename, num_lines);
    }

    /*3 args. Check if first one is -n*/
    if(argc == 3){
        if(!my_strcmp(argv[1],"-n")){
            if(is_number(argv[2])){
                num_lines = my_atoi(argv[2]);
                return process_input(filename, num_lines);
            }else{
                fprintf(stderr,"Error: %s is not a number\n",argv[2]);
                return 1;
            }
        }else{
            fprintf(stderr,"Error: %s is not a valid argument\n",argv[1]);
            return 1;
        }
    }

    /*4 args or more args. Check if first one is -n*/
    if(argc > 3){
        if(!my_strcmp(argv[1],"-n")){
            if(is_number(argv[2])){
                num_lines = my_atoi(argv[2]);
                filename = argv[3];
                return process_input(filename, num_lines);
            }else{
                fprintf(stderr,"Error: %s is not a number\n",argv[2]);
                return 1;
            }
        }else{
            filename = argv[1];
            if(!my_strcmp(argv[2],"-n")){
                if(is_number(argv[3])){
                    num_lines = my_atoi(argv[3]);
                    return process_input(filename, num_lines);
                }else{
                    fprintf(stderr,"Error: %s is not a number\n",argv[3]);
                    return 1;
                }
            }else{
                fprintf(stderr,"Error: %s is not a valid argument\n",argv[2]);
                return 1;
            }
        }
    }

    fprintf(stderr,"Error: invalid number of arguments\n");
}