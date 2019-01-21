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

FILE *fpp2;
char ext2[] = ".txt";
char savefile_name2[100];
char sbackup_name2[100];

void addnewletter(struct data *info, char word, int location, int position){
	info[location].letter = word;
	info[location].numoccur = 1;
	info[location].positions = (int*)malloc(sizeof(int));	//set only one int size array, will adjast later
	info[location].positions[0] = position;
	info[location].inE = -1;
}

int sortset (int set[], int size){
	for (int i = 0; i < size-1; i++){
		int min = i;
		for (int j = i+1; j < size; j++){
			if(set[min] == set[j]){
				size = size - 1;
				for (int k = j; k < size; k++){
					set[k] = set[k+1];
				}
			}
			if(set[min] > set[j]){
				min = j;
			}
		}
		int temp = set[i];
		set[i] = set[min];
		set[min] = temp;
	}
	return size;
}

int holubAlg(char * s, int l, int p){
	int j;
	int print = p;
	char word[l];
	strcpy(word,s);
	int lenw = strlen(word);	//Size of the word
	struct data *letterdata = malloc(2*sizeof(struct data));  //dynammic data storing information about letters
	int primitive;	//What we are returning 0 = primitive and 1 = imprimitive
	int uniq = 1;	//initial letter count = 1 the initial one we put in
	
	//Store information about the letters of this word.
	addnewletter(letterdata, word[0], 0, 0);	//add initial letter, in array 0, position 0
		
	for(int i = 1; i < lenw; i++) {	
		for(int j = 0; j < uniq; j++){ 
			if(letterdata[j].letter == word[i]){	//If the letter exist in the struct data.
				letterdata[j].numoccur++;			//Increase num of occurence 
				int temp = letterdata[j].numoccur;	//Get new num of occurence
				letterdata[j].positions = realloc(letterdata[j].positions, temp * sizeof(int));		//Increase the memory size for position array
				letterdata[j].positions[temp-1] = i;	//Store position of the word (i) into the next position in the array (temp-1).
				break;
			}
			else if( j == uniq-1){
				uniq++;		//Increase the num of unqiue letter
				letterdata = realloc(letterdata, uniq * sizeof(struct data) );		//Increase the memory for the whole struct data.
				addnewletter(letterdata, word[i], j+1, i);		//Add the new letter to the struct data, j+1 = uniq anyway which is its new location in the struct array, i represent position of the word.
				break;
			}
		}
	}
		
	//Find neigbourhood of all letters of the word.
	BOOL leftfind = FALSE;	
	BOOL rightfind = FALSE;
	int getleftlimit = 1;	//Initial left limit (going to subtruct by 1 later when storing it in the struct array)
	int getrightlimit = 1;	//Initial right limit (going to subtruct by 1 later when storing it in the struct array)
	
	int *getposition; //Pointer making it easier to get the array of positions for one particular letter later.
	int getnumocc;	//Num of occurence of the letter
		
	for(int j = 0; j < uniq; j++){ 	//loop unique words otherword the object array
		getposition = letterdata[j].positions;
		getnumocc = letterdata[j].numoccur;
		if(getnumocc == 1){		//If there is only one occurrence of a letter, it is imprimitive and we can store it's neighbourhood as:
			letterdata[j].leftlimit = getposition[0];	//Left limit is the start position 0, from the current letter position - its own position = 0
			letterdata[j].rightlimit = lenw-getposition[0]-1;	//Right limit is the end position of the word, -1 because lenw is one value more.
		}
		else{
			while (leftfind == FALSE){	//Finding the correct left neighbourhood
				for (int k = 0; k < getnumocc-1; k++){ 			//We are checking the current and next position of the current letter.
					int s = (getposition[k]-getleftlimit); 		//Neigbourhood of first occurrence of letter
					int t = (getposition[k+1]-(getleftlimit)); 	//Neigbourhood of second occurrence of letter
					if(s < 0){ 	//If the start letter went beyond the beginning of the word
						leftfind = TRUE; 
						break;
					}
					if(word[s] != word[t]){ //When letters the between the two neigbourhood are not the same
						leftfind = TRUE;
						break;
					}
				}
				if(leftfind == FALSE){ 	//Letters between the two neigbourhood are the same
					getleftlimit++;		//Increase the size of left neighbourhood, checking the next letter.
				}
			}
			while (rightfind == FALSE){	//Finding the correct right neighbourhood
				for (int m = getnumocc-1; m > 0; --m){ 	//Searching from the right, from the end to the start between two neighbourhood
					int u = (getposition[m]+getrightlimit);		//Neighbourhood of last occurrence of letter
					int v = (getposition[m-1]+getrightlimit);	//Neighbourhood of second last occurrence of the letter
					if(u == lenw){	//If the last letter went beyond the length of the word
						rightfind = TRUE;
						break;
					}
					if(word[u] != word[v]){ //Not the same word.
						rightfind = TRUE;
						break;
					}
				}
				if(rightfind == FALSE){ //When letters are the same
					getrightlimit++;	//Increase the size of right neighbourhood, check the next letter
				}
			}
			letterdata[j].leftlimit = getleftlimit-1;		//Add the neighbourhood into the struct array,
			letterdata[j].rightlimit = getrightlimit-1;			//-1 because the while loop end while it was checking the next neighbourhood
			getleftlimit = 1;	//Reset limits
			getrightlimit = 1;
			leftfind = FALSE;	//Reset booleans
			rightfind = FALSE;	
		}
	}
	//Data Collection of the word is complete

	//Start of Holob's algorithm
	struct LRdata *newsetdata = malloc((lenw+1)*sizeof(struct LRdata));
	for (int i = 0; i < lenw+1; i++){
		newsetdata[i].L = 0;
		newsetdata[i].R = 0;
	}
		
	int Lset[lenw+1];	//Lset with twice the size of the word (just in case)
	int Rset[lenw+1];	//Rset with twice the size of the word (just in case)
	int Lsize = 2;		//Since the array is not filled up, we create a limiter for both array.
	int Rsize = 2;

	struct Estr *Eset = malloc((lenw+1)*sizeof(struct Estr));//Eset store the letter, position in struct array and it's final morphicfactorisation (datastart, dataend)
	int Esize = 0;				//Limiter for Eset.
	int Lpoint = 0;		//Lpoint keep tracks on the position of the Lset
	int Rpoint = 0;		//Rpoint keep tracks on the position of the Rset

	//Condition a
	Lset[0] = 0;
	Rset[0] = 0;
	Lset[1] = lenw;
	Rset[1] = lenw;
	newsetdata[0].L = 1;
	newsetdata[0].R = 1;
	newsetdata[lenw].L = 1;
	newsetdata[lenw].R = 1;
	
	int minocc = lenw+1;
	char minchar;
	int ctempocc = lenw+1;
	char ctempchar;
	int ctemploc = lenw+1;
	int loc;
	int intervalfind = lenw;

	//Original method, find interval between L,R that doesn't contain a letter in E, increase by L.
	BOOL Efind = FALSE;
	BOOL Letterfind = FALSE;
	
	while(1){ //<- Main Algorithm
		intervalfind = 0; //Reseting the indicator for finding an interval without an expanding letter
		minocc = lenw+1;	//Reset the minimum occurrence of letter to highest.
		ctempocc = lenw+1;
		//Reset the pointers.
		Lpoint = 0;	
		Rpoint = 0;	
				
		while(Lpoint < Lsize-1){	//Lsize-1 becuase Lpoint starts at 0. There's no need to check the last L of the set 
			if(Lset[Lpoint] >= Rset[Rpoint]){	//If L > R, we need to increase R so that R > L.
				Rpoint = Rpoint + 1;
			}
			else {
				int startpoint = Lset[Lpoint];		//Startpoint of the interval in the word.
				int intervalsize = Rset[Rpoint];	//Endpoint of the interval in the word.
				for (int i = startpoint; i < intervalsize; i++){	//Going through each letter in the interval checking for an existing expanding letter.
					char tempchar = word[i];	//The letter is not an expanding letter.
					int temploc = 0;			
					for(int k = 0; k < uniq; k++){
						if( tempchar == letterdata[k].letter ){
							temploc = k;	//Find the location of the letter in the struct array
							break;
						}	
					}
					int tempocc = letterdata[temploc].numoccur;		//Get the num of occurence for that letter
					if( ctempocc > tempocc){							//Compare it to the previous minimum occurrence
						ctempocc = tempocc;							//Update the new minimum occuring letter in this interval
						ctempchar = tempchar;
						ctemploc = temploc;
					}
				}
		
				if (Esize == 0){
					minocc = ctempocc;							//Update the new minimum occuring letter in this interval
					minchar = ctempchar;
					loc = ctemploc;
					intervalfind = intervalfind + 1;	
					break;
				}
				else{
					for(int j = 0; j < Esize; j++){ 				
						if(ctempchar == Eset[j].letter){				//Check if the letter exist in the expanding set
							ctempocc = lenw+1;
							break;
						}
						else if ( j == Esize-1){
							if( minocc > ctempocc){							//Compare it to the previous minimum occurrence*/
								minocc = ctempocc;							//Update the new minimum occuring letter in this interval
								minchar = ctempchar;
								loc = ctemploc;
								//printf("ctempchar: %c, ctempocc: %d\n",minchar,minocc);
								intervalfind = intervalfind + 1;
								break;
							}
							else{
								 ctempocc = lenw+1;
							}
						}
					}
				}
							
				//Holob's methods
				if(Rpoint == Rsize-1){
					//printf("Reached the end of Rpoint \n");
					Rpoint = 0;
					Lpoint = Lpoint + 1;
					//printf("Add Lpoint to search for a new interval \n");
				}
				else{
					Rpoint = Rpoint + 1;
					//printf("Add a Rpoint to the increase interval size \n");
				}
				
				//Another method
				/*if(Lset[Lpoint+1] > Rset[Rpoint]){
					Rpoint = Rpoint + 1;
					//printf("2 Increase Rpoint %d \n",Rpoint);
					//printf("2 Increase Rset[Rpoint] %d\n",Rset[Rpoint]);
				}
				else{
					Lpoint = Lpoint + 1;
					//printf("2 Increase Lpoint %d \n",Lpoint);
					//printf("2 Increase Lset[Lpoint] %d\n",Lset[Lpoint]);
				}*/
			}
		}
		

		
		if(intervalfind == 0){	//There no more interval to be found, the algorithm is finished.
			primitive = 1;
			break;
		}
		
		//Add the new expanding letter to the Eset.
		Eset[Esize].letter = minchar;
		Eset[Esize].datapos = loc;
		Eset[Esize].datastart = -1;
		Eset[Esize].dataend = -1;
		letterdata[loc].inE = Esize;
		Esize = Esize + 1;		//Increase the number of expanding letter by one.
		
		getposition = letterdata[loc].positions; 
		int limit = letterdata[loc].numoccur;		//The total number of the occurrence for this expanding letter which is used as a limit for looping.
		int leftneigh = letterdata[loc].leftlimit;	//The neighbourhoods for that expanding letter
		int righneigh = letterdata[loc].rightlimit;
		
		//Condition b
		for (int i = 0; i < limit; i++){
			newsetdata[getposition[i]].L = 1;
			newsetdata[getposition[i]+1].R = 1;
		}
				
		//Condition c
		for (int j = 0; j < limit; j++){
			newsetdata[getposition[j]+righneigh+1].L = 1;
			newsetdata[getposition[j]-leftneigh].R = 1;
		}
	
		//Condition d: Synchronisation of Neighbourhood
		BOOL repeat = FALSE;

		for (int c = Esize; c > 0; --c){	//Start from the end, which is the latest expanding letter.
			int loc2 = Eset[c-1].datapos;	//Get the position of the expanding letter in the struct array.
			getposition = letterdata[loc2].positions;	//Get the list of the position of that expanding letter in the word
			leftneigh = letterdata[loc2].leftlimit;		//Get the neighbourhoods.
			righneigh = letterdata[loc2].rightlimit;
			int limit2 = letterdata[loc2].numoccur;		//Get the number of occurrence of the expanding letter in the word.
			int lengthneigh = 1 + leftneigh + righneigh + 1;	//Length of the expanding letter in the word (leftneigh+righneigh), add 1 to include the expanding letter, 1 to make it into the limit (the R is one more)
			int Larray[lengthneigh*3]; 		//Templete for the Ls for the expanding interval
			int Rarray[lengthneigh*3];		//Templete for the Rs for the expanding interval
			int Larraypoint = 0;			//Limit points for the Larray and Rarray
			int Rarraypoint = 0;
			
			//Finding expanding interval pattern for neighbourhood
			for (int i = 0; i < limit2; i++){	//Loop through the number of occurrence of the expanding letter
				int wordpos = getposition[i]-leftneigh;	//Starting position for the interval of the current position of the expanding letter in the word.
				for(int j = 0; j < lengthneigh; j++){ 	//Loop through size of interval
					if(newsetdata[wordpos+j].L == 1){ 	//Checking if there is an L for that position. 
						Larray[Larraypoint] = j;		//Add the current interval literation to the L templete (not the position of the letter in the word)
						Larraypoint = Larraypoint + 1;	//Increase the Larray limit.	
					}
					if(newsetdata[wordpos+j].R == 1){
						Rarray[Rarraypoint] = j;
						Rarraypoint = Rarraypoint + 1;
					}
				}
				//Sort the templete, since we are adding new values at the end of the array.
				Larraypoint = sortset(Larray, Larraypoint);
				Rarraypoint = sortset(Rarray, Rarraypoint);
			}
			
			//With the templete done, we add in the missing Ls and Rs.
			for (int i = 0; i < limit2; i++){
				int wordpos = getposition[i]-leftneigh;
				for(int j = 0; j < Larraypoint; j++){
					int Lwordpos = wordpos+Larray[j]; 	//interval templete: starting word position + to the where there exist an L within the interval templete
					if( newsetdata[Lwordpos].L == 0){
						newsetdata[Lwordpos].L = 1;		//Add the new position of L to the word.
						repeat = TRUE;					//Since we have added an L to the word, there is a possibility that it affect the other expanding interval, so we repeat the whole section later.
					}
				}
				for(int m = 0; m < Rarraypoint; m++){
					int Rwordpos = wordpos+Rarray[m];
					if( newsetdata[Rwordpos].R == 0){
						newsetdata[Rwordpos].R = 1;
						repeat = TRUE;
					}
				}
			}
			
			if (repeat == TRUE){
				c = Esize + 1;		//Reset the whole loop.
				repeat = FALSE;		//Reset boolean
			}
		}
		Lsize = 0;
		Rsize = 0;
		for (int i = 0; i < lenw+1; i++){
			if( newsetdata[i].L == 1 ){
				Lset[Lsize] = i;
				Lsize = Lsize + 1;
			}	
		}
		for (int j = 0; j < lenw+1; j++){
			if( newsetdata[j].R == 1 ){
				Rset[Rsize] = j;
				Rsize = Rsize + 1;
			}	
		}
		
		if(Esize == uniq){	//If number of expanding letter = to all unique letter in the word.
			primitive = 0;	//Word is primitive and break of the algorithm.
			break;
		}
	}
	
	//Printing out the results
	if(print == 1 || print == -1){
		//Morphic Factorisation
		int Estart = 0;
		int Eend = 0;
		int Epos1;
		int Epos2;
		int EEpos1;
		int EEpos2;
		int temp1 = 0;
		
		//Getting the prefix of the first expanding letter
		for (int m = 0; m < Esize; m++){
			if(word[Lset[temp1]] == Eset[m].letter){ 
				Epos1 = Lset[temp1];
				EEpos1 = m;
				break;
			}
			else if(m == Esize-1){
				temp1 = temp1 + 1;
				m = -1;
			}
		}
		if(temp1+1 >= Lsize-1){
			Eset[0].datastart = 0;
			Eset[0].dataend = lenw;
		}
		else{
			//Now getting all other prefixes of all expanding letter in the word and store them in Eset as the morphic factorisation of that expanding letter
			for (int k = temp1+1; k < Lsize-1; k++){
				int temp2 = 0;
				for (int n = 0; n < Esize; n++){
					if(word[Lset[k+temp2]] == Eset[n].letter){
						Epos2 = Lset[k];
						EEpos2 = n;
						break;
					}
					if( n == Esize-1){
						temp2 = temp2 + 1;
						n = -1;
					}
				}
				k = k+temp2;
				for (int p = 0; p < Rsize; p++){
					if(Rset[p] <= Epos2){
						Eend = Rset[p];
					}
					if(Rset[p] > Epos2){
						break;
					}
				}
				
				//Storing the factorisation 
				if(k == temp1+1 || Eset[EEpos1].datastart == -1){
					Eset[EEpos1].datastart = Estart;
					Eset[EEpos1].dataend = Eend;
				}
				Estart = Eend;
				Epos1 = Epos2;
				EEpos1 = EEpos2;
			}
		}
		printf("Word: %s\n", word);
		if( print == -1) {
		//Print out the Ls and Rs and the word
		printf(" L");
		
		for (int k = 1; k < lenw+1; k++){
			for (int m = 0; m < Lsize; m++){
				if( k == Lset[m]){
					printf(" L");
					break;
				}
				if( k < Lset[m]){
					printf("  ");
					break;
				}
			}
		}
		printf("\n |%c", word[0]);
		
		for (int k = 1; k < lenw; k++){
			printf("|%c", word[k]);
		}
		printf("|\n R");
		
		for (int k = 1; k < lenw+1; k++){
			for (int m = 0; m < Rsize; m++){
				if( k == Rset[m]){
					printf(" R");
					break;
				}
				if( k < Rset[m]){
					printf("  ");
					break;
				}
			}
		}
		printf("\n");
		}
		//Print out the morphicfactorisation of the word
		for (int j = 0; j < uniq; j++){
			if(letterdata[j].inE != -1){
				int temp4 = letterdata[j].inE;
				printf(" %c -> ", Eset[temp4].letter);
				printf("%.*s,  ", Eset[temp4].dataend-Eset[temp4].datastart, word + Eset[temp4].datastart);
			}
			else{
				printf(" %c -> Empty, ",letterdata[j].letter);
			}
		}
		printf("\n\n");
	}
	//free the memory
	for (int i = 0; i < uniq; i++){
		free(letterdata[i].positions);
	}
	free(Eset);
	free(newsetdata);
	free(letterdata);
	
	return primitive;
}


