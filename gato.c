
#include <stdio.h>
#include <errno.h>
#include <string.h>
#include <unistd.h>
#include "better_write.h"

int main(int argc, char **argv) {
  ssize_t read_res;
  char buf[4096];
  size_t bytes_to_be_written;

  for (;;) {
    read_res = read(0, buf, sizeof(buf));
    if (read_res == ((ssize_t) 0)) {
      /* We are done, as we have hit EOF */
      break;
    }
    if (read_res < ((ssize_t) 0)) {
      /* There has been an error on read() */
      fprintf(stderr, "Error using read: %s\n",
	      strerror(errno));
      return 1;
    }
    /* Print out what we just read */
    bytes_to_be_written = (size_t) read_res;
    if (better_write(1, buf, bytes_to_be_written) < ((ssize_t) 0)) {
      /* There has been an error on write() */
      fprintf(stderr, "Error using write: %s\n",
	      strerror(errno));
      return 1;
    }
  }
  return 0;
}
