#include <stdio.h>
#include <string.h>
#include <assert.h>

int main(int argc, char *argv[]) {
  int show_pid = 0;
  int num_sort = 0;
  int show_version = 0;
  for (int i = 0; i < argc; i++) {
    assert(argv[i]);
    if (strcmp(argv[i], "-p") == 0 || strcmp(argv[i], "--show-pids") == 0) show_pid = 1;
    else if (strcmp(argv[i], "-n") == 0 || strcmp(argv[i], "--numeric-sort") == 0) num_sort = 1;
    else if (strcmp(argv[i], "-V") == 0 || strcmp(argv[i], "--version") == 0) show_version = 1;
    else {printf("illegal args\n"); assert(0);}
    printf("argv[%d] = %s\n", i, argv[i]);
  }
  printf("p: %d, n: %d, V: %d\n", show_pid, num_sort, show_version);
  assert(!argv[argc]);
  return 0;
}
