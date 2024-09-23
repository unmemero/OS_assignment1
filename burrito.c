
#define _DEFAULT_SOURCE 

#include <stdio.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <stdlib.h>

ssize_t better_write(int fd, const char *buf, size_t count) {
  size_t already_written, to_be_written, written_this_time, max_count;
  ssize_t res_write;

  if (count == ((size_t) 0)) return (ssize_t) count;
  
  already_written = (size_t) 0;
  to_be_written = count;
  while (to_be_written > ((size_t) 0)) {
    max_count = to_be_written;
    if (max_count > ((size_t) 8192)) {
      max_count = (size_t) 8192;
    }
    res_write = write(fd, &(((const char *) buf)[already_written]), max_count);
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

int main(int argc, char **argv) {
  int rc;
  char buf[4096];
  ssize_t read_res;
  size_t bytes_read_in, i;
  char byte;
  char *current_line;
  size_t current_line_size, current_line_alloc, new_size;
  void *ptr;
  char **all_lines;
  size_t all_lines_size, all_lines_alloc;
  size_t *all_lines_lengths;

  rc = 0;
  current_line = NULL;
  current_line_size = (size_t) 0;
  current_line_alloc = (size_t) 0;
  all_lines = NULL;
  all_lines_lengths = NULL;
  all_lines_size = (size_t) 0;
  all_lines_alloc = (size_t) 0;
  for (;;) {
    read_res = read(0, buf, sizeof(buf));
    if (read_res < ((ssize_t) 0)) {
      fprintf(stderr, "Error with read(): %s\n",
	      strerror(errno));
      rc = 1;
      goto cleanup_and_return;
    }
    if (read_res == ((ssize_t) 0)) {
      /* We have hit the EOF condition */
      break;
    }
    bytes_read_in = (size_t) read_res;
    for (i=(size_t) 0; i<bytes_read_in; i++) {
      byte = buf[i];
      /* Handle the byte named byte that is part of what we just read
	 in. 

	 Put the byte into the current line, if it is a newline or
	 not.

      */
      if (current_line == NULL) {
        /* This is the very first byte of the current line */
        current_line_alloc = (size_t) 1;
        ptr = calloc(current_line_alloc, sizeof(char));
        if (ptr == NULL) {
          fprintf(stderr, "Error: could not allocate memory\n");
          rc = 1;
          goto cleanup_and_return;
        }
        current_line = (char *) ptr;
      } else {
	/* This is a subsequent byte in the current line, which is
	   already allocated 
	*/
	if (current_line_size >= current_line_alloc) {
	  /* Here we need to reallocate the current line */
	  new_size = current_line_alloc + current_line_alloc;
	  if (new_size < current_line_alloc) {
	    fprintf(stderr, "Error: cannot handle memory that large\n");
	    rc = 1;
	    goto cleanup_and_return;
	  }
	  ptr = reallocarray(current_line, new_size, sizeof(char));
	  if (ptr == NULL) {
	    fprintf(stderr, "Error: could not allocate memory\n");
	    return 1;
	  }
	  current_line_alloc = new_size;
	  current_line = ptr;
	}
      }
      /* The current line, represented by current_line 
	 points to memory and there is at least 1 byte left for 
	 our current to be put in.
      */
      current_line[current_line_size] = byte;
      current_line_size++;

      /* If the byte we just stored was a newline character, we 
	 need to start a new current line.

	 So, we need to store away the current line first.
      */
      if (byte == '\n') {
	if (all_lines == NULL) {
	  all_lines_alloc = (size_t) 1;
	  ptr = calloc(all_lines_alloc, sizeof(char *));
	  if (ptr == NULL) {
	    fprintf(stderr, "Error: could not allocate memory\n");
	    rc = 1;
	    goto cleanup_and_return;
	  }
	  all_lines = (char **) ptr;
	  ptr = calloc(all_lines_alloc, sizeof(size_t));
	  if (ptr == NULL) {
	    fprintf(stderr, "Error: could not allocate memory\n");
	    rc = 1;
	    goto cleanup_and_return;
	  }
	  all_lines_lengths = (size_t *) ptr;
	} else {
	  if (all_lines_size >= all_lines_alloc) {
	    new_size = all_lines_alloc + all_lines_alloc;
	    if (new_size < all_lines_alloc) {
	      fprintf(stderr, "Error: cannot handle memory that large\n");
	      rc = 1;
	      goto cleanup_and_return;
	    }
	    ptr = reallocarray(all_lines, new_size, sizeof(char *));
	    if (ptr == NULL) {
	      fprintf(stderr, "Error: could not allocate memory\n");
	      return 1;
	    }
	    all_lines = (char **) ptr;
	    ptr = reallocarray(all_lines_lengths, new_size, sizeof(size_t));
	    if (ptr == NULL) {
	      fprintf(stderr, "Error: could not allocate memory\n");
	      rc = 1;
	      goto cleanup_and_return;
	    }
	    all_lines_lengths = (size_t *) ptr;
	    all_lines_alloc = new_size;
	  }
	}
	all_lines[all_lines_size] = current_line;
	all_lines_lengths[all_lines_size] = current_line_size;
	all_lines_size++;
	current_line = NULL;
	current_line_size = (size_t) 0;
	current_line_alloc = (size_t) 0;
      }
    }
  }

  /* Here, we have read all input.

     There is one special case that we need to handle:

     If we have started a current line but that line
     did not end in a newline character, we still need to
     put the current line into the all lines array.

  */
  if (current_line != NULL) {
    if (all_lines == NULL) {
      all_lines_alloc = (size_t) 1;
      ptr = calloc(all_lines_alloc, sizeof(char *));
      if (ptr == NULL) {
	fprintf(stderr, "Error: could not allocate memory\n");
	rc = 1;
	goto cleanup_and_return;
      }
      all_lines = (char **) ptr;
      ptr = calloc(all_lines_alloc, sizeof(size_t));
      if (ptr == NULL) {
	fprintf(stderr, "Error: could not allocate memory\n");
	rc = 1;
	goto cleanup_and_return;
      }
      all_lines_lengths = (size_t *) ptr;
    } else {
      if (all_lines_size >= all_lines_alloc) {
	new_size = all_lines_alloc + all_lines_alloc;
	if (new_size < all_lines_alloc) {
	  fprintf(stderr, "Error: cannot handle memory that large\n");
	  rc = 1;
	  goto cleanup_and_return;
	}
	ptr = reallocarray(all_lines, new_size, sizeof(char *));
	if (ptr == NULL) {
	  fprintf(stderr, "Error: could not allocate memory\n");
	  rc = 1;
	  goto cleanup_and_return;
	}
	all_lines = (char **) ptr;
	ptr = reallocarray(all_lines_lengths, new_size, sizeof(size_t));
	if (ptr == NULL) {
	  fprintf(stderr, "Error: could not allocate memory\n");
	  rc = 1;
	  goto cleanup_and_return;
	}
	all_lines_lengths = (size_t *) ptr;
	all_lines_alloc = new_size;
      }
    }
    all_lines[all_lines_size] = current_line;
    all_lines_lengths[all_lines_size] = current_line_size;
    all_lines_size++;
    current_line = NULL;
    current_line_size = (size_t) 0;
    current_line_alloc = (size_t) 0;
  }
  
  /* Here, we have an array all_lines with all the lines
     
     And we can output these lines again, using write()
     resp. better_write().

  */
  for (i=(size_t) 0; i<all_lines_size; i++) {
    if (better_write(1, all_lines[i], all_lines_lengths[i]) < ((ssize_t) 0)) {
      fprintf(stderr, "Error with write(): %s\n",
	      strerror(errno));
      rc = 1;
      goto cleanup_and_return;
    }
  }

 cleanup_and_return:
  if (current_line != NULL) {
    free(current_line);
  }
  if (all_lines != NULL) {
    for (i=(size_t) 0; i<all_lines_size; i++) {
      free(all_lines[i]);
    }
    free(all_lines);
    free(all_lines_lengths);
  }
  
  return rc;
}