unsigned long long totalprim = 0;
unsigned long long totalcount = 0;
void *thr_func(void *inputdata) {
	data_t *getdata = (data_t *)inputdata;
	struct results newdata = setgeneration(getdata);
	pthread_mutex_lock(&lock_x);
		totalprim = totalprim + newdata.prim;
		totalcount = totalcount + newdata.total;
	pthread_mutex_unlock(&lock_x);
	
	return NULL;
}

void *thr_func2(void *inputdata) {
	data_t2 *getdata = (data_t2 *)inputdata;
	int m = (*getdata).m;
	int n = (*getdata).n;
	int p = (*getdata).p;
	sp(m,p,(*getdata).c,n,(*getdata).save,(*getdata).savetype,(*getdata).print);
	/*pthread_mutex_lock(&lock_x);
		printf("passed %lld\n",totalcount);
		//totalprim = totalprim + newdata.prim;
		//totalcount = totalcount + newdata.total;
	pthread_mutex_unlock(&lock_x);*/
	
	return NULL;
}



//Setpartition functions
void createdata(data_t * input, int num, int op1){
	//data_t data = *input;
	int nit = num;
	int op = op1;
	int n = (*input).n;
	//data.p[n+1];
	//data.q[n+1];
	(*input).nc = nit;
	(*input).test1 = nit+1;
	(*input).test2 = 1;
	(*input).mtc = 0;
	(*input).p[0] = 0;
	(*input).q[0] = 0;
	if(nit == 2){
		for (int i = 1; i < n+1; i++){
			(*input).p[i] = 0;
			(*input).q[i] = 0;
		}
		(*input).mtc = 0;
	}
	else{
		(*input).mtc = 1;
		if(op > 1){
			(*input).nc = op;
			int stage = n - nit;
			(*input).test1 = op+1;
			(*input).test2 = nit-op+1;
			//p set
			(*input).p[1] = stage;
			int sum = stage;
			for (int i = 2; i < nit+1; i++){
				if( i == op){
					(*input).p[i] = n - sum;
				}
				else{
					(*input).p[i] = 1;
					sum++;
				}
			}
			for(int i = nit+1; i < n+1; i++){
				(*input).p[i] = 0;
			}
			
			int subt = op;
			int change = n - nit + op -2;
			for (int j = n; j > 0; --j){
				if( j <= change ){
					if( subt > 1){
						subt = subt-1;
						(*input).q[j] = subt;
					}
					else{
						(*input).q[j] = 1;
					}
				}
				else{
					(*input).q[j] = subt;
				}
			}
		}
		else{
			(*input).p[1] = n-nit+1;
			for (int i = 2; i < n+1; i++){
				if(i <= nit){
					(*input).p[i] = 1;
				}
				else{
					(*input).p[i] = 0;
				}
			}
			int temp = nit;
			//printf("temp = %d",temp);
			for (int j = n; j > 0; --j){
				if(temp > 1){
					(*input).q[j] = temp;
					temp = temp - 1;
				}
				else{
					(*input).q[j] = 1;
				}
			}
		}
	}
	if(op == 1){
		(*input).test1 = op+1;
		(*input).test2 = (*input).nc;
	}
	if(op == nit-1){ //here
		(*input).test1 = nit+1;
		(*input).test2 = 1;
	}
}

