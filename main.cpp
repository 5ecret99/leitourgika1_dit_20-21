#include <stdio.h>
#include <stdlib.h>
#include <iostream>
#include <cstring>
#include <string>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <sys/types.h>
#include <sys/sem.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <semaphore.h>
#include "functions.cpp"
#define LEN 15//MAXIMUM CHAR COUNT FOR INPUT MESSAGES
using namespace std;
int main(int argc ,char **argv)
	{
	srand ((unsigned int)time(NULL));
	int prob=atoi(argv[0])*10+atoi(argv[1]);
	cout<<"probability: "<<prob<<endl;
	//int prob=60;
	//semaphores
	sembuf sem[9];
	int sem_id;
	semun argument;
	argument.val=1;
	key_t key;
	sem_id = semget(key, 9, IPC_CREAT|0600);
	if(sem_id==-1)
		{
        perror("shmget()");
        exit(EXIT_FAILURE);
    	}
	printf("semaphores id: %d\n",sem_id);
	for(int i=0;i<9;i++)
		{
		sem[i].sem_num=i;
		sem[i].sem_flg=0;
		sem[i].sem_op=-1;
		printf("semaphore %d with operation %d\n",i,sem[i].sem_op);
		}
 	semctl(sem_id,9,SETVAL, argument);

 	int shm_id[4];
 	char* str[4];
 	for(int i=0;i<4;i++)
 		{
 		if((shm_id[i] = shmget(IPC_PRIVATE, 5 , IPC_CREAT|0600)) == -1)
 			{
        	perror("shmget()");
        	exit(EXIT_FAILURE);
    		}
    	cout<<"char* id: " <<shm_id[i]<<endl;
    	if((str[i] = (char *)shmat(shm_id[i], 0, 0)) == (char *) -1)
    		{
       		perror("shmat()");
        	exit(EXIT_FAILURE);
   			}
   		}
   	int shmeid[2];
 	int* err[2];
 	int shmchid[2];
 	long long int* chk[2];
 	for(int i=0;i<2;i++)
 		{
 		if((shmeid[i] = shmget(IPC_PRIVATE, 5 , IPC_CREAT|0600)) == -1)
 			{
        	perror("shmget()");
        	exit(EXIT_FAILURE);
    		}
    	cout<<"integer error id: "<<shmeid[i]<<endl;
    	if((err[i] = (int *)shmat(shmeid[i], 0, 0)) == (int *) -1)
    		{
       		perror("shmat()");
        	exit(EXIT_FAILURE);
   			}
   		else
   			*err[i]=1;
   		if((shmchid[i] = shmget(IPC_PRIVATE, 5 , IPC_CREAT|0600)) == -1)
 			{
        	perror("shmget()");
        	exit(EXIT_FAILURE);
    		}
    	cout<<"long long int checksum id: "<<shmchid[i]<<endl;
    	if((chk[i] = (long long int *)shmat(shmchid[i], 0, 0)) == (long long int *) -1)
    		{
       		perror("shmat()");
        	exit(EXIT_FAILURE);
   			}
   		}

   	*err[0]=1;*err[1]=1;
   	//if  #DEFINE TERM long long int doesnt work change with this
   	//fgets(str[0], LEN, stdin);
   	//cout<<checksum(str[0])<<endl;
	int pid1=fork();
	int pid2=fork();
	int pid3=-1;
	if (pid1!=0&&pid2==0)
		pid3=fork();
	int fl=1;
	while(fl)
		{	
		//p1
		if (pid1!=0&&pid2!=0&&pid3==-1)
			{
			//cout<<"p1"<<endl;
			//cout<<"parent"<<endl<<pid1<<" "<<pid2<<" "<<pid3<<endl;
			cout<<"ENTER P1->P2 MESSAGE:"<<endl;
			fgets(str[0], LEN, stdin);
			fl=term(str[0]);
			//time for p2
			sem[0].sem_op=1;
			semop(sem_id,&sem[0],1);

			//time for enc1
			sem[1].sem_op=1;
			semop(sem_id,&sem[1],1);

			//wait for enc1
			sem[2].sem_op=-1;
			semop(sem_id,&sem[2],1);

			cout<<"MESSAGE FROM P2 TO P1: ";
			fl=term(str[0]);
			cout<<str[0]<<endl;
			}
		//p2
		else if (pid1!=0&&pid2==0&&pid3==0)
			{
			//cout<<"p2"<<endl;
			//cout<<"child_d"<<endl<<pid1<<" "<<pid2<<" "<<pid3<<endl;
			//wait for p1	
			sem[0].sem_op=-1;
			semop(sem_id,&sem[0],1);

			cout<<"ENTER P2->P1 MESSAGE:"<<endl;
			fgets(str[3], LEN, stdin);
			fl=term(str[3]);

			//time for enc2
			sem[7].sem_op=1;
			semop(sem_id,&sem[7],1);

			//wait for enc2
			sem[8].sem_op=-1;
			semop(sem_id,&sem[8],1);

			//if (checksum(str[3])==TERM)
				//fl=0;

			cout<<"MESSAGE FROM P1 TO P2: ";
			fl=term(str[3]);
			cout<<str[3]<<endl;
			}

		//enc1
		else if (pid1==0&&pid2!=0&&pid3==-1)
			{
			//cout<<"enc1"<<endl;
			//cout<<"child_a"<<endl<<pid1<<" "<<pid2<<" "<<pid3<<endl;

			//wait for p1
			sem[1].sem_op=-1;
			semop(sem_id,&sem[1],1);
			fl=term(str[0]);

			strcpy(str[1],str[0]);

			//time for chan
			sem[4].sem_op=1;
			semop(sem_id,&sem[4],1);
			sleep(2);
			

			//wait for chan
			sem[3].sem_op=-1;
			semop(sem_id,&sem[3],1);

			while(*chk[0]!=checksum(str[1]))
				{
				*err[0]=1;
				sem[3].sem_op=1;
				semop(sem_id,&sem[3],1);
				sleep(3);
				sem[3].sem_op=-1;
				semop(sem_id,&sem[3],1);
				}

			*err[0]=0;
			//if (checksum(str[1])==TERM)
				//fl=0;
			fl=term(str[1]);
			strcpy(str[0],str[1]);

			//time for chan
			sem[3].sem_op=1;
			semop(sem_id,&sem[3],1);

			//waiting for chan
			sem[4].sem_op=-1;
			semop(sem_id,&sem[4],1);
			

			//time for p1
			sem[2].sem_op=1;
			semop(sem_id,&sem[2],1);
			}


		//enc2
		else if (pid1==0&&pid2==0&&pid3==-1)
			{
			//cout<<"enc2"<<endl;
			//cout<<"child_c"<<endl<<pid1<<" "<<pid2<<" "<<pid3<<endl;
			//wait for p2
			sem[7].sem_op=-1;
			semop(sem_id,&sem[7],1);
			//if (checksum(str[3])==TERM)
				//fl=0;
			fl=term(str[3]);
			strcpy(str[2],str[3]);


			//time for chan
			sem[5].sem_op=1;
			semop(sem_id,&sem[5],1);
			sleep(2);

			//wait for chan
			sem[6].sem_op=-1;
			semop(sem_id,&sem[6],1);

			while(*chk[1]!=checksum(str[2]))
				{
				*err[1]=1;
				sem[6].sem_op=1;
				semop(sem_id,&sem[6],1);
				sleep(3);
				sem[6].sem_op=-1;
				semop(sem_id,&sem[6],1);
				}

			*err[1]=0;
			fl=term(str[2]);
			strcpy(str[3],str[2]);
			//if (checksum(str[2])==TERM)
				//fl=0;
			//time for chan
			sem[6].sem_op=1;
			semop(sem_id,&sem[6],1);

			//waiting for chan
			sem[5].sem_op=-1;
			semop(sem_id,&sem[5],1);

			//time for p2
			sem[8].sem_op=1;
			semop(sem_id,&sem[8],1);
			}

		//chan
		else if (pid1!=0&&pid2==0&& pid3!=0)
			{
			//cout<<"chan"<<endl;
			//cout<<"child_b"<<endl<<pid1<<" "<<pid2<<" "<<pid3<<endl;
			char*str1,*str2,*temp;
			
			//wait for enc2
			sem[5].sem_op=-1;
			semop(sem_id,&sem[5],1);
			//wait for enc1
			sem[4].sem_op=-1;
			semop(sem_id,&sem[4],1);
			*err[1]=1;*err[0]=1;
			str1=(char*)malloc(sizeof(str[1]));fl=term(str[1]);
			str2=(char*)malloc(sizeof(str[2]));fl=term(str[2]);
			temp=(char*)malloc(sizeof(temp));
			strcpy(str2,str[2]);
			strcpy(str1,str[1]);
			*chk[1]=checksum(str1);
			*chk[0]=checksum(str2);

			while(*err[1])
				{
				strcpy(str[2],str_change(str1,prob));
				//cout<< str[2] <<"is not the same as"<< str1<<endl;
				//strcpy(str[2],temp);
				//strcpy(str[2],str1);
				sem[6].sem_op=1;
				semop(sem_id,&sem[6],1);
				sleep(3);
				sem[6].sem_op=-1;
				semop(sem_id,&sem[6],1);
				}

			while(*err[0])
				{
				strcpy(str[1],str_change(str2,prob));
				//cout<< str[1] <<"is not the same as"<< str2<<endl;
				//strcpy(str[1],temp);
				//strcpy(str[1],str2);
				sem[3].sem_op=1;
				semop(sem_id,&sem[3],1);
				sleep(2);
				sem[3].sem_op=-1;
				semop(sem_id,&sem[3],1);
				}
			free(str1);free(str2);
			sem[4].sem_op=1;
			semop(sem_id,&sem[4],1);
			sem[5].sem_op=1;
			semop(sem_id,&sem[5],1);
			}
		fl=0;
		}
		semctl(sem_id, 9, IPC_RMID);
		for(int i=0;i<4;i++)
 			{
 			semctl(shm_id[i], 0, IPC_RMID);
 			}
 		for(int i=0;i<2;i++)
 			{
 			semctl(shmeid[i], 0, IPC_RMID);
 			semctl(shmchid[i], 0, IPC_RMID);
 			}
 

	}