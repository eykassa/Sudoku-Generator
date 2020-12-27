#include <pthread.h>
#include <stdio.h>
#include <string.h>
//#include <malloc.h>
#include <stdio.h>
#include <sys/shm.h>
#include <sys/stat.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>

typedef struct
{
	int row;
	int col;

}parameter;

char array[9][9];//max 9 by 9
int size=3;
char* data="123456789";
char* data2="abcdefghijklmnopqrstuvwxy";
int status=1;

void *checkBox(void *param); /*checks square box*/
void *checkRow(void *param); 
void *checkCol(void *param); 

void getInput(char* fileName){
	FILE *file;
	file=fopen(fileName,"r");
	int n;
	printf("Please enter the size of n for sudoku \n");
	fscanf(file,"%d",&n);
	printf("Size is \n",n);
	size=n;
	int i,j;
	char temp[3];
	for(i=0; i<(n*n);i++){
		for(j=0; j<(n*n); j++){
			fscanf(file,"%s",&temp);
			array[i][j]=temp[0];
		}
	}
	fclose(file);
}
void printArray(){
	printf("Array the was read in \n");
	int i,j;
	for(i=0; i<(size*size); i++){
		for(j=0; j<(size*size); j++){
			printf("%c ",array[i][j]);
		}
		printf("\n");
	}
	printf("====================\n");
}
void generate(){
	printf("Good choice , your sudoku is begin generated \n");
	char *word;
	if(size>3){
		word=data2;//if n>3 then we use letter instead of numbers
	}
	else{
		word=data;
	}
	int i,j;
	pthread_t tid2[3];
	pthread_attr_t attr2[3];
	for(i=0; i<(size*size); i++){

		for(j=0; j<(size*size); j++){
			status=1;
			int pos=0;
			int ran=(rand()%(size*size));
			array[i][j]=word[ran];//assign the current position a random letter or number

			parameter* p= malloc(sizeof(parameter));
			p->row=i;
			p->col=j;
			pthread_attr_init(&attr2[pos]);
			pthread_create(&tid2[pos],&attr2[pos],checkRow,(void*)p);
			pos+=1;
			//pthread_join(tid2[pos-1],NULL);

			pthread_attr_init(&attr2[pos]);
			pthread_create(&tid2[pos],&attr2[pos],checkCol,(void*)p);
			pos+=1;
			//pthread_join(tid2[pos-1],NULL);			
		
			parameter* p2= malloc(sizeof(parameter));
			p2->row=(((i+1)/size)*size);
			if((i+1)%size !=0){
				p2->row+=size;			
			}
			p2->col=(((j+1)/size)*size);
			if((j+1)%size !=0){
				p2->col+=size;
			}

			pthread_attr_init(&attr2[pos]);
			pthread_create(&tid2[pos],&attr2[pos],checkBox,(void*)p2);
			pos+=1;
			//pthread_join(tid2[pos-1],NULL);

			int k;
			for(k=0; k<3; k++){
				pthread_join(tid2[k],NULL);
			}
			if(!status){
				array[i][j]='0';
				j--;	
			}
		}
	}
	

	

}
int main(int argc, char *argv[])
{
 printf("Hello , Welcome to Essayas's sudoku game\n");
 printf("Please enter 1 to generate a sudoku or 0 to check your generated sudoku \n");
 int input;
 scanf("%d",&input);
 if(input){
	printf("Please enter n side size of the square sudoku \n");
	scanf("%d",&size);
	generate();
	printArray();
	return 0;
 }
 printf("Please enter the filename where your sudoku is saved \n");
 char* fileName[50];
 scanf("%s",fileName);
 getInput(fileName);
 printArray();
 status=1;
 int s=(size*size)*3;
 pthread_t tid[s];
 pthread_attr_t attr[s];
 int i;
 int j;
 int pos;
  pos=0;
 for(i=0; i<size*size; i++){
	for(j=0; j<size*size; j++){
		if(i==0){//check col
		           parameter* shared_memory=malloc(sizeof(parameter));
                	   shared_memory->row=i;
                	   shared_memory->col=j;

			   pthread_attr_init(&attr[pos]);
   			   pthread_create(&tid[pos],&attr[pos],checkCol,(void*)shared_memory);
			   pos++;
		}
		if(j==0){//check row
		           parameter* shared_memoryy=malloc(sizeof(parameter));
	                   shared_memoryy->row=i;
            		   shared_memoryy->col=j;

			   pthread_attr_init(&attr[pos]);
                           pthread_create(&tid[pos],&attr[pos],checkRow,(void*)shared_memoryy);
			   pos++;
		}	
		if(((i+1)%size)== 0 && ((j+1)%size) == 0){//if we have square check the square
		 	   parameter* shared_memoryyy=malloc(sizeof(parameter));
	                   shared_memoryyy->row=i;
                           shared_memoryyy->col=j;
			   pthread_attr_init(&attr[pos]);
                           pthread_create(&tid[pos],&attr[pos],checkBox,(void*)shared_memoryyy);
			   pos++;

		}
	}

 }
 for(i=0; i<((size*size)+size+size); i++){
	pthread_join(tid[i],NULL);
 }
 if(status==1)
	printf("The sudoku is correct \n");
 else
	printf("The is not correct \n");
 return 1;
}