struct results setgeneration(data_t * input){
	int m;
	int l;
	int n = (*input).n;
	char tempset[n];
	int temp;
	struct results data;
	unsigned long count = 0;
	unsigned long prim = 0;
	while( (*input).p[(*input).test1] != (*input).test2 ){
		if( (*input).mtc == 0 ){
			(*input).nc = 1;
			for (int i = 0; i < n+1; i++) {
				(*input).q[i] = 1;
			}
			(*input).p[1] = n;
		}
		else{
			m = n;
			l = (*input).q[m];
			while( (*input).p[l] == 1 ){
				(*input).q[m] = 1;
				m = m - 1;
				l = (*input).q[m];				
			}
			(*input).nc = (*input).nc + m - n;
			(*input).p[1] = (*input).p[1] + n - m;
			if( l == (*input).nc ){
				(*input).nc = (*input).nc + 1;
				(*input).p[(*input).nc] = 0;
			}
			(*input).q[m] = l + 1;
			(*input).p[l] = (*input).p[l] - 1;
			(*input).p[l+1] = (*input).p[l+1] + 1;
		}
		if( (*input).nc != n ){
			(*input).mtc = 1;
		}else{
			(*input).mtc = 0;
		}
		count = count + 1;
		char temp2;
		for(int i = 1; i < n + 1; i++){
			temp2 = converttoletter((*input).q[i]);
			tempset[i-1] = temp2;
		}
		tempset[n] = '\0';
		temp = holubAlg(tempset,n,(*input).print);
		if(temp == 0){
			prim = prim + 1;
		}
		if((*input).save == 1){
			fpp2 = fopen(savefile_name2,"a");	
			if(temp == 0){
				if((*input).savetype == 1 || (*input).savetype == 3){
					fputs(tempset,fpp2);		
					fputs("\n",fpp2);
				}
				else if((*input).savetype == 4){
					fputs(tempset,fpp2);
					fputs(" : primitive ",fpp2);
					fputs("\n",fpp2);
				}
			}
			else{
				if((*input).savetype == 2 || (*input).savetype == 3){
					fputs(tempset,fpp2);
					fputs("\n",fpp2);
				}
				else if((*input).savetype == 4){
					fputs(tempset,fpp2);
					fputs(" : imprimitive",fpp2);
					fputs("\n",fpp2);
				}
			}
			fclose(fpp2);
		}
	}
	data.prim = prim;
	data.total = count;
	return data;
 }

