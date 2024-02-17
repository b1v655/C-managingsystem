#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <conio.h>

time_t actualtime(){
	time_t t;
	time(&t);
	return t;
}

 struct data{
	time_t systime;
	char name[40];
	char email[40];
	char telephonnumber[11];
	int systemstarke;
	struct data* pointer;
};

void stddatawrite(struct data* t)
{
	printf("%s %s %s %s %d\n\n",ctime(&t->systime), t->name,t->email,t->telephonnumber,t->systemstarke);
}

struct data* stddataread(){
	struct data* t=(struct data*) malloc(sizeof(struct data));
	t->systime=actualtime();
	printf("Adja meg a nevet: ");
	scanf("%s",t->name);
	printf("Adja meg az e-mailt: ");
	scanf("%s",t->email);
	printf("Adja meg a telefonszamot: ");
	scanf("%s",t->telephonnumber);
	printf("Adja meg a rendszerteljesitmenyt: ");
	scanf("%d",&t->systemstarke);
	t->pointer=NULL;	
	return t;
}

int checker(char input[],char check[])
{
    int i,result=1;
    for(i=0; input[i]!='\0' || check[i]!='\0'; i++) {
        if(input[i] != check[i]) {
            result=0;
            break;
        }
    }
    return result;
}

struct data* first=NULL;
struct data* last=NULL;

void getline()
{
	system("cls");
	printf("Adatfelvetel:\n");
	struct data* t=stddataread();
	if(first==NULL){
		first=t;
		last=t;
	}else{
		last->pointer=t;
		last=t;
	}
}

void andern()
{
	system("cls");
	printf("Adatmodositas:\n");
	struct data* t=first;
	char search[40];
	scanf("%s",search);
	while(t!=NULL&&checker(t->name,search)==0){
		t=t->pointer;
	}
	if(t!=NULL){
		printf("A modositani kivant rekord:");
		stddatawrite(t);
		t->systime=actualtime();
		scanf("%s %s %s %d", t->name,t->email,t->telephonnumber,&t->systemstarke);
	}else{
		printf("Nem talalhato a keresett rekord!");
	}
	getch();
}

void delete()
{
	system("cls");
	printf("Adattorles:\n");
	char search[40];
	scanf("%s",search);
	struct data* t=first;
	struct data* p=NULL;
	while(t!=NULL&&checker(t->name,search)==0){
		p=t;
		t=t->pointer;
	}
	if(t!=NULL){
		printf("A kitorolt rekord:");
		stddatawrite(t);
		if(first==t)first=t->pointer;
		if(last==t)last=p;
		if(p!=NULL)	p->pointer=t->pointer;
		free(t);
	}else{
		printf("Nem talalhato a keresett rekord!\n");
	}
	getch();
		
}

void all()
{
	system("cls");
	printf("Kilistazott adatok:\ndatum\t nev\t e-mail\t teljesitmeny\n-----\t ---\t ------\t -----------\n");
	struct data* t=first;
	while(t!=NULL){
		stddatawrite(t);
		t=t->pointer;
	}
}

void  byname()
{
	system("cls");
	printf("Nev szerinti listazas: \n");
	struct data* t=first;
	char search[40];
	printf("Adjon meg egy nevet:"); 
	scanf("%s",search);
	printf("datum\t nev\t e-mail\t teljesitmeny\n-----\t ---\t ------\t -----------\n");
	while(t!=NULL){
		if(checker(t->name,search)==1)stddatawrite(t);
		t=t->pointer;
	}
}

void bysyspower()
{
	system("cls");
	printf("Teljesitmeny szerinti listazas: \n");
	struct data* t=first;
	int power;
	printf("Adja meg a listazni kivant teljesitmenyt:"); 
	scanf("%d",&power);
	printf("datum\t nev\t e-mail\t teljesitmeny\n-----\t ---\t ------\t -----------\n");
	while(t!=NULL){
		if(t->systemstarke==power)stddatawrite(t);
		t=t->pointer;
	}
}

void listing()
{
	char c;
	printf("\t 1-osszes,\n\t 2-szurt nevre,\n\t 3-szurt teljesitmenyre\n");
	do{
		c = getch();
	}while(c!='1'&&c!='2'&&c!='3');
	switch (c){
		case '1': all();break;
		case '2': byname();break;
		case '3': bysyspower();break;
	}
	getch();

	}
void readfile()
{
	
	
}

void writefile()
{
	FILE *fp;
	fp=fopen("database.txt", "w");
	if(fp != NULL){
		struct data* t=first;
		while(t!=NULL){
			fprintf(fp, "%s|%s|%s|%|s|%d\n",ctime(&t->systime), t->name,t->email,t->telephonnumber,t->systemstarke);
			t=t->pointer;
		}
		fclose(fp);
	}
}

void deallocate()
{
	last=first;
	while(last!=NULL){
		last=first->pointer;
		free(first);
		first=last;
	}
}
int main()
{
	readfile();
	char c;
	
	do{
		system("cls");
		printf(" 0-Kilepes,\n 1-adatfelvetel,\n 2-adatmodositas,\n 3-adattorles,\n 4-adatlistazas.\n");
		do{
			c = getch();
		}while(c!='0'&&c!='1'&&c!='2'&&c!='3'&&c!='4');
		switch (c){
			case '1': getline();break;
			case '2': andern();break;
			case '3': delete();break;
			case '4': listing();break;
		}		
	}while(c!='0');
	writefile();
	deallocate();
	return 0;
}