void *checkRow(void * param){
	parameter* p=(parameter*)param;
	int row=p->row;
	int col=p->col;
	if(row>= (size*size) || col >=(size*size)){
		while(1){
			printf("row %d %d \n",row,col);
		}
		status&=0; 
		pthread_exit(0);
	}

	char ch;
	if(size<=3)//if n is less than 3 then we use numbers
		ch='1';
	else
		ch='a';//else we use letters 

	int i;
	int value[size*size];
	memset(value,0,sizeof(value));
	for(i=0; i<(size*size); i++){
		int pos= array[row][i] -ch;
		if(pos<0 || pos>=(size*size)){

		}
		else if(value[pos]==1){
			status&=0;
			pthread_exit(0);		
		}	
		else{
			value[pos]=1;
			
		}	
	}
	status&=1;
	pthread_exit(0);
	
}
void *checkCol(void * param){
	parameter* p=(parameter*)param;
	int row=p->row;
	int col=p->col;

	if(row>= (size*size) || col >=(size*size)){
		while(1){
                        printf("col %d %d \n",row,col);
                }
		status&=0; 
		pthread_exit(0);		
	}

	char c;
	if(size<=3)
		c='1';
	else
		c='a';

	int i;
	
	int value[size*size];
	memset(value,0,sizeof(value));
	for(i=0; i<(size*size); i++){
		int pos= array[i][col] -c;
		if(pos<0 || pos>=size*size){
	
		}
		else if(value[pos]==1){
			status&=0;
			pthread_exit(0);		
		}	
		else{
			value[pos]=1;
			
		}	
	}
	status&=1;
	pthread_exit(0);
	
}
void *checkBox(void * param){
	parameter* p=(parameter*)param;
	int srow=(p->row-size)+1;
	int scol=(p->col-size)+1;

        if(srow<0 || scol < 0){
		status&=0;
		pthread_exit(0);
	}		
	char c;
	if(size<=3)
		c='1';
	else
		c='a';

	int i;
        int j;	
	int value[size*size];
	memset(value,0,sizeof(value));
	for(j=srow; j< srow+size; j++){
	for(i=scol; i<(scol+size); i++){
		int pos= array[j][i] -c;
		if(pos >=(size*size) || pos<0 ){

		}
		else if(value[pos]==1){
			status&=0;
			pthread_exit(0);		
		}	
		else{
			value[pos]=1;
			
		}	
	}
	}
	status&=1;
	pthread_exit(0);
	
}
