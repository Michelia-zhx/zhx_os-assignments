#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <dirent.h>
#include <assert.h>
#include <unistd.h>
#include <sys/types.h>

typedef struct Node{
  char *pname;
  int pid; 
  int ppid;
  int space_num;
  struct Node *l_child;
  struct Node *r_bro;
}PNode;

extern void show_version();
extern pid_t *get_pids(int *num_pid, pid_t *max_pid);
extern pid_t get_num(char *line);
extern void print_tree(int show_p, PNode *root);

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
  pid_t max_pid = 0;
  pid_t *sys_pids = get_pids(&num_pid, &max_pid);
  PNode *ps_tree = (PNode *)malloc((max_pid + 1)*sizeof(PNode));
  for (int i = 0; i < max_pid+1; i ++){
    PNode *pnode = &(ps_tree[i]);
    pnode->pname = NULL;
    pnode->pid = 0;
    pnode->ppid = 0;
    pnode->space_num = 0;
    pnode->l_child = NULL;
    pnode->r_bro = NULL;
  }
  for (int i = 0; i < num_pid; i ++){
    // get the information of each node
    pid_t pid = sys_pids[i];
    char filename[25];
    sprintf(filename, "/proc/%d/status", pid);
    // printf("%s\n", filename);
    FILE *status = fopen(filename, "r");
    if (status == NULL){
      printf("error when opening %s\n", filename);
      exit(-1);
    }

    // get p_name of the process
    char * name_buff = (char *)malloc(50*sizeof(char));
    fscanf(status, "%s", name_buff);
    free(name_buff);
    fscanf(status, "%s", name_buff);
    printf("pname: %s", name_buff);
    
    char * temp_buff = (char *)malloc(50*sizeof(char));
    for (int i = 0; i < 5; i ++){
      fgets(temp_buff, 50, status);
    }
    free(temp_buff);
    //  get pid of the process
    char pid_buff[50];
    fscanf(status, 50, pid_buff);
    free(pid_buff);
    fscanf(status, 50, pid_buff);
    pid_t pid_status = atoi(pid_buff);
    assert(pid_status == pid);
    printf("pid_buff: %s, pid_status: %d\n", pid_buff, pid_status);
    //  get parent pid of the process
    char ppid_buff[50];
    fscanf(status, 50, ppid_buff);
    free(pid_buff);
    fscanf(status, 50, ppid_buff);
    pid_t ppid = atoi(ppid_buff);
    printf("ppid_buff: %s, ppid_status: %d\n", ppid_buff, ppid);
    fclose(status);
    
    // build the tree
    PNode *cur_node = &(ps_tree[pid]);
    cur_node->pname = name_buff;
    cur_node->pid = pid;
    cur_node->ppid = ppid;
    PNode *p_node = &(ps_tree[ppid]);
    if (p_node->l_child == NULL) {
      p_node->l_child = cur_node;
      p_node->space_num = 0;
    }
    else {
      if (!num_s){  // if not need to sort by pid
        PNode *temp = p_node->l_child;
        while(temp->r_bro != NULL)  temp = temp->r_bro;
        temp->r_bro = cur_node;
        cur_node->space_num = p_node->space_num + strlen(p_node->pname) + 2;
        if (p_node == &(ps_tree[1])) cur_node->space_num -= 2;
      }
      else {  // insert node by pid
        PNode *temp1 = p_node->l_child;
        PNode *temp2 = p_node->l_child;
        temp2 = temp2->r_bro;
        if (temp1->pid > pid){  // insert at the head of child_node list
          cur_node->r_bro = temp1;
          p_node->l_child = cur_node;
          cur_node->space_num = 0;
          temp1->space_num = p_node->space_num + strlen(p_node->pname) + 2;
          if (p_node == &(ps_tree[1])) temp1->space_num -= 2;
        }
        else {  // insert at the middle or tail of child_node list
          if (!temp2){
            temp1->r_bro = cur_node;
            cur_node->r_bro = NULL;
            cur_node->space_num = p_node->space_num + strlen(p_node->pname) + 2;
            if (p_node == &(ps_tree[1])) cur_node->space_num -= 2;
          }
          while (temp2){
            assert(temp1->pid != pid && temp2->pid != pid);
            if (temp1->pid < pid && pid < temp2->pid){
              cur_node->r_bro = temp1->r_bro;
              temp1->r_bro = cur_node;
              cur_node->space_num = temp2->space_num;
              break;
            }
            temp1 = temp1->r_bro;
            temp2 = temp2->r_bro;
            if (!temp2){
              temp1->r_bro = cur_node;
              cur_node->r_bro = NULL;
              cur_node->space_num = p_node->space_num + strlen(p_node->pname) + 2;
              if (p_node == &(ps_tree[1])) cur_node->space_num -= 2;
            }
          }
        }
      }
    }
  }

  for (int i = 1; i <= max_pid; i ++){
    PNode *a = &(ps_tree[i]);
    printf("pid: %d, ppid: %d\n", a->pid, a->ppid);
  }
  
  print_tree(show_p, &(ps_tree[1]));

  return 0;
}

void show_version(){
  printf("pstree 1.0\nCopyright (C) 2020 Hanxiao Zhang (a vegetable bird)\n");
  printf("\nThis is a poor and free software\nAnd if anything goes wrong, don't ask me.\n");
  return;
}

pid_t *get_pids(int *num_pid, pid_t *max_pid){
  DIR *dir;
  struct dirent *ptr;
  dir = opendir("/proc");
  if (dir == NULL){
    printf("error when opening /proc\n");
    exit(-1);
  }
  int list_size = 300;  // initial size of pid list
  pid_t *sys_pids = (pid_t *)malloc(list_size*sizeof(pid_t));
  pid_t *new_list = NULL;
  int i = 0;
  while ((ptr=readdir(dir)) != NULL){
    if (ptr->d_type == 4 && strspn(ptr->d_name, "0123456789") == strlen(ptr->d_name)){
      sys_pids[i] = atoi(ptr->d_name);
      if (*max_pid < atoi(ptr->d_name)) *max_pid = atoi(ptr->d_name);
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
  printf("max_pid: %d, num_pid: %d, i: %d\n",*max_pid, *num_pid, i);
  return sys_pids;
}

pid_t get_num(char *line){
  pid_t id = 0;
  char *ptr = line;
  while ((*ptr) != '\0'){
    if ('0' <= (*ptr) && (*ptr) <= '9')
      id = id*10+((*ptr)-'0');
    ptr ++;
  }
  return id;
}

void print_tree(int show_p, PNode *root){
  return;
}
