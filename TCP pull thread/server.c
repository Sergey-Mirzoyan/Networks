#include<stdio.h>
#include<stdlib.h>
#include<sys/types.h>
#include<sys/socket.h>
#include<netinet/in.h>
#include<arpa/inet.h>
#include<unistd.h>
#include<pthread.h>
#include<string.h>
#include "timer.h"

#define STR_LEN 1000
#define THREAD_COUNT 1000

int num_str; 
char** theArray;
pthread_mutex_t mutex;
double total = 0;
int counter = 0;

void initArray();
void *ServerEcho(void *args);

 
int main(int argc, char* argv[])
{
	if(argc != 3) {
		printf("Error: incorrect number of arguments, please try again\n");
		return 0;
	}

	num_str = atoi(argv[2]); // кол-во символов
	initArray();

	struct sockaddr_in sock_var;
	int serverFileDescriptor=socket(AF_INET,SOCK_STREAM,0);
	int clientFileDescriptor;
	int i;

	pthread_t* t;

	sock_var.sin_addr.s_addr=inet_addr("127.0.0.1");
	sock_var.sin_port=atoi(argv[1]); // порт
	sock_var.sin_family=AF_INET;
	if(bind(serverFileDescriptor,(struct sockaddr*)&sock_var,sizeof(sock_var))>=0)
	{
		printf("Socket has been created!\n");
		listen(serverFileDescriptor,2000);
		pthread_mutex_init(&mutex, NULL); 
		while(1)       
		{
			t = malloc(THREAD_COUNT*sizeof(pthread_t));
			for(i=0;i<THREAD_COUNT;i++)      //до 1000 потоков
			{
				clientFileDescriptor=accept(serverFileDescriptor,NULL,NULL);
				pthread_create(&t[i],NULL,ServerEcho,(void *)(intptr_t)clientFileDescriptor);
			}
			for(i=0; i<THREAD_COUNT;i++)
				pthread_join(t[i],NULL);

			free(t); 
		}
		close(serverFileDescriptor);
	}
	else
		printf("\nsocket creation failed\n");

	pthread_mutex_destroy(&mutex);
	return 0;
}

void *ServerEcho(void *args) {
	int clientFileDescriptor=(int)args;
	int converted_pos;
	int pos;

	// чтение или запись клиентом
	char operation[STR_LEN];
	char read_buf[] = "read";
	char write_buf[] = "write";
	double start, finish, elapsed ;

	// чтение
	read(clientFileDescriptor,operation,STR_LEN);

	GET_TIME(start);
	pthread_mutex_lock(&mutex); //critical section

	if(strcmp(operation, read_buf) == 0) {
		//printf("Client %d wants to read...\n", clientFileDescriptor);
		
		// позиция начала чтения
		read(clientFileDescriptor, &converted_pos, sizeof(converted_pos));
		pos = ntohl(converted_pos);
		//printf("Client %d sent pos is: %d\n", clientFileDescriptor, pos);

		write(clientFileDescriptor,theArray[pos],STR_LEN);
	}
	//письмо
	else if(strcmp(operation, write_buf) == 0) {
		
		read(clientFileDescriptor, &converted_pos, sizeof(converted_pos));
		pos = ntohl(converted_pos);
		
		sprintf(theArray[pos], "String %d has been modified by a write request", pos);
		write(clientFileDescriptor,theArray[pos],STR_LEN);
	}
	pthread_mutex_unlock(&mutex); // unlock to other threads
	
	// append time to total & print when 1000 is reached
	GET_TIME(finish); 
	elapsed = finish - start;
	total = total + elapsed;
	counter++;
	if (counter == 1000) {
		printf("%e\n", total);
		counter = 0;
		total = 0;
	}

	close(clientFileDescriptor);
	return NULL;
}

void initArray() {
	// initialize the array with number of strings
	theArray = malloc(num_str*sizeof(char[STR_LEN]));

	int i;
	for(i = 0; i < num_str; i++) {
		theArray[i] = malloc(STR_LEN*sizeof(char));
		sprintf(theArray[i], "theArray[%d]: initial value", i);
	}
}