//Set partition 2
int addset(int * c,int n,int save, int savetype, int print){
	char tempset[n];
	int temp;
	char temp2;
	for(int i = 1; i < n + 1; i++){
		temp2 = converttoletter(c[i]);
		tempset[i-1] = temp2;
	}
	tempset[n] = '\0';
	temp = holubAlg(tempset,n,print);
	if(save == 1){
		fpp2 = fopen(savefile_name2,"a");	
		if(temp == 0){
			if(savetype == 1 || savetype == 3){
				fputs(tempset,fpp2);		
				fputs("\n",fpp2);
			}
			else if(savetype == 4){
				fputs(tempset,fpp2);
				fputs(" : primitive ",fpp2);
				fputs("\n",fpp2);
			}
		}
		else{
			if(savetype == 2 || savetype == 3){
				fputs(tempset,fpp2);
				fputs("\n",fpp2);
			}
			else if(savetype == 4){
				fputs(tempset,fpp2);
				fputs(" : imprimitive",fpp2);
				fputs("\n",fpp2);
			}
		}
		fclose(fpp2);
	}
	
	return temp;
	
} 

void sp(int m, int p, int * c, int n, int s, int sty, int pt){
	int value = 0;
	unsigned long long count = 0;
	unsigned long long prim = 0;
	//printf("m = %d, p = %d, n = %d\n",m,p,n);
	if (p < n){
		for (int i = 1; i < m+1; i++){
			c[p] = i;
			sp(m,p+1,c,n,s,sty,pt);
			//printf("prim %d, count %d\n",prim,count);
		}
		if( (c[2] == 2 && p-1 != m) || (c[2] == 1 && p-2 != m)  ){
			c[p] = m + 1;
			sp(m+1,p+1,c,n,s,sty,pt);
			//printf("prim %d, count %d\n",prim,count);
		}
	}
	else if (p == n){
		for (int i = 1; i < m+2; i++){
			c[p] = i;
			value = addset(c,n,s,sty,pt);
			count = count + 1;
			if(value == 0){
				prim = prim + 1;
			}
		}
		pthread_mutex_lock(&lock_x);
			totalprim = totalprim + prim;
			totalcount = totalcount + count;
		pthread_mutex_unlock(&lock_x);
		
	}
	
	//return data;
}

