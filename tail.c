#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <unistd.h>
#include <string.h>
#include <errno.h>

/*
    Better write function
    To display all bytes requested in STDOUT
*/
ssize_t better_write(int fd, const char *buf, size_t count) {
  size_t already_written, to_be_written, written_this_time;
  ssize_t res_write;

  if (count == ((size_t) 0)) return (ssize_t) count;
  
  already_written = (size_t) 0;
  to_be_written = count;
  while (to_be_written > ((size_t) 0)) {
    res_write = write(fd, &(((const char *) buf)[already_written]), to_be_written);
    if (res_write < ((size_t) 0)) {
      /* Error */
      return res_write;
    }
    if (res_write == ((ssize_t) 0)) {
      /* Nothing written, stop trying */
      return (ssize_t) already_written;
    }
    written_this_time = (size_t) res_write;
    already_written += written_this_time;
    to_be_written -= written_this_time;
  }
  return (ssize_t) already_written;
}

/*
    To check if a string is a number
*/
int is_number(const char *arg) {
    int i = 0;
    if (arg[0] == '\0') return 0;
    if (arg[0] == '-') {
        i = 1;
        if (arg[1] == '\0') return 0;
    }
    while (arg[i] != '\0') {
        if (arg[i] < '0' || arg[i] > '9') return 0;
        i++;
    }
    return 1;
}

/*
    To determine size of string
*/
int my_strlen(const char *arg){
    int i = 0;
    while(arg[i] != '\0') i++;
    return i;
}

/*
    To compare two strings
*/
int my_strcmp(const char *arg, const char *cmp){
    while(*arg != '\0' && *cmp != '\0'){
        if(*arg != *cmp) return 1;
        arg++;
        cmp++;
    }
    if(*arg == '\0' && *cmp == '\0') return 0;
    return 1;
}

/*
    my_strcpy
*/
void my_strcpy(char *dest, const char *src){
    while(*src != '\0'){
        *dest = *src;
        dest++;
        src++;
    }
    *dest = '\0';
}

/*
    To convert string to integer
*/
int my_atoi(const char *arg){
    int result = 0;
    if (arg[0] == '-') result = -1;
    for(int i = 0; arg[i] != '\0'; i++){
        result = result * 10 + arg[i] - '0';
    }
    return result;
}

/*
    To display error to user
*/
void display_error_message(const char *message){
    ssize_t write_result = better_write(STDERR_FILENO, message, my_strlen(message));
    write_result *=write_result; /*So no warnings*/
}

/*Garbage collector*/
int gc(char **last_lines, ssize_t max_num_lines, int fd){
    if(fd != STDIN_FILENO) close(fd);
    for(int i = 0; i < max_num_lines; i++) if(last_lines[i] != NULL)free(last_lines[i]);
    free(last_lines);
    return 1;
}

