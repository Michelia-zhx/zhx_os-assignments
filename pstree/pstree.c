#include <stdio.h>
#include <string.h>
#include <dirent.h>
#include <assert.h>

typedef struct PNode{
  char *pname;
  int pid; 
  int ppid;
  struct PNode *l_child;
  struct PNode *r_bro;
}root;

extern void show_version();

int main(int argc, char *argv[]) {
  int show_p = 0;
  int num_s = 0;
  int show_v = 0;

  for (int i = 0; i < argc; i++) {
    assert(argv[i]);
    if (strcmp(argv[i], "-p") == 0 || strcmp(argv[i], "--show-pids") == 0) show_p = 1;
    if (strcmp(argv[i], "-n") == 0 || strcmp(argv[i], "--numeric-sort") == 0) num_s = 1;
    if (strcmp(argv[i], "-V") == 0 || strcmp(argv[i], "--version") == 0) show_v = 1;
    printf("argv[%d] = %s\n", i, argv[i]);
  }
  printf("p: %d, n: %d, V: %d\n", show_p, num_s, show_v);

  assert(!argv[argc]);
  if (show_v == 1){
    show_version();
    return 0;
  }

  DIR *dir_ptr;
  struct dirent *direntp;
  dir_ptr = opendir("~/Documents");
  if (dir_ptr == NULL){
    fprintf(stderr, "ls: cannot open /proc");
  }
  else {
    direntp = readdir(dir_ptr);
    while (direntp != NULL){
      printf("%s\n", direntp->d_name);
    }
    closedir(dir_ptr);
  }

  return 0;
}

void show_version(){
  printf("pstree 1.0\nCopyright (C) 2020 Hanxiao Zhang (a vegetable bird)\n");
  printf("\nThis is a poor and free software\nAnd if anything goes wrong, don't ask me.\n");
  return;
}