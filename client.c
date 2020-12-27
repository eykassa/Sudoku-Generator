#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <fcntl.h>
#include <sys/shm.h>
#include <sys/stat.h>
#include <sys/mman.h>
void clientRun(char* a, int* b);
int main()
{
      sleep(3);
      /* Shared Memory size
       *
       */
	const int SIZE = 8*4096;
      /* Name of the shared memory segment
       *
       */
       const char *name = "SHMSEQ";
       const char *namee = "SHMSEQQ";
       /* Shared memory file descriptor */
	int shm_fd;
	int shm_fd2;
      /*pointer used for access to shared memory segment */
	void *ptr;
	void *ptr2;
	int i;

	/* open the shared memory segment */
	shm_fd = shm_open(name, O_CREAT | O_RDWR, 0666);
	shm_fd2 = shm_open(namee, O_CREAT | O_RDWR, 0666);
	if (shm_fd == -1|| shm_fd2==-1) {
		printf("shared memory failed\n");
		exit(-1);
	}

	/* now map the shared memory segment in the address space of the process */
	ptr = mmap(0,SIZE,  PROT_READ | PROT_WRITE, MAP_SHARED, shm_fd, 0);
	ptr2 = mmap(0,SIZE,  PROT_READ | PROT_WRITE, MAP_SHARED, shm_fd2, 0);
	if (ptr == MAP_FAILED|| ptr2== MAP_FAILED) {
		printf("Map failed\n");
		exit(-1);
	}
	
	int* p=(int*) ptr;
	if(p[0]!=1){
		p[0]=1;
		printf("This client will be the master client \n");
		clientRun((char*)ptr2,p);
	}
	else{
		printf("There is a client already doing computation \n");
		printf("Stay connect to receive the final result \n");
		while(p[0]!=2){
			printf("Please be patient \n");
			sleep(5);
		}
		printf("The result was %d\n",p[1]);	
	}
	

	/* remove the shared memory segment */
	if (shm_unlink(name) == -1|| shm_unlink(namee)==-1) {
		printf("Error removing %s\n",name);
		exit(-1);
	}

	return 0;
}

char getLastChar(char* s){ 
        return s[0];
}





void writeToServer(char* fd,char* str){
	char* t=(char*) malloc(sizeof(char)*1024);
	strcat(t,"#");
	strcat(t,str);
	strcat(t,"\n");
	sprintf(fd,"%s",t);
}
void readFromServer(char* fd,int* fd2,char* str){
	while(getLastChar(fd)!='$'&&fd2[0]!=2){
		//printf("clinet waiting read \n");
		//printf("%s\n",fd);
		sleep(1);
	}
	if(fd2[0]==2)
		return;
	fd++;
	//printf("check pre %s\n",str);
	sscanf(fd," %[^\n]%*c",str);	
	//printf("cliet received : %s",str);
}

void clientRun(char* fd,int* fd2){
	printf("Client entered \n");
        char* str=(char*) malloc(sizeof(char)*1024);
        char* temp=(char*) malloc(sizeof(char)*1024);
        while(temp!="$Done" && fd2[0]!=2){
        //read(fd,temp,sizeof(char)*1024);
	readFromServer(fd,fd2,temp);
	//sscanf(fd," %[^\n]%*c",temp);	
        if(fd2[0]==2||strcmp(temp,"$Done")==0){
		break;
        }

        printf("received %s",temp);
	scanf(" %[^\n]%*c", str);

	writeToServer(fd,str);
        }
	//readFromServer(fd,fd2,temp);
	printf("The final result is %d\n", fd2[1]);
	printf("Thank you for being such a good master client \n");

}

