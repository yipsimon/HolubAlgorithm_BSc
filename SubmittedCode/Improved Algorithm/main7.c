#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <string.h>
#include <math.h>
#include <time.h>
#include <unistd.h>
#include "mainfunctions.h"

#define BOOL char
#define FALSE 0
#define TRUE 1

int main(){
	
	clock_t start,end;
	double cpu_time_used;

	int selector;
	char str[100]; 	
	int len;
	int temp;
	
	FILE *fp;
	FILE *fpp;
	char file_name[100];
	int print1;
	int save;
	int savetype;


	int file_num;
	char filem[20][33];
	char ext[] = ".txt";
	char savefile_name[100];
	char sbackup_name[100];
	
	
	int N1;
	int mode;
	int printsty;
	int p[28];
	int q[28];

	printf("\n%35s\n", "Decision Program based on Holub's Algorithm");
	printf("\n");
	while (1){
		unsigned long prim = 0;
		unsigned long imprim = 0;
		//unsigned long long totalprim = 0;
		//unsigned long long totalcount = 0;
		printf("%s\n", "Menu");
		printf("%d : %s\n",1, "Solve only one word");
		printf("%d : %s\n",2, "Insert a file");
		printf("%d : %s\n",3, "Insert multiple files");
		printf("%d : %s\n",4, "Generate set partition of size n and run algorithm");
		printf("%d : %s\n",5, "End Program");
		
		scanf("%d", &selector);
		switch(selector) {
			case 1:
				printf( "Enter a word : ");
				scanf("%s", str);
				len = strlen(str);
				start = clock();
				temp = holubAlg(str,len,-1);
				end = clock();
				cpu_time_used = ((double) (end - start))/ CLOCKS_PER_SEC;
				if(temp == 0){
					printf("This word is morphically primitive\n");
				}
				else{
					printf("This word is morphically imprimitive\n");
				}
				printf("\n %f seconds to execute\n",cpu_time_used);
				
				printf( "Enter 1 restart the program or 0 to Exit:");
				int selector2;
				scanf("%d", &selector2);
				if (selector2 == 0){
					exit(0);
				}
				else{
					fprintf(stderr, "Restarting Program...\n");
				}
			break;
		
			case 2:
				printf("Enter the name of file you wish to load (with the extension .txt) \n");
				scanf("%s", file_name);
				
				printf("Do you wish to display the results: Yes = 1, No = 0 \n");
				scanf("%d",&print1);
				
				printf("Do you wish to save the contents: Yes = 1, No = 0 \n");
				scanf("%d", &save);
				if(save == 1){
					printf("Enter filename to be created: ");
					scanf("%s", savefile_name);
					strcpy(sbackup_name, savefile_name);
					strcat(savefile_name,ext);	
					printf("Do you wish to save: \n(1) All primitive words   (2) All imprimitive words   (3) Both \n");
					scanf("%d", &savetype);
					if(savetype == 3){
						printf("Do you want to label them? 	Yes = 1 No = 0 \n");
						int tempb;
						scanf("%d", &tempb);
						savetype = savetype + tempb;
					}
				}
				
				fp = fopen(file_name,"r");
				
				if( fp == NULL )
				{
					perror("Error while opening the file.\n");
					exit(EXIT_FAILURE);
				}
				char tempword[500000];
				start = clock();
				while(fscanf(fp, "%s", tempword) == 1 ){
					int length = strlen(tempword);
					temp = holubAlg(tempword, length, print1);
					
					if(temp == 0){
						prim = prim + 1;
					}
					else{
						imprim = imprim + 1;
					}
					unsigned long temptotal = prim+imprim;
					if( temptotal % 20000 == 0){
						printf("passed %d\n",temptotal);
					}
					if(save == 1){
						fpp = fopen(savefile_name,"a");	
						if(temp == 0){
							if(savetype == 1 || savetype == 3){
								fputs(tempword,fpp);
								fputs("\n",fpp);
							}
							else if(savetype == 4){
								fputs(tempword,fpp);
								fputs(" : primitive ",fpp);
								fputs("\n",fpp);
							}
						}
						else{
							if(savetype == 2 || savetype == 3){
								fputs(tempword,fpp);
								fputs("\n",fpp);
							}
							else if(savetype == 4){
								fputs(tempword,fpp);
								fputs(" : imprimitive",fpp);
								fputs("\n",fpp);
							}
						}
						fclose(fpp);
					}
				}
				fclose(fp);
				end = clock();
				cpu_time_used = ((double) (end - start))/ CLOCKS_PER_SEC;
				
					printf( "\n Total words: %d", prim+imprim);
					printf( "\n Number of morphically primitive words: %d", prim);
					printf( "\n Number of morphically imprimitive words: %d \n", imprim);
					printf("\n %f seconds to execute\n",cpu_time_used);
				printf( "Enter 1 restart the program or 0 to Exit:");
				int selector3;
				scanf("%d", &selector3);
				if (selector3 == 0){
					exit(0);
				}				
				else{
					fprintf(stderr, "Restarting Program...\n");
				}
			break; 
		
			case 3:
				printf("Enter the number of file you wish to load ~ max 18 (with the extension .txt) \n");
				scanf("%d", &file_num);
				for (int i = 0; i < file_num; i++){
					printf("Enter file %d (with the extension .txt) \n",i+1);
					scanf("%s",filem[i]);
				}
								
				for (int z = 0; z < file_num; z++){
					fp = fopen(filem[z],"r");
						
					if( fp == NULL ){
						perror("Error while opening the file.\n");
						exit(EXIT_FAILURE);
					}
					start = clock(); 
					char tempword[500000];
					while(fscanf(fp, "%s", tempword) == 1 ){
						int length = strlen(tempword);
						temp = holubAlg(tempword, length,0);
						if(temp == 0){
							prim = prim + 1;
						}
						else{
							imprim = imprim + 1;
						}
						
						unsigned long temptotal2 = prim+imprim;
						if( temptotal2 % 20000 == 0){
							printf("passed %d\n",temptotal2);
						}
					}	
					fclose(fp);
				}
				end = clock();
				cpu_time_used = ((double) (end - start))/ CLOCKS_PER_SEC;
				printf( "\n Total words: %d", prim+imprim);
				printf( "\n Number of morphically primitive words: %d", prim);
				printf( "\n Number of morphically imprimitive words: %d \n", imprim);
				printf("\n %f seconds to execute\n",cpu_time_used);
				printf( "Enter 1 restart the program or 0 to Exit:");
				int selector4;
				scanf("%d", &selector4);
				if (selector4 == 0){
					exit(0);
				}				
				else{
					fprintf(stderr, "Restarting Program...\n");
				}
			break;	
			
			case 4:
				printf( "Enter length of word: ");
				scanf("%d", &N1);
				printf( "Select (1) = Set generator 1 or (2) = Set generator 2 \n");
				scanf("%d", &mode);
				if( N1 < 10){
					printf("Do you wish to display the results: Yes = 1, No = 0 \n");
					scanf("%d",&print1);
					printf("Do you wish to save the contents: Yes = 1, No = 0 \n");
					scanf("%d", &save);
				}
				else{
					save = 0;
					print1 = 0;
				}
				start = clock();
				multithread(mode,N1,save,print1);
				end = clock();
				cpu_time_used = ((double) (end - start))/ CLOCKS_PER_SEC;
				printf("\n %f seconds to execute\n",cpu_time_used);
				printf( "Enter 1 restart the program or 0 to Exit:");
				int selector5;
				scanf("%d", &selector5);
				if (selector5 == 0){
					exit(0);
				}
				else{
					fprintf(stderr, "Restarting Program...\n");
				}
			break;
						
			case 5:
				exit(0);
			break;
			
			default:
				fprintf(stderr, "Invalid Selection, Restarting \n");
			break; 
		}
	
	}


}
//gcc main.c -c
//gcc main.o main2.c -o main2.exe
//gcc helloWorld.c -o helloWorld.exe
//setx PATH "%PATH%;C:\MinGW\bin;"

