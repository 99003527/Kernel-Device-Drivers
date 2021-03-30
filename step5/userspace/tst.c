#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <fcntl.h>
int main(){

 int ret,fd,nbytes;
 fd=open("/dev/psample", O_RDWR);

 if(fd<0) 
 {
  perror("open");
  exit(1);
 }

 char str[]="Hello Kernel";
 int len=strlen(str);
 nbytes=write(fd,str,len);

 if(nbytes<0) 
 {
  perror("write");
 }	

 char buf[64];
 int maxlen=64;
 nbytes=read(fd,buf,maxlen);
 close(fd);
 }
