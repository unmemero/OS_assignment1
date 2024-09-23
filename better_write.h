#ifndef BETTER_WRITE_H
#define BETTER_WRITE_H

ssize_t better_write(int fd, const char *buf, size_t count);
int is_number(const char *arg);
int my_strlen(const char *arg);
int my_strcmp(const char *arg, const char *cmp);
int my_atoi(const char *arg);

#endif