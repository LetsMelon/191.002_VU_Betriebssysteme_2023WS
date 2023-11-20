#include <fcntl.h>
#include <stdio.h>
#include <sys/mman.h>
#include <unistd.h>

#include "shared_memory.h"

#define SHARED_MEMORY_NAME "/12220857_shm"

static shared_memory_t *sm_open(int *fd, bool is_master, int flags) {
  *fd = shm_open(SHARED_MEMORY_NAME, flags, 0600);
  if (*fd == -1) {
    return NULL;
  }

  if (is_master == true) {
    if (ftruncate(*fd, sizeof(shared_memory_t)) < 0) {
      shm_unlink(SHARED_MEMORY_NAME);
      close(*fd);

      return NULL;
    }
  }

  shared_memory_t *shared_memory;
  shared_memory = mmap(NULL, sizeof(shared_memory_t), PROT_READ | PROT_WRITE,
                       MAP_SHARED, *fd, 0);
  if (shared_memory == MAP_FAILED) {
    shm_unlink(SHARED_MEMORY_NAME);
    close(*fd);

    return NULL;
  }

  if (is_master == true) {
    shared_memory->read_index = 0;
    shared_memory->write_index = 0;
  }

  return shared_memory;
}

shared_memory_t *sm_open_slave(int *fd) { return sm_open(fd, false, O_RDWR); }

shared_memory_t *sm_open_master(int *fd) {
  return sm_open(fd, true, O_RDWR | O_CREAT);
}

static int sm_close(shared_memory_t *shm, int fd, bool is_master) {
  int status = 0;

  if (munmap(shm, sizeof(*shm)) != 0) {
    status = -1;
  }

  if (close(fd) != 0) {
    status = -1;
  }

  if (is_master) {
    if (shm_unlink(SHARED_MEMORY_NAME) != 0) {
      status = -1;
    }
  }
  return status;
}

int sm_close_slave(shared_memory_t *shared_memory, int fd) {
  return sm_close(shared_memory, fd, false);
}

int sm_close_master(shared_memory_t *shared_memory, int fd) {
  return sm_close(shared_memory, fd, true);
}
