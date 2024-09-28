#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <string.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/mman.h>

/*
  Write to STDOUT_FILENO. Improved version of write
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
  Check if the argument is a number
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
  Calculate string length
*/
int my_strlen(const char *arg){
    int i = 0;
    while(arg[i] != '\0') i++;
    return i;
}

/*
  String comparison
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
  String to int converter
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
  For string concats
*/
char *my_strcat(const char* msg1, const char* msg2){
  char *result;
  int i = 0;
  while(msg1[i] != '\0') i++;
  int j = 0;
  while(msg2[j] != '\0') j++;
  result = calloc(i+j+1, sizeof(char));
  for(int k = 0; k < i; k++) result[k] = msg1[k];
  for(int k = 0; k < j; k++) result[i+k] = msg2[k];
  result[i+j] = '\0';
  return result;
}

/*
  Add null terminator
*/
char *add_null_terminator(const char *arg, size_t length){
    char *result = calloc(length + 1, sizeof(char));
    for(size_t i = 0; i < length; i++) result[i] = arg[i];
    result[length] = '\0';
    return result;
}

/*
  Display error message
*/
void display_error_message(const char *message){
    ssize_t write_result = better_write(STDERR_FILENO, message, my_strlen(message));
    write_result *=write_result; /*So no warnings*/
}

/*
    Check if the number is valid.If it is, return null pointer, else, return max the first six digits of the number
*/
char *valid_number(const char* arg){
    /*Find if it's within length and if each char is a number*/
    int arg_length = my_strlen(arg);
    if(arg_length > 10 || arg_length < 1) return NULL;
    for(int i = 0; i<arg_length; i++) if(arg[i] < '0' || arg[i] > '9') return NULL;
    
    /*Obtain lookup code*/
    if(arg_length > 6) arg_length = 6;
    arg_length++;
    char *lookup_code;
    if((lookup_code = calloc(arg_length,sizeof(char))) == NULL){
        display_error_message("Error allocating memory\n");
        return NULL;
    }
    for(int i = 0; i < arg_length; i++) lookup_code[i] = arg[i];
    lookup_code[arg_length-1] = '\0';
    return lookup_code;
}

/*
    In case lseek fails
*/
int read_and_find(int fd, const char *phone){

    /*Allocate buffer to read file*/
    char *buffer;
    size_t buffer_size = 8192;

    if((buffer = calloc(buffer_size, sizeof(char))) == NULL){
        display_error_message("Error allocating memory\n");
        if(fd != STDIN_FILENO) close(fd);
        return 1;
    }

    /*Read file to not do constant syscalls*/
    ssize_t read_result;
    size_t total_chars_read = (size_t)0;
    while((read_result = read(fd,buffer + total_chars_read,buffer_size - total_chars_read)) > 0){
        total_chars_read += (size_t) read_result;
        if(total_chars_read == buffer_size){
            buffer_size += buffer_size;
            char *new_buffer;
            if((new_buffer = realloc(buffer, buffer_size)) == NULL){
                display_error_message("Error allocating memory\n");
                free(buffer);
                if(fd != STDIN_FILENO) close(fd);
                return 1;
            }
            buffer = new_buffer;
        }
    }

    /*Go through file, try finding number*/
    size_t current_phone_char = (size_t)0, phone_length = (size_t)my_strlen(phone);
    ssize_t index_of_found = (size_t)-1;

    for (size_t i = (size_t)0; i < total_chars_read; i++) {
        if (buffer[i] == phone[current_phone_char]) {
            index_of_found = i;
            size_t chars_equal = 1;

            while (chars_equal < phone_length && (i + chars_equal) < total_chars_read) {
                if (buffer[i + chars_equal] == phone[chars_equal]) {
                    chars_equal++;
                } else {
                    break;
                }
            }

            if (chars_equal == phone_length) {
                break;
            } else {
                index_of_found = -1;
            }
        }
    }


    /*Not found*/
    if(index_of_found < 0){
        display_error_message("Unable to locate number\n");
        free(buffer);
        if(fd != STDIN_FILENO) close(fd);
        return 1;
    }
    /*Print out*/
    else{
        char *message = my_strcat("\n\0",add_null_terminator(buffer + index_of_found + 6, 25));
        message = my_strcat(message,"\n\0");
        if((better_write(STDOUT_FILENO, message,25) < 0)){
            display_error_message("Unable to write location\n");
            free(buffer);
            if(fd != STDIN_FILENO) close(fd);
            return 1;
        }
    }

    free(buffer);
    if(fd != STDIN_FILENO) close(fd);
    return 0;
}

