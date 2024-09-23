#include <unistd.h>

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

int my_strlen(const char *arg){
    int i = 0;
    while(arg[i] != '\0') i++;
    return i;
}

int my_strcmp(const char *arg, const char *cmp){
    while(*arg != '\0' && *cmp != '\0'){
        if(*arg != *cmp) return 1;
        arg++;
        cmp++;
    }
    if(*arg == '\0' && *cmp == '\0') return 0;
    return 1;
}

int my_atoi(const char *arg){
  int result = 0;
  if (arg[0] == '-') result = -1;
  for(int i = 0; arg[i] != '\0'; i++){
    result = result * 10 + arg[i] - '0';
  }
  return result;
}