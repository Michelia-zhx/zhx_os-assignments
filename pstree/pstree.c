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
  int left_dis;
  int len_pid;
  struct Node *l_child;
  struct Node *r_bro;
}PNode;

extern void show_version();
extern pid_t *get_pids(int *num_pid, pid_t *max_pid);
extern int get_len(int pid);
extern void print_tree(int show_p, PNode *root);
extern void helper(int show_p, PNode *root);

int main(int argc, char *argv[]) {
  putchar(10);
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
  if (num_s) show_p = 1;
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
    pnode->left_dis = 0;
    pnode->len_pid = 0;
    pnode->l_child = NULL;
    pnode->r_bro = NULL;
  }
  for (int i = 0; i < num_pid; i ++){
    // printf("loop %d\n", i+1);
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
    // printf("1\n");
    // get p_name of the process
    char * temp_buff = (char *)malloc(50*sizeof(char));
    fscanf(status, "%s", temp_buff);
    char * name_buff = (char *)malloc(50*sizeof(char));
    fscanf(status, "%s", name_buff);
    
    for (int i = 0; i < 5; i ++){
      fgets(temp_buff, 50, status);
    }
    //  get pid of the process
    fscanf(status, "%s", temp_buff);

    char * pid_buff = (char *)malloc(50*sizeof(char));
    fscanf(status, "%s", pid_buff);
    pid_t pid_status = atoi(pid_buff);
    assert(pid_status == pid);
    // printf("pid_buff: %s, pid_status: %d\n", pid_buff, pid_status);
    
    fscanf(status, "%s", temp_buff);

    //  get parent pid of the process
    char * ppid_buff = (char *)malloc(50*sizeof(char));
    fscanf(status, "%s", ppid_buff);
    pid_t ppid = atoi(ppid_buff);
    // printf("ppid_buff: %s, ppid_status: %d\n", ppid_buff, ppid);
    fclose(status);
    // printf("2\n");
    
    // build the tree
    PNode *cur_node = &(ps_tree[pid]);
    printf("pname: %s\n", name_buff);
    cur_node->pname = name_buff;
    cur_node->pid = pid;
    cur_node->ppid = ppid;
    cur_node->len_pid = get_len(pid);
    PNode *p_node = &(ps_tree[ppid]);
    if (p_node->pid == 0) {
      cur_node->left_dis = 0;
      continue;
    }
    if (p_node->l_child == NULL) {
      p_node->l_child = cur_node;
      cur_node->left_dis = p_node->left_dis + strlen(p_node->pname) + 2;
      if (show_p) cur_node->left_dis += 2+p_node->len_pid;
    }
    else {
      if (!num_s){  // if not need to sort by pid
        PNode *temp = p_node->l_child;
        while(temp->r_bro != NULL)  temp = temp->r_bro;
        temp->r_bro = cur_node;
        cur_node->left_dis = p_node->left_dis + strlen(p_node->pname) + 2;
        if (show_p) cur_node->left_dis += 2+p_node->len_pid;
      }
      else {  // insert node by pid
        PNode *temp1 = p_node->l_child;
        PNode *temp2 = p_node->l_child;
        temp2 = temp2->r_bro;
        if (temp1->pid > pid){  // insert at the head of child_node list
          cur_node->r_bro = temp1;
          p_node->l_child = cur_node;
          cur_node->left_dis = 0;
          temp1->left_dis = p_node->left_dis + strlen(p_node->pname) + 2;
          if (show_p) cur_node->left_dis += 2+p_node->len_pid;
        }
        else {  // insert at the middle or tail of child_node list
          if (!temp2){
            temp1->r_bro = cur_node;
            cur_node->r_bro = NULL;
            cur_node->left_dis = p_node->left_dis + strlen(p_node->pname) + 2;
            if (show_p) cur_node->left_dis += 2+p_node->len_pid;
          }
          while (temp2){
            assert(temp1->pid != pid && temp2->pid != pid);
            if (temp1->pid < pid && pid < temp2->pid){
              cur_node->r_bro = temp1->r_bro;
              temp1->r_bro = cur_node;
              cur_node->left_dis = temp2->left_dis;
              break;
            }
            temp1 = temp1->r_bro;
            temp2 = temp2->r_bro;
            if (!temp2){
              temp1->r_bro = cur_node;
              cur_node->r_bro = NULL;
              cur_node->left_dis = p_node->left_dis + strlen(p_node->pname) + 2;
              if (show_p) cur_node->left_dis += 2 + p_node->len_pid;
            }
          }
        }
      }
    }
  }

  for (int i = 1; i <= max_pid; i ++){
    PNode *a = &(ps_tree[i]);
    if (a->pid != 0) printf("pname: %s, pid: %d, ppid: %d\n",a->pname, a->pid, a->ppid);
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
  printf("\n");
  closedir(dir);
  if (!sys_pids) {
    exit(-1);
  }
  // printf("max_pid: %d, num_pid: %d, i: %d\n",*max_pid, *num_pid, i);
  return sys_pids;
}

int get_len(int pid){
  int len = 1;
  while (pid/10 != 0){
    len ++;
    pid /= 10;
  }
  return len;
}

void print_tree(int show_p, PNode *root){
  if (!root) return;
  if (root->pid == 0) assert(0);
  helper(show_p, root);
}

void helper(int show_p, PNode *root){
  if (!root) {
    // printf("\n");
    return;
  }
  if (root->pid == 0) assert(0);
  if (root->l_child) (root->l_child)->left_dis = 2;
  for (int i = 0; i < root->left_dis; i ++) printf(" ");
  printf("%s", root->pname);
  if (show_p) printf("(%d)", root->pid);
  PNode *child = root->l_child;
  if (!child) printf("\n");
  while (child != NULL){
    helper(show_p, child);
    child = child->r_bro;
  }
  // printf("\n");
  return;
}