/*
    Processing file
*/
int process_input(const char* filename, ssize_t max_num_lines){
    /*Number of lines too many*/
    if(max_num_lines > (ssize_t)INT64_MAX){
            display_error_message("Error: Number of lines is too large\n");
        return 1;
    }
    /*0 or negative lines requested by user*/
    if(max_num_lines < 1) return 0;

    /*Get fd based on filename*/
    int fd;
    if(filename == NULL) fd = STDIN_FILENO;
    else{
        if((fd = open(filename, O_RDONLY)) < 0){
            display_error_message("Unable to open file\n");
            return 1;
        }
    }
    
    /*Create buffer to store current line*/
    char *buffer;
    size_t buffer_size = 8192;
    if((buffer = calloc(buffer_size,sizeof(char))) == NULL){
        display_error_message("Unable to allocate memory for buffer\n");
        if(fd != STDIN_FILENO) close(fd);
        return 1;
    }

    /*Create array to store max num lines and one for lengths*/
    char **lines;
    size_t lines_lengths[max_num_lines];
    if((lines = calloc(max_num_lines,sizeof(char *))) == NULL){
        display_error_message("Unable to allocate memory for lines\n");
        free(buffer);
        if(fd != STDIN_FILENO) close(fd);
        return 1;
    }

    /*Create way to read fileuntil EOF*/
    ssize_t read_result;
    size_t total_chars_read = (size_t)0;
    while((read_result = read(fd, buffer + total_chars_read, buffer_size - total_chars_read)) > 0){
        total_chars_read += read_result;
        if(total_chars_read >= read_result){
            buffer_size += buffer_size;
            char *newbuffer;
            if((newbuffer = realloc(buffer, buffer_size)) == NULL){ 
                display_error_message("Error allocating memory\n");
                return gc(lines, max_num_lines, fd);
            }
            buffer = newbuffer;
        }
    }

    /*There was an error reading*/
    if(read_result < (ssize_t)0){ 
        display_error_message("Unable to read file\n");
        return gc(lines, max_num_lines, fd);
    }

    /* Save last max_num_lines by iterating backwards */
    size_t position = total_chars_read - 1;
    size_t line_end = total_chars_read;
    ssize_t current_line = max_num_lines - 1;

    while (current_line >= 0) {
        /* Handle the start of the read source (beginning of the buffer) */

        if (position == 0) {
            lines_lengths[current_line] = line_end - position + 1;
            if (lines_lengths[current_line] > 0) {
                lines[current_line] = calloc(lines_lengths[current_line] + 2, sizeof(char));
                if (lines[current_line] == NULL) {
                    display_error_message("Error allocating memory\n");
                    return gc(lines, max_num_lines, fd);
                }

                /* Manually copy the remaining characters to the last line */
                for (size_t i = 0; i < lines_lengths[current_line]; i++) {
                    lines[current_line][i] = buffer[position + i];
                }
                lines[current_line][lines_lengths[current_line] - 1] = '\n';
                lines[current_line][lines_lengths[current_line]] = '\0';
            }
            break;
        }

        /* Handle newline */
        if (buffer[position] == '\n') {
            /* Handle last char being newline */
            if (position == total_chars_read - 1) {  // Should compare to total_chars_read - 1
                lines_lengths[current_line] = 1;
                lines[current_line] = calloc(2, sizeof(char));
                if (lines[current_line] == NULL) {
                    display_error_message("Error allocating memory");
                    return gc(lines, max_num_lines, fd);
                }
                lines[current_line][0] = '\n';
                lines[current_line][1] = '\0'; 
            } else {
                size_t line_start = position + 1;
                lines_lengths[current_line] = line_end - line_start + 1;
                
                /* Ensure lines_lengths is not zero before allocating memory */
                if (lines_lengths[current_line] > 0) {
                    lines[current_line] = calloc(lines_lengths[current_line] + 2, sizeof(char));
                    if (lines[current_line] == NULL) {
                        display_error_message("Error allocating memory");
                        return gc(lines, max_num_lines, fd);
                    }

                    for (size_t i = 0; i < lines_lengths[current_line]; i++) {
                        lines[current_line][i] = buffer[line_start + i];
                    }

                    if(line_end != total_chars_read)  lines[current_line][lines_lengths[current_line] - 1] = '\n';
                    lines[current_line][lines_lengths[current_line]] = '\0';
                }
            }

            current_line--;
            line_end = position;
        }

        position--;

        /* Prevent going out of bounds */
        if (position < 0) break;
    }

    /*Remove null terminator from printing*/
    if(lines[max_num_lines-1][lines_lengths[max_num_lines-1]] == '\0')lines_lengths[max_num_lines - 1]--;

    /*We don't need the buffer anymore, so we discard it*/
    free(buffer);

    /*print last lines using better_write*/
    
    for(size_t i = (size_t)0; i < max_num_lines; i++){
        if((better_write(STDOUT_FILENO,lines[i],lines_lengths[i])) < (ssize_t)0){
            display_error_message("Error writing into STDOUT\n");
            return gc(lines,max_num_lines,fd);
        }
    }

    /*Garbage collection and return*/
    gc(lines, max_num_lines,fd);    
    return 0;
}

int main(int argc, char const *argv[]){
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
                display_error_message("Error: Not a number\n");
                return 1;
            }
        }else{
            display_error_message("Error: Illegal use of -n\n");
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
                display_error_message("Error: Expected number after -n\n");
                return 1;
            }
        }else{
            filename = argv[1];
            if(!my_strcmp(argv[2],"-n")){
                if(is_number(argv[3])){
                    num_lines = my_atoi(argv[3]);
                    return process_input(filename, num_lines);
                }else{
                    display_error_message("Error: Expected number after-n\n");
                    return 1;
                }
            }else{
                display_error_message("Error: Illegal use of -n\n");
                return 1;
            }
        }
    }

    display_error_message("Error: Too many arguments\n");
    return 1;
}