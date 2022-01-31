#include <stdio.h>
#include <stdlib.h>
#include <iostream>
#include <cstring>
#include <string>
#include <sys/types.h>
#include <sys/sem.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/ipc.h>
#include <string.h>
#define TERM 402206974//CHECKSUM FOR TERM
//#include "class.h"

using namespace std;

long long int checksum(char* str)
	{
	long long num =13;
	int x=sizeof(str)/sizeof(char);
	for(int i=0 ;i<x;i++)
		{
		num*=17;
		num+=str[i];
		num*=31;
		num%=1000000011;
		}
	return num;
	}

char* str_change(char* str,int prob)
	{
	
	char* str_cpy=(char*)malloc(sizeof(str));
	strcpy(str_cpy,str);
	srand ((unsigned int)time(NULL));
	if(rand()%101>prob)
		return str_cpy;
	else
		{
		int x=sizeof(str)/sizeof(char);
		
		int char_repl=rand()%x;
		
		for (int i = 0; i < char_repl; i++)
			{
			str_cpy[rand()%(x-1)]='a'+ (rand()%26);
			}
		return  str_cpy;
		}
	}
int term(char* str)
	{
	if(checksum(str)==TERM)
		return 0;
	return 1;
	}
union semun
	{
	int val;
	struct semid_ds *buf;
	ushort *array;
	};