/*unsigned long long getprim(){
	unsigned long long gettempdata = totalprim;
	return gettempdata;	
}

unsigned long long getcount(){
	unsigned long long gettempdata2 = totalcount;
	return gettempdata2;	
}

void addtotal(struct results final){
	totalprim = totalprim + final.prim;
	totalcount = totalcount + final.total;	
}*/


int multithread(int mode1, int N11, int save1, int print1){
	
	pthread_t thr[10];
	int rc;
	int mode = mode1;
	int N1 = N11;
	int save2 = save1;
	int savetype2 = 0;
	int print = print1;

	pthread_mutex_init(&lock_x, NULL);
	pthread_mutex_init(&lock_y, NULL);


	
	if(save2 == 1){
		printf("Enter filename to be created: ");
		scanf("%s", savefile_name2);
		strcpy(sbackup_name2, savefile_name2);
		strcat(savefile_name2,ext2);	
		printf("Do you wish to save: \n(1) All primitive words   (2) All imprimitive words   (3) Both \n");
		scanf("%d", &savetype2);
		if(savetype2 == 3){
			printf("Do you want to label them? 	Yes = 1 No = 0 \n");
			int tempb;
			scanf("%d", &tempb);
			savetype2 = savetype2 + tempb;
		}	
	}
	
	if ( mode == 1){
		int sizenum = 0;
		for (int i = 2; i < N1; i++){
			if(i > 11){
				sizenum = sizenum + i - 1; //here
			}
			else{
				sizenum++;
			}
		}
		printf("sizenum %d\n", sizenum);
		data_t setdata[sizenum];
		
		int count = 0;
		int num = 2;
		int op1 = 0;
		for(int i = 0; i < N1-2; i++){
			if( num > 11){
				for(int j = 0; j < num-1; j++){ //here
					op1++;
					setdata[count].n = N1;
					setdata[count].save = save2;
					setdata[count].savetype = savetype2;
					setdata[count].print = print;
					setdata[count].p = (int*)malloc(sizeof(int) * N1+1);
					setdata[count].q = (int*)malloc(sizeof(int) * N1+1);
					createdata(&setdata[count], num, op1);
					count++;
				}
				op1 = 0;
				
			}else{
				setdata[count].n = N1;
				setdata[count].save = save2;
				setdata[count].savetype = savetype2;
				setdata[count].print = print;
				setdata[count].p = (int*)malloc(sizeof(int) * N1+1);
				setdata[count].q = (int*)malloc(sizeof(int) * N1+1);
				createdata(&setdata[count], num, op1);
				count++;
			}
			num++;		
		}
		/*for(int k = 0; k < sizenum; k ++){
			printf("count = %d, n = %d, nc = %d, test1 = %d, test2 = %d, mtc = %d\n",k,setdata[k].n,setdata[k].nc,setdata[k].test1,setdata[k].test2,setdata[k].mtc);
			printf("p: ");
			for (int l = 0; l < N1+1; l++){
				printf("%d,",setdata[k].p[l]);
			}
			printf("\n q: ");
			for (int l = 0; l < N1+1; l++){
				printf("%d,",setdata[k].q[l]);
			}
			printf("\n");
		}*/
		struct results final;
		for(int k = 0; k < sizenum; k++){
			if ( k > 5 ){
				int limit = 5;
				while ( k+limit > sizenum ){
					limit = limit - 1;
				}
				for(int p = 0; p < limit-1; p++){
					if ((rc = pthread_create(&thr[p], NULL, thr_func, &setdata[k]))) {
						fprintf(stderr, "error: pthread_create, rc: %d\n", rc);
						return EXIT_FAILURE;
					}
					printf("%d pthread_create(%d) success \n",p,k);
					k++;
				}
				final = setgeneration(&setdata[k]);
				printf("main pthread_create(%d) success \n",k);
				//printf("passed %d\n",totalcount);
				for(int p = 0; p < limit-1; p++){
					pthread_join(thr[p], NULL);
					free(setdata[k-p-1].p);
					free(setdata[k-p-1].q);
					printf("pthread_join(%d) success \n",p);
				}
				totalprim = totalprim + final.prim; 
				totalcount = totalcount + final.total; 
				//printf("passed %lld\n",totalcount);
				free(setdata[k].p);
				free(setdata[k].q);
				
			}else{
				final = setgeneration(&setdata[k]);
				printf("main pthread_create(%d) success \n",k);
				totalprim = totalprim + final.prim;
				totalcount = totalcount + final.total;		
				//printf("passed %lld\n",totalcount);			
				free(setdata[k].p);
				free(setdata[k].q);				
			}
		}
		
	}
	else{
		int point = 0;
		for(int i = 0; i < N1-2; i++){
			for(int e = 1; e < 3; e++){
				point++;
			}
		}
		
		/*for(int i = 0; i < N1-1; i++){
			setdata2[i].c = (int*)malloc(sizeof(int) * N1+1);
			setdata2[i].m = i+1;
			setdata2[i].p = i+2;
			setdata2[i].n = N1;
			for (int j = 0; j < N1+1; j++){
				if(j <= setdata2[i].m ){
					setdata2[i].c[j] = j;
				}
				else{
					setdata2[i].c[j] = 0;
				}
			}
			point++;
		}*/
		data_t2 setdata2[point+1];
		//printf("size %d\n", point);
		int count2 = 0;
		for(int i = 0; i < N1-2; i++){
			for(int e = 1; e < 3; e++){
				setdata2[count2].c = (int*)malloc(sizeof(int) * N1+1);
				setdata2[count2].m = i+e;
				setdata2[count2].p = i+3;
				setdata2[count2].n = N1;
				setdata2[count2].save = save2;
				setdata2[count2].savetype = savetype2;
				setdata2[count2].print = print;
				int nums = setdata2[count2].m;
				int subtt = 2-e;
				for (int j = N1; j > -1; --j){
					if(j <= setdata2[count2].m+subtt){
						if (nums == 1 && j > 1){
							setdata2[count2].c[j] = nums;
						}
						else{
							setdata2[count2].c[j] = nums;
							--nums;
						}
					}
					else{
						setdata2[count2].c[j] = 0;
					}
				}
				count2++;
			}
		}
		printf("size %d\n", count2);
		/*for(int i = 0; i < point; i++){
			printf(" i = %d, m = %d, p = %d, n = %d",i,setdata2[i].m,setdata2[i].p,setdata2[i].n);
			printf("c :");
			for (int j = 0; j < N1+1; j++){
				printf("%d,",setdata2[i].c[j]);
			}
			printf("\n");
		}*/
		//sleep(2);
		//struct results final2;
		/*for(int i = 0; i < N1; i++){
			sp(setdata2[i].m, setdata2[i].p, setdata2[i].c,setdata2[i].n);
			printf("i = %d, prim %ld, count %ld\n", i,final2.prim, final2.total);
			totalprim = totalprim + final2.prim;
			totalcount = totalcount + final2.total;	
		}*/
		for(int k = point-1; k > -1; --k){
			if ( k < 16 && point-1 > 14){ //19 = 15
				int limit = 6;
				while ( k-limit < -1 ){
					limit = limit - 1;
				}
				for(int p = 0; p < limit-1; p++){
					if ((rc = pthread_create(&thr[p], NULL, thr_func2, &setdata2[k]))) {
						fprintf(stderr, "error: pthread_create, rc: %d\n", rc);
						return EXIT_FAILURE;
					}
					printf("%d pthread_create(%d) success \n",p,k);
					--k;
				}
				sp(setdata2[k].m, setdata2[k].p, setdata2[k].c,setdata2[k].n,setdata2[k].save,setdata2[k].savetype,setdata2[k].print);
				printf("main pthread_create(%d) success \n",k);
				//free(setdata2[k].c);
				//printf("passed %d\n",totalcount);
				for(int p = 0; p < limit-1; p++){
					pthread_join(thr[p], NULL);
					//printf("%d",k+p+1);
					//free(setdata2[k+p+1].c);
					printf("pthread_join(%d) success \n",p);
				}
				//printf("passed %d\n",totalcount);
			}
			else{
				sp(setdata2[k].m, setdata2[k].p, setdata2[k].c,setdata2[k].n,setdata2[k].save,setdata2[k].savetype,setdata2[k].print);
				printf("main pthread_create(%d) success \n",k);
			}
		}
	
	}
	
	pthread_mutex_destroy(&lock_x);
	
	printf("Totalprim %lld, Totalcount %lld\n", totalprim, totalcount);
	
	totalprim = 0;
	totalcount = 0;
	
	return EXIT_SUCCESS;
	
}


