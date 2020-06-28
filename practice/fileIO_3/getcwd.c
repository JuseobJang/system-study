#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/wait.h>
#include <dirent.h>
#include <string.h>

void creatEnv();

/**
 * 파일을 복사하여 사본을 생성한다. 
 * @param buf   경로명을 저장할 버퍼 변수.
 * @param size  버퍼 변수의 크기
 * @return 버퍼 변수의 시작 주소, 즉 buf
 */
char *mygetcwd(char *buf, size_t size) {
  DIR *c_drip, *p_drip;
  struct dirent *c_dp, *p_dp;
  long c_inode,p_inode;
  int errno;
  char *name;

  c_drip = opendir(".");
  while(1){
    c_dp = readdir(c_drip);
    if(strcmp(c_dp->d_name,".")==0){
      break;
    }
  }
  c_inode = c_dp -> d_ino;


  p_drip = opendir("..");
  while(1){
    p_dp = readdir(p_drip);
    if(strcmp(p_dp->d_name,".")==0){
      break;
    }
  }
  p_inode = p_dp -> d_ino; 

  while(c_inode != p_inode){
    rewinddir(p_drip);
    while((p_dp = readdir(p_drip))!=NULL){
      p_inode = p_dp -> d_ino;
      if(c_dp -> d_ino == p_dp -> d_ino){
        name = p_dp -> d_name;
        name = strcat(name, "/");
        buf = strcat(name,buf);
        break;
      }
    }  
    if((errno = chdir(".."))==-1){
      printf("error to chdir");
    }
    else{
      c_drip = opendir(".");
      while(1){
        c_dp = readdir(c_drip);
        if(strcmp(c_dp->d_name,".")==0){
          break;
        }
      }   
      c_inode = c_dp -> d_ino;

      p_drip = opendir("..");
      while(1){
        p_dp = readdir(p_drip);
        if(strcmp(p_dp->d_name,".")==0){
          break;
        }
      } 
      p_inode = p_dp -> d_ino; 
    }


    
  }
  char ans[255]="";
  ans[0]='/';
  strcat(ans, buf);
  for(int i=0;ans[i];i++)
  {
    if(!ans[i+1])
    {
      ans[i] = '\0';
    }
  }
  buf = ans;
  
  return buf;
}

int main(void) {
  pid_t pid;
  int status;
  char buf[255];

  creatEnv();
  chdir("dir/sub");

  printf("original func: %s\n", getcwd(NULL, 0));
  printf("mygetcwd func: %s\n", mygetcwd(buf, 255));

  return 0;
}

void creatEnv(){
  mkdir("dir", 0755);
  mkdir("dir/sub", 0755);
  mkdir("dir/sub2", 0);
  
  creat("dir/a", 0755);
  creat("dir/b", 0755);
  creat("dir/sub/x", 0755);
  symlink("dir/a", "dir/sl");
  symlink("dir/x", "dir/dsl");
}