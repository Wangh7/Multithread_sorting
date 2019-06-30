#include <stdio.h>
#include <string.h>
#include <pthread.h>
#include <dirent.h>
#include <fcntl.h>
#include <unistd.h>
#define MAX 150
pthread_t id[10];
char path[100];
char filepathlist[MAX][MAX];
int len = 0;
int threadcount = 0;
int sortthread(void *arg){
  int i = *(int *)arg;
  int l = i;
  printf("thread %d is starting\n",i);
  while(l<len){
    char* filepath = filepathlist[l];
    char temp[1000];
    char old[1000];
    int fd,nbyte;
    if((fd = open(filepath,O_RDWR)) == -1){
      printf("thread %d open file %d filed!\n",i,l);
      return 1;
    }
    memset(temp,0,sizeof(temp));
    memset(old,0,sizeof(old));
    nbyte = read(fd,temp,sizeof(temp));
    read(fd,old,sizeof(old));
    for(int a = 0;a < nbyte;a++){
      for(int b = 0;b < nbyte - i;b++){
        if(temp[b] > temp[b + 1]){
          int t = temp[b];
          temp[b] = temp[b + 1];
          temp[b + 1] = t;
        }
      }
    }
    lseek(fd,0,SEEK_SET);
    write(fd,temp,nbyte);
    close(fd);
    printf("thread %d sort file %d finished!\n",i,l);
    //printf("thread %d sort file %d finished! old:%s new:%s\n",i,l,old,temp);
    l += threadcount;
  }
  printf("thread %d is over!\n",i);
  return 0;
}

int getpathlist(char path[]){
  DIR *d;
  struct dirent *f;
  char temppath[MAX];
  if(!(d = opendir(path))){
    printf("Can't open %s\n",path);
    return 1;
  }
  while((f = readdir(d)) != NULL){
    if(strncmp(f->d_name,".",1) == 0)
      continue;
    if(f->d_type == 8){
      char txt[5];
      for(int i=0;i<4;i++)
        txt[i] = f->d_name[strlen(f->d_name)-4+i];
      txt[4]='\0';
      if(strcmp(txt,".txt") == 0){
        char filepath[MAX];
        strcpy(filepath,path);
        strcat(filepath,"/");
        strcat(filepath,f->d_name);
        strcpy(filepathlist[len++],filepath);
      }
    }
    if(f->d_type == 4){
      strcpy(temppath,path);
      strcat(temppath,"/");
      strcat(temppath,f->d_name);
      getpathlist(temppath);
    }
  }
  closedir(d);
  return 0;
}

int main(void){
  int ret0,ret1;
  int ret[10];
  int index[10] = {0,1,2,3,4,5,6,7,8,9};
  char basepath[MAX];
  printf("Please enter the path:\n");
  ///Users/wangh7/Desktop/多线程/word
  scanf("%s",basepath);
  do{
    printf("Please enter the number of thread(<=10):\n");
    scanf("%d",&threadcount);
  }while(threadcount>10&&printf("Too large!\n"));
  if(getpathlist(basepath))
    return 1;
  printf("found %d file(s):\n",len);
  for(int i = 0; i < len; i++)
    printf("%s\n", filepathlist[i]);
  for(int i=0;i<threadcount;i++){
    ret[i] = pthread_create(&id[i],NULL,(void *) sortthread,&index[i]);
  }
  for(int i=0;i<threadcount;i++){
    if(ret[i]!= 0){
      printf("Create thread error!\n");
      return 1;
    }
  }
  for(int i=0;i<threadcount;i++){
    pthread_join(id[i],NULL);
  }
  return 0;
}