char converttoletter(int number){
	char letter;
	switch(number) {
		case 1:
			letter = 'a';
			break;
		case 2:
			letter = 'b';
			break;
		case 3:
			letter = 'c';
			break;
		case 4:
			letter = 'd';
			break;
		case 5:
			letter = 'e';
			break;
		case 6:
			letter = 'f';
			break;
		case 7:
			letter = 'g';
			break;
		case 8:
			letter = 'h';
			break;
		case 9:
			letter = 'i';
			break;
		case 10:
			letter = 'j';
			break;
		case 11:
			letter = 'k';
			break;
		case 12:
			letter = 'l';
			break;
		case 13:
			letter = 'm';
			break;
		case 14:
			letter = 'n';
			break;
		case 15:
			letter = 'o';
			break;
		case 16:
			letter = 'p';
			break;
		case 17:
			letter = 'q';
			break;
		case 18:
			letter = 'r';
			break;
		case 19:
			letter = 's';
			break;
		case 20:
			letter = 't';
			break;
		case 21:
			letter = 'u';
			break;
		case 22:
			letter = 'v';
			break;
		case 23:
			letter = 'w';
			break;
		case 24:
			letter = 'x';
			break;
		case 25:
			letter = 'y';
			break;
		case 26:
			letter = 'z';
			break;
				
		default :
			printf("Invalid grade\n" );
			letter = '*';
			break;
	}
	
	return letter;
}










