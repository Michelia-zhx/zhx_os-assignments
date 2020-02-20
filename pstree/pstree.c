#include <stdio.h>
#include <assert.h>

int main(int argc, char *argv[]) {
  int show_pid = 0;
  int num_sort = 0;
  int show_version = 0;
  for (int i = 0; i < argc; i++) {
    assert(argv[i]);
    if (argv[i] == "-p" || argv[i] == "--show-pids") show_pid = 1;
    if (argv[i] == "-n" || argv[i] == "--numeric-sort") num_sort = 1;
    if (argv[i] == "-V" || argv[i] == "--version") show_version = 1;
    printf("argv[%d] = %s\n", i, argv[i]);
  }
  printf("p: %d, n: %d, V: %d\n", show_pid, num_sort, show_version);
  assert(!argv[argc]);
  return 0;
}
