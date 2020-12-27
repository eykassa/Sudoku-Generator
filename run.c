#include <pthread.h>
#include <stdio.h>
#include <string.h>
#include <malloc.h>
#include <stdio.h>
#include <sys/shm.h>
#include <sys/stat.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include<fcntl.h>
#include<sys/mman.h>





int sum; /*shared data*/
int* dataBase;
int* percentage;
int size;
int* result;

void *compute(void *param); /*the second hread*/
void *calculate(void *param);
void *display(void *param);
void readFromClient(void *param,void *param2); /*the first thread*/
void writeToClient(char *param,char *param2); /*the first thread*/
char getLastChar(char* s){
        return s[0];
}

void innit(){
	dataBase=malloc(3*sizeof(int));
	size=3;
	result=malloc(3*sizeof(int));
	percentage=malloc(3*sizeof(int));
	percentage[0]=20;
	percentage[1]=40;
	percentage[2]=40;
}
int main(int argc, char *argv[])
{
 innit();



	const int SIZE = 8*4096;
        int interval=1024;
      /* Name of the shared memory segment
       *
       */
        const char *name = "SHMSEQ";
        const char *namee = "SHMSEQQ";
       /* Shared memory file descriptor */
        int shm_fd;
        int shm_fd2;
       /*pointers used for access to shared memory segment */

        void *ptr,*startptr;
        void *ptr2,*startptr2;

        char str[128];
        if (argc>1)
        interval  = atoi(argv[1]);

        /* create the shared memory segment */
        shm_fd = shm_open(name, O_CREAT | O_RDWR, 0666);
        shm_fd2 = shm_open(namee, O_CREAT | O_RDWR, 0666);


        /* configure the size of the shared memory segment */
        ftruncate(shm_fd,SIZE);
        ftruncate(shm_fd2,SIZE);


        /* now map the shared memory segment in the address space of the process */
        startptr=ptr = mmap(0,SIZE, PROT_READ | PROT_WRITE, MAP_SHARED, shm_fd, 0);
        startptr2=ptr2 = mmap(0,SIZE, PROT_READ | PROT_WRITE, MAP_SHARED, shm_fd2, 0);
        if (ptr == MAP_FAILED||ptr2==MAP_FAILED) {
                printf("Map failed\n");
                exit(-1);
        }

	int * shared_memory=(int*)ptr;
	shared_memory[0]=0;
	sleep(2);
	char * result=(char*)ptr2;
	while(shared_memory[0]!=1){
		printf("Server waiting for connect \n");
	}
	printf("Connected \n");
	readFromClient(ptr2,shared_memory);	


	sleep(5); 
 	pthread_t tid; /* thread id */
 	pthread_t tid2; /* thread id 2*/
 	pthread_t tid3;
 	pthread_attr_t attr, attr2,attr3; /* thread attributes */


   	pthread_attr_init(&attr2);
   	pthread_create(&tid2,&attr2,compute,shared_memory);
   	pthread_join(tid2,NULL);
	
   	pthread_attr_init(&attr3);
   	pthread_create(&tid3,&attr3,display,shared_memory);
   	pthread_join(tid3,NULL);
	shared_memory[0]=2; 
   return 0;
}
void writeToClient(char* fd,char* str){
        //write(fd,str,sizeof(char)*1024);
        char* t=(char*) malloc(sizeof(char)*1024);
        strcat(t,"$");
        strcat(t,str);
        strcat(t,"\n");
        sprintf(fd,"%s",t);
        printf("Sent %s\n",fd);

}
int readd(char* fd){
        while(getLastChar(fd)!='#'){
                //printf("server waiting \n");
                sleep(1);
        }
        fd++;

        char* str=(char*)malloc(sizeof(char)*1024);
        //read(fd,str,sizeof(char)*1024);
        sscanf(fd," %[^\n]%*c",str);
        return atoi(str);
}
void readFromClient(void* p, void* p2){
	int* data=(int*)p2;
	int pos=1;

	char* dataBase=(char*)p;
	char* str=malloc(sizeof(char)*1024);
	str="Pleas enter your assignment grade ";

	writeToClient(dataBase,str);
	int num;
	num=readd(dataBase);
	data[pos++]=num;
	
	str="Please enter your midterm grade ";
	writeToClient(dataBase,str);
	num=readd(dataBase);
	data[pos++]=num;


	str="Please enter your ideal grade ";
	writeToClient(dataBase,str);
	data[pos++]=num;


}
void *compute(void* param){
	int* dataBase=(int*)param;
	pthread_attr_t array[4];
	pthread_t array2[4];
	int array3[4]={-1,1,2,3};
	//array3[2]=&(*dataBase);
	int *pon[2];
	pon[1]=dataBase;
	int i;
	
	for(i=1; i<=size; i++){
		   pthread_attr_init(&array[i]);
		   pon[0]=&array3[i];
		   //printf("size  %d\n",i);
		   pthread_create(&array2[i],&array[i],calculate,(void*)pon);
		   pthread_join(array2[i],NULL);
	}
	pthread_exit(0);

	pthread_exit(0);
	//printf("Done eee \n");
	/* or you can do this to speed up the process 
	for(i=0; i<size; i++){
		pthread_join(array2[i],NULL);
	}
	*/

}
void *calculate(void *param){
	double percentage[4]={0,0.20,0.40,0.40};
	int* p=(int *)param;
	int pos=*((int *)p[0]);
	int* dataBase=(int *)p[1];
	//int pos=(p[0]);
	//printf("pos %d\n",pos);
	dataBase[pos]= dataBase[pos]*percentage[pos];
	pthread_exit(0);
}
void *display(void * param){
	int* dataBase=(int*)param;
	int final=0;
	int i=0;
	for(i =1; i<=size; i++){
		printf("%d\n",dataBase[i]);
		final+=dataBase[i];
	}
	dataBase[1]=final;
 	printf("The final result is %d\n",final);
	
}

