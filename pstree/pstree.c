#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <dirent.h>
#include <assert.h>
#include <unistd.h>
#include <sys/types.h>

typedef struct PNode{
  char *pname;
  int pid; 
  int ppid;
  struct PNode *l_child;
  struct PNode *r_bro;
}root;

extern void show_version();
extern pid_t *get_pids(int *num_pid);

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

  int num_pid = 0;
  pid_t *pids = get_pids(&num_pid);

  return 0;
}

void show_version(){
  printf("pstree 1.0\nCopyright (C) 2020 Hanxiao Zhang (a vegetable bird)\n");
  printf("\nThis is a poor and free software\nAnd if anything goes wrong, don't ask me.\n");
  return;
}

pid_t *get_pids(int *num_pid){
  DIR *dir;
  struct dirent *ptr;
  dir = opendir("/proc");
  if (dir == NULL){
    perror("cannot open /proc\n");
    exit(-1);
  }
  int list_size = 100;  // initial size of pid list
  pid_t *sys_pids = (pid_t *)malloc(list_size*sizeof(pid_t));
  pid_t *new_list = NULL;
  int i = 0;
  while ((ptr=readdir(dir)) != NULL){
    if (ptr->d_type == 4 && strspn(ptr->d_name, "0123456789") == strlen(ptr->d_name)){
      sys_pids[i] = atoi(ptr->d_name);
      printf("%d  ", atoi(ptr->d_name));
      if ((i+1)%10 == 0) printf("\n");
      (*num_pid) ++;
      i ++;
      if (i == list_size){ // if the list is not long enough, expand the pid list
        list_size += 100;
        new_list = (pid_t *)malloc(list_size*sizeof(pid_t));
        for (int j = 0; j < i; j ++){
          new_list[j] = sys_pids[j];
        }
        free(sys_pids);
        sys_pids = new_list;
      }
    }
  }
  closedir(dir);
  if (!sys_pids) {
    exit(-1);
  }
  printf("num_pid: %d, i: %d\n", *num_pid, i);
  return sys_pids;
}