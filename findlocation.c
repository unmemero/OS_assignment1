#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <fcntl.h>
#include "better_write.h"
#include <sys/mman.h>

int compare_code(const char *target, const char *line){
    ssize_t target_count = my_strlen(target);
    for(int i = 0; i < target_count; i++){
        if(target[i] > line[i]){
            return -1;
        }
        if(target[i] < line[i]){
            return 1;
        }
    }
    return 0;
}

ssize_t binary_search(const char *map, size_t fileSize, const char *target){
    int start = 0;
    int end = fileSize - 1;

    while( start <= end){
        int mid = start + (end - start) / 2;
        int comparison = compare_code(target, map[mid]);
        if(comparison == 0){
            return (ssize_t) mid;
        }
        if(comparison == -1){
            end = mid - 1;
        } else if(comparison == 1){
            start = mid + 1;
        }
    }
    return -1;
}

/*
    Use buffers and reading if file is non-seekable
*/
int find_non_mappable(int fd, const char *arg){
    ssize_t chars_read = 0;
    size_t buffer_size = 8192, position = 0;
    char * buffer;
    char current_char;
    
    if((buffer = calloc(buffer_size, sizeof(char))) == NULL){
        display_error_message("Error: Memory allocation failed\n");
        if(fd != STDIN_FILENO) close(fd);
        return 1;
    } 
    
    ssize_t read_result;
    if((read_result = read(fd, &current_char,1)) < 0){
        display_error_message("Error reading file\n");
        free(buffer);
        if(fd != STDIN_FILENO) close(fd);
        return 1;
    }

    while(read_result != 0){
        buffer[position++] = current_char;
        chars_read ++;

        if(position == buffer_size){
            buffer_size += buffer_size;
            char *new_buffer;
            if((new_buffer= realloc(buffer, buffer_size)) == NULL){
                display_error_message("Error reallocating array");
                free(buffer);
                if(fd != STDIN_FILENO) close(fd);
                return 1;
            }
            buffer = new_buffer;
        }
    }
    
    
}

int find_mappable(int fd, off_t fileSize, const char *target){

    ssize_t result;
    char *mapped;

    mapped = mmap(NULL, fileSize, PROT_READ, MAP_PRIVATE,fd, 0);
    if(mapped == MAP_FAILED){
        display_error_message("Error: Mapping\n");
        return 1;
    }
    result = binary_search(mapped, fileSize, target);
    if(result < (ssize_t) 0){
        if(munmap(mapped, fileSize) < (ssize_t) 0){
            display_error_message("Error: Unmapping\n");
            return 1;
        }
        close(fd);
        better_write(1, "Target not found\n", sizeof("Target not found\n"));
        return 0;
    }
    better_write(1, mapped[result], sizeof(mapped[result]));
    if(munmap(mapped, fileSize) < (ssize_t) 0){
        display_error_message("Error: Unmapping\n");
        return 1;
    }
    close(fd);
    return result;
}



int main(int argc, char const *argv[]){

    char target[6];
    size_t target_count;
    off_t fileSize;
    char *mapped;

    int fd;

    ssize_t result;
    
    /* If no arguments program fails*/
    if(argc < (size_t) 2){
        display_error_message("Error: Invalid Argument\n");
        return 1;
    }

    /* Sets the size of our target*/
    if(my_strlen(argv[1]) < sizeof(target)) target_count = my_strlen(argv[1]); 
    else target_count = sizeof(target);

    /* If we only have file name and target we look search for target in nanpa*/
    if(argc == (size_t) 2) fd = open("nanpa", O_RDONLY);
    else fd = open(argv[2], O_RDONLY);
    
    if(fd < (ssize_t) 0){
        display_error_message("Error opening file\n");
        return 1;
    }
    for(int i = 0; i < target_count; i++){
        if(is_number(argv[1][i])){
            target[i] = argv[1][i];
        } else{
            display_error_message("Error: Invalid Input\n");
            return 1;
        }
    }
    fileSize = lseek(fd, 0, SEEK_END);
    if(fileSize < (ssize_t) 0){
        /* If file is not mappable*/
        file_non_mappable(fd, argv[1]);
    } else{
        find_mappable(fd, fileSize, target);
    }
    return result;
}