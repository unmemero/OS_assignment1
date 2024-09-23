#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <errno.h>
#include "better_write.h"

/*Add functions and comments from better_write.c*/

/*Garbage collector*/
void cleanup_and_return(char *current_line, char **lines, size_t *line_lengths, int fd){
    if(current_line != NULL) free(current_line);
    if(lines != NULL){
        for(size_t i = 0; i<sizeof(lines);i++) if(lines[i] != NULL) free(lines[i]);
        free(lines);
        free(line_lengths);
    }
    if(fd != STDIN_FILENO) close(fd);
}

/*Processing file*/
int process_file(const char* filename, ssize_t max_num_lines){
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

    /*Initialize arrays and pointers for current line and all lines*/
    int return_code = 0;
    char buffer[4096], *current_line = NULL, current_char, **lines = NULL;
    void *pointer;
    ssize_t read_result;
    size_t chars_read, current_line_size = (size_t)0, current_line_allocation = (size_t)0, new_size;
    size_t num_lines = (size_t)0, lines_allocated = (size_t)0, *line_lengths = NULL;

    /*Loop until the number of lines reaches the amount desired by the user*/
    while(num_lines<max_num_lines){

        /*
            Read from the file and handle errors
        */
        if((read_result = read(fd, buffer, sizeof(buffer))<0)){
            fprintf(stderr,"Error reading from file %s\n",strerror(errno));
            cleanup_and_return(current_line, lines, line_lengths, fd);
            return 1;
        }

        /*End of file*/
        if(read_result == (size_t)0) break;

        /*Loop through the buffer*/
        for(size_t i = (size_t)0 ; i<(size_t)read_result; i++){
            current_char = buffer[i];

            /*Empty line case*/
            if(current_line == NULL){
                current_line_allocation = (size_t)1;

                if((pointer = calloc(current_line_allocation, sizeof(char))) == NULL){
                    fprintf(stderr,"Error allocating memory, %s\n", strerror(errno));
                    cleanup_and_return(current_line, lines, line_lengths, fd);
                    return 1;
                }
                current_line = (char*)pointer;
            }else{
                /*
                    Subsequent characters
                    First case is we need more memory
                */
                if(current_line_size >= current_line_allocation){
                    new_size = current_line_allocation + current_line_allocation;
                    if(new_size < current_line_allocation){
                        fprintf(stderr,"Error: too much memory required, %s\n", strerror(errno));
                        cleanup_and_return(current_line, lines, line_lengths, fd);
                        return 1;
                    }
                    if((pointer = realloc(current_line, new_size*sizeof(char))) == NULL){
                        fprintf(stderr,"Error reallocating memory, %s\n", strerror(errno));
                        cleanup_and_return(current_line, lines, line_lengths, fd);
                        return 1;
                    }
                    current_line = (char*)pointer;
                    current_line_allocation = new_size;
                }
            }

            /*There's at least 1 byte left to include in*/
            current_line[current_line_size] = current_char;
            current_line_size++;

            /*Handle newline character*/
            if(current_char == '\n'){
                if(lines == NULL){
                    lines_allocated = (size_t)1;
                    if((pointer = calloc(lines_allocated, sizeof(char*))) == NULL){
                        fprintf(stderr,"Error allocating memory, %s\n", strerror(errno));
                        cleanup_and_return(current_line, lines, line_lengths, fd);
                        return 1;
                    }
                    lines = (char**)pointer;
                    if((pointer = calloc(lines_allocated, sizeof(size_t)))==NULL){
                        fprintf(stderr,"Error allocating memory, %s\n", strerror(errno));
                        cleanup_and_return(current_line, lines, line_lengths, fd);
                        return 1;
                    }
                    line_lengths = (size_t*)pointer;
                }else{
                    if(num_lines >= lines_allocated){
                        if((new_size = lines_allocated + lines_allocated)<lines_allocated){
                            fprintf(stderr,"Error: too much memory required, %s\n", strerror(errno));
                            cleanup_and_return(current_line, lines, line_lengths, fd);
                            return 1;
                        }
                        if((pointer = realloc(lines, new_size*sizeof(char*))) == NULL){
                            fprintf(stderr,"Error reallocating memory, %s\n", strerror(errno));
                            cleanup_and_return(current_line, lines, line_lengths, fd);
                            return 1;
                        }
                        lines = (char**)pointer;
                        if((pointer = realloc(line_lengths, new_size*sizeof(size_t))) == NULL){
                            fprintf(stderr,"Error reallocating memory, %s\n", strerror(errno));
                            cleanup_and_return(current_line, lines, line_lengths, fd);
                            return 1;
                        }
                        line_lengths = (size_t*)pointer;
                        lines_allocated = new_size;
                    }
                }
                lines[num_lines] = current_line;
                line_lengths[num_lines] = current_line_size;
                num_lines++;
                current_line = NULL;
                current_line_size = (size_t)0;
                current_line_allocation = (size_t)0;
            }
        }
    }
    
    /*Line with no \n, EOF case*/
    if(current_line != NULL) {
        if(lines == NULL){
            lines_allocated = (size_t)1;
            if((pointer = calloc(lines_allocated, sizeof(char*))) == NULL){
                fprintf(stderr,"Error allocating memory, %s\n", strerror(errno));
                cleanup_and_return(current_line, lines, line_lengths, fd);
                return 1;
            }
            lines = (char**)pointer;
            if((pointer = calloc(lines_allocated, sizeof(size_t))) == NULL){
                fprintf(stderr,"Error allocating memory, %s\n", strerror(errno));
                cleanup_and_return(current_line, lines, line_lengths, fd);
                return 1;
            }
            line_lengths = (size_t*)pointer;
        }else{
            
        }
    }
    return 0;
}

int main(int argc, char const *argv[]){
    
    return 0;
}