/*
    Implement binary search on mapped file
*/
ssize_t binary_search(const char *map, size_t fileSize, const char *target) {
    /*Get lines and target*/
    size_t start = 0;
    size_t end = fileSize / 32 - 1;
    size_t target_length = my_strlen(target);

    /*buffer for the current position in mapping*/
    char *current_map = calloc(target_length + 1, sizeof(char));
    if (!current_map) {
        display_error_message("Memory allocation failed\n");
        return -1;
    }

    /*Search based on offsets*/
    while (start <= end) {
        size_t mid = start + (end - start) / 2;
        size_t mid_offset = mid * 32;

        for (size_t i = 0; i < target_length; i++) {
            current_map[i] = map[mid_offset + i];
        }
        current_map[target_length] = '\0';

        int comparison = my_strcmp(target, current_map);
        if (comparison == 0) {
            free(current_map);
            return (ssize_t)mid_offset;  
        } else if (comparison < 0) {
            end = mid - 1;  
        } else {
            start = mid + 1;  
        }
    }

    /*Cleanup*/
    free(current_map);
    return -1;
}


/*
    Main logic to handle edge cases
*/
int main(int argc, char const *argv[]){
    int return_code = 0;
    char *lookup_number, *mapping, *message;
    off_t filesize;

    /*Single argument. Tool must fail*/
    if(argc == 1){
        display_error_message("1 or more arguments required\n");
        return 1;
    }

    /*
    * Two args. Read file formatted like nanpa from STDIN and display location associated
    * First Validate argv[1]
    */
    if(argc == 2){
        /*Check if num works*/
        if((lookup_number = valid_number(argv[1])) == NULL){
            display_error_message("Incorrect number format. PLease dial again\n");
            free(lookup_number);
            return 1;
        }

        /*Check if lseek works*/
        /*It doesn't*/
        if((filesize = lseek(STDIN_FILENO,(off_t)0,SEEK_END)) < 0){
            return_code = read_and_find(STDIN_FILENO,lookup_number);
            free(lookup_number);
            return return_code;
        /*lseek works*/
        }else{
            /*Check if file could be mapped*/
            int fd = STDIN_FILENO;
            if((mapping = mmap(NULL,(size_t)filesize,PROT_READ,MAP_PRIVATE,fd,(off_t)0)) == NULL){
                display_error_message("Error mapping file\n");
                free(lookup_number);
                return 1;
            }
            /*Check if string was found*/
            ssize_t found;
            if((found = binary_search(mapping,filesize, lookup_number)) < 0){
                display_error_message("Location not found\n");
                free(lookup_number);
                return 1;
            }
            /*Concatenate message and print*/
            message = my_strcat(add_null_terminator(mapping + found + 6, 25),"\n\0");
            if(better_write(STDOUT_FILENO,message,my_strlen(message)) < 0){
                display_error_message("Error writing location\n");
                free(lookup_number);
                free(message);
                return 1;
            }
            /*Check if unmmaping fails*/
            int return_code;
            if((return_code = munmap(mapping,(size_t)filesize)) < 0){
                display_error_message("Error unmapping file\n");
                free(lookup_number);
                free(message);
                return return_code;
            }
            /*Cleanup*/
            free(lookup_number);
            free(message);
        }
    }

    /*
    *   3 args 
    */
    if(argc == 3){
        /*Check if num works*/
        if((lookup_number = valid_number(argv[1])) == NULL){
            display_error_message("Incorrect number format. PLease dial again\n");
            free(lookup_number);
            return 1;
        }

        /*Open file*/
        int fd;
        if((fd = open(argv[2],O_RDONLY)) < 0){
            display_error_message("Error opening file\n");
            return 1;
        }
        /*Check if lseek works*/
        off_t filesize;

        /*It doesn't*/
        if((filesize = lseek(fd,(off_t)0,SEEK_END)) < 0){
            return_code = read_and_find(fd,lookup_number);
            free(lookup_number);
            if(fd != STDIN_FILENO) close(fd);
            return return_code;
        }else{
            /*Check if mapping failed*/
            if((mapping = mmap(NULL,(size_t)filesize,PROT_READ,MAP_PRIVATE,fd,(off_t)0)) == NULL){
                display_error_message("Error mapping file\n");
                free(lookup_number);
                if(fd != STDIN_FILENO) close(fd);
                return 1;
            }
            /*Check if found*/
            ssize_t found;
            if((found = binary_search(mapping,filesize, lookup_number)) < 0){
                display_error_message("Location not found\n");
                free(lookup_number);
                if(fd != STDIN_FILENO) close(fd);
                return 1;
            }
            /*Concatenate message and print*/
            message = my_strcat(add_null_terminator(mapping + found + 6, 25),"\n\0");
            if(better_write(STDOUT_FILENO,message,my_strlen(message)) < 0){
                display_error_message("Error writing location\n");
                free(lookup_number);
                free(message);
                if(fd != STDIN_FILENO) close(fd);
                return 1;
            }
            /*Unmap*/
            int return_code;
            if((return_code = munmap(mapping,(size_t)filesize)) < 0){
                display_error_message("Error unmapping file\n");
                free(lookup_number);
                free(message);
                if(fd != STDIN_FILENO) close(fd);
                return return_code;
            }

            /*Cleanup*/
            free(lookup_number);
            free(message);
            if(fd != STDIN_FILENO) close(fd);
        }
    }

    /*End reached*/
    return return_code;
}