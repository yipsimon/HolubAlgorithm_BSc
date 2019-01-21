#ifndef MAIN_H_   /* Include guard */
#define MAIN_H_

struct data {
	char letter;
	int numoccur;
	int* positions;
	int leftlimit;
	int rightlimit;
	int inE;
};

struct Estr {
	char letter;
	int datapos;
	int datastart;
	int dataend;
};

struct LRdata{
	int L;
	int R;
};

void addnewletter(struct data *info, char word, int location, int position);
int sortset (int set[], int size);

int holubAlg(char * str, int l, int p);


//Global
char converttoletter(int number);
int nlen;
int globprint;
int globprintsty;
unsigned long setsize;
unsigned long sprim;
unsigned long simprim;

pthread_mutex_t lock_x;
pthread_mutex_t lock_y;

int multithread(int mode1, int N11, int save1, int print1);
/*unsigned long long getprim();
unsigned long long getcount();*/


struct results{
	unsigned long long prim;
	unsigned long long total;
 };

//void addtotal(struct results final);
void *thr_func(void *inputdata);
void *thr_func2(void *inputdata);
 
 
//setpartition 1
typedef struct _data_t {
	int n;
	int nc;
  	int* p;
	int* q;
	int test1;
	int test2;
	int mtc ;
	int save;
	int savetype;
	int print;
	//char set[30];
} data_t;


	
void createdata(data_t * input, int num, int op1);
struct results setgeneration(data_t * input);

//setpartition 2
typedef struct _data_t2 {
	int m;
	int p;
	int *c;	
	int n;
	int save;
	int savetype;
	int print;
} data_t2;


int addset(int * c,int n, int s, int sty, int pt);
void sp(int m, int p, int * c, int n, int s, int sty, int pt);

 




#endif































