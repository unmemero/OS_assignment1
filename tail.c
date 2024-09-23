#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
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


/*Processing file*/
int process_input(const char* filename, ssize_t max_num_lines){
    /*Check where to read from*/
    int fd;
    if(filename == NULL){
        fd = STDIN_FILENO;
    }else{
        fd = open(filename, O_RDONLY);
        if(fd<0){
            better_write(STDERR_FILENO, "Error opening file \n", sizeof("Error opening file \n"));
            return 1;
        }
    }

    /*Initialize calllocated buffer*/
    size_t buffer_size = 8192;
    char *buffer = calloc(buffer_size, sizeof(char));
    if(buffer == NULL){
        better_write(STDERR_FILENO, "Error allocating buffer\n", sizeof("Error allocating buffer\n"));
        if(fd != STDIN_FILENO) close(fd);
        return 1;
    }

    /*Read until EOF*/
    ssize_t total_read =(ssize_t)0;
    ssize_t read_bytes;
    while((read_bytes = read(fd, buffer + total_read, buffer_size - total_read)) > 0){
        total_read += read_bytes;

        /*Full buffer*/
        if(total_read == buffer_size){
            buffer_size += buffer_size;
            char *new_buffer = realloc(buffer, buffer_size);
            if(new_buffer == NULL){
                better_write(STDERR_FILENO,"Error allocating memory\n",sizeof("Error allocating memory\n"));
                free(buffer);
                if(fd != STDIN_FILENO) close(fd);
                return 1;
            }
            buffer = new_buffer;
        }
    }

    if(read_bytes < 0){
        better_write(STDERR_FILENO,"Error reading file\n",sizeof("Error reading file\n"));
        free(buffer);
        if(fd != STDIN_FILENO) close(fd);
        return 1;
    }

    /*Process last lines of file*/
    ssize_t num_lines = 0;
    for(ssize_t i = total_read - 1;i>-1;i--){
        if(buffer[i] == '\n'){
            num_lines++;
            if(num_lines == max_num_lines){
                better_write(STDOUT_FILENO,buffer + i + 1, total_read - i - 1);
                free(buffer);
                if(fd != STDIN_FILENO) close(fd);
                return 0;
            }
        }
    }

    better_write(STDOUT_FILENO,buffer,total_read);
    free(buffer);
    if(fd != STDIN_FILENO) close(fd);
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
                better_write(STDERR_FILENO,"Error: not a number\n",sizeof("Error: not a number\n"));
                return 1;
            }
        }else{
            better_write(STDERR_FILENO,"Error: Illegal use of -n\n",sizeof(sizeof("Error: Illegal use of -n\n")));
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
                better_write(STDERR_FILENO,"Error: Number required for argument after -n\n",sizeof("Error: Number required for argument after -n\n"));
                return 1;
            }
        }else{
            filename = argv[1];
            if(!my_strcmp(argv[2],"-n")){
                if(is_number(argv[3])){
                    num_lines = my_atoi(argv[3]);
                    return process_input(filename, num_lines);
                }else{
                    better_write(STDERR_FILENO,"Error: Number required for argument after -n\n",sizeof("Error: Number required for argument after -n\n"));
                    return 1;
                }
            }else{
                better_write(STDERR_FILENO,"Error: Illegal use of -n\n",sizeof("Error: Illegal use of -n\n"));
                return 1;
            }
        }
    }

    better_write(STDERR_FILENO,"Too many arguments\n",sizeof("Too many arguments\n"));
    return 1;
}