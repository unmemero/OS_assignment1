#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <errno.h>
#include <stdint.h>
#include <string.h>
#include <sys/resource.h>

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
int my_strcmp(const char *arg, const char *cmp) {
    while (*arg != '\0' && *cmp != '\0') {
        if (*arg != *cmp) {
            if(*arg < *cmp) return -1;
            return 1;  
        }
        arg++;
        cmp++;
    }
    if (*arg == '\0' && *cmp == '\0') return 0;
    if(*arg == '\0') return -1;
    return 1;
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
    To display error messages
*/
void display_error_message(const char *message){
    ssize_t write_result = better_write(STDERR_FILENO, message, my_strlen(message));
    write_result *=write_result; /*So no warnings*/
}

/*
    To process when head has no arguments
*/
int process_input(const char* filename, ssize_t max_num_lines){
    struct rlimit rl;
    int limit_result;
    if((limit_result = getrlimit(RLIMIT_STACK,&rl) < 0)){
        display_error_message("Unable to fetch amount of memory\n");
        return 1;
    }

    /*User wants negative lines or 0 lines*/
    if(max_num_lines < (ssize_t)1) return 0;

    /*Check where to read from*/
    int fd;
    if(filename == NULL)fd = STDIN_FILENO;
    else{
        if((fd = open(filename, O_RDONLY))<0){
            display_error_message("Unable to open file\n");
            return 1;
        }
    }

    /*Initialize line buffer*/
    size_t buffer_size = 8192;
    char *buffer;
    if((buffer=calloc(buffer_size, sizeof(char)))==NULL){
        display_error_message("Error: Unable to allocate buffer\n");
        if(fd != STDIN_FILENO) close(fd);
        return 1;
    }

    ssize_t read_result;
    size_t total_bytes_read = (ssize_t)0;
    /*Read entire file to avoid syscall overhead by context switches*/
    while((read_result = read(fd, buffer + total_bytes_read, buffer_size - total_bytes_read)) > (ssize_t)0){
        total_bytes_read += read_result;
        if(total_bytes_read == buffer_size){
            buffer_size += buffer_size;
            char *new_buffer = realloc(buffer, buffer_size);
            if(new_buffer == NULL){
                display_error_message("Error allocating memory");
                free(buffer);
                if(fd != STDIN_FILENO) close(fd);
                return 1;
            }
            buffer = new_buffer;
        }
    }

    /* Iterate through buffer until lines printed = max_num_lines or EOF */
size_t position = (size_t)0, current_line = (size_t)0, line_start = (size_t)0;
while (current_line < max_num_lines && position <= total_bytes_read) {
    if (buffer[position] == '\n' || position == total_bytes_read) {
        if ((better_write(STDOUT_FILENO, buffer + line_start, position - line_start + 1)) < 0) {
            display_error_message("Error on writing into STDOUT");
            if (fd != STDIN_FILENO) close(fd);
            free(buffer);
            return 1;
        }
        current_line++;
        line_start = position + 1;
    }
    position++;
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
                display_error_message("Error: not a number\n");
                return 1;
            }
        }else{
            display_error_message("Error: Incorrect arguments\n");
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
                display_error_message("Error: Number required for argument after -n\n");
                return 1;
            }
        }else{
            filename = argv[1];
            if(!my_strcmp(argv[2],"-n")){
                if(is_number(argv[3])){
                    num_lines = my_atoi(argv[3]);
                    return process_input(filename, num_lines);
                }else{
                    display_error_message("Error: Number required for argument after -n\n");
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