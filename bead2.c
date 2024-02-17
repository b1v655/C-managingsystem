#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <sys/types.h>
#include <string.h>
#include <time.h>
#include <unistd.h>
#include <wait.h>
#include <signal.h>
char* actualtime(){
	time_t t;
	time(&t);
	char* s= ctime(&t);
	*(s+strlen(s)-1)='\0';
	return s;
}
struct data{
	char systime[40];
	char name[40];
	char email[40];
	char telephonnumber[11];
	int systemstarke;
	int isCompleted;
	struct data* pointer;
};
typedef struct {
    char systime[40];
	char name[40];
	char email[40];
	char telephonnumber[11];
	int systemstarke;
} Job;
typedef struct{
    int numjobs;
    Job work[2];
} Jobs;
Job copyJob(struct data* in){
    Job j;
    strcpy(j.email,in->email);
    strcpy(j.name,in->name);
    strcpy(j.systime,in->systime);
    strcpy(j.telephonnumber,in->telephonnumber);
    j.systemstarke=in->systemstarke;
    return j;
};

int checker(const char input[],char check[])
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


void stddatawrite(struct data* t)
{
	printf("%s, %s, %s, %s, %d W, %d \n\n",t->systime, t->name,t->email,t->telephonnumber,t->systemstarke,t->isCompleted);
}
void listing(struct data* first,struct data* last)
{
	system("cls");
	printf("datum\t\t\t  nev\t e-mail\t telefonszam\t teljesitmeny\t elkeszult\n-----\t\t\t  ---\t ------\t -----------\t -----------\t ---------\n");
	struct data* t=first;
	while(t!=NULL){
		stddatawrite(t);
		t=t->pointer;
	}
}
void readfile(struct data** first,struct data** last)
{
	
	FILE *file;
    file = fopen("database.txt", "r");
	char* s;
	char seged[7];
	while(fgetc(file)!=EOF){
		struct data* t=(struct data*) malloc(sizeof(struct data));
		char ch;
		int i;
		for(i=0;(ch = (char)fgetc(file)) != '|';++i)t->systime[i] = ch;
		t->systime[i] = '\0';
		for(i=0;(ch = (char)fgetc(file)) != '|';++i)t->name[i]=ch;
		t->name[i] = '\0';
		for(i=0;(ch = (char)fgetc(file)) != '|';++i)t->email[i]=ch;
		t->email[i] = '\0';
		for(i=0;(ch = (char)fgetc(file)) != '|';++i)t->telephonnumber[i]=ch;
		t->telephonnumber[i] = '\0';
		for(i=0;(ch = (char)fgetc(file)) != '|';++i)seged[i]=ch;
		seged[i] = '\0';
		t->systemstarke=atoi(seged);
		for(i=0;(ch = (char)fgetc(file)) != '\n';++i)seged[i]=ch;
		seged[i] = '\0';
		t->isCompleted=atoi(seged);
		t->pointer=NULL;
		if(*first==NULL){*first=t;*last=t;
		}else{(*last)->pointer=t;*last=t;}
	}
}
time_t dttm(char * datetime){
    struct tm tm;
    strptime(datetime, "%a-%b-%d-%H:%M:%S-%Y", &tm);
    time_t t = mktime(&tm);
    return t;
}
void writefile(struct data* first,struct data* last)
{
	FILE *fp;
	fp=fopen("database.txt", "w");
	if(fp != NULL){
		struct data* t=first;
		while(t!=NULL){
			fprintf(fp, "\n%s|%s|%s|%s|%d|%d\n",t->systime, t->name,t->email,t->telephonnumber,t->systemstarke,t->isCompleted);
			t=t->pointer;
		}
		fclose(fp);
	}
}
void deallocate(struct data** first,struct data** last)
{
	*last=*first;
	while(*last!=NULL){
		*last=(*first)->pointer;
		free(*first);
		*first=*last;
	}
}
void handler(int signalnum){
  printf("Signal No. :%d\n", signalnum);
}
void program(struct data* first,struct data* last)
{
	double weekInSecs = 60*60*24*7;
	struct data * akt=first;
	
	if(akt==NULL) return;
	
	int pipefd[2];
    pid_t pid;
    if (pipe(pipefd) == -1)
    {
        perror("Hiba a pipe nyitaskor!");
        exit(EXIT_FAILURE);
    }
	
	while (akt->pointer!=NULL&& !(difftime(time(NULL), dttm(akt->systime))> weekInSecs&&akt->isCompleted==0)){
		akt=akt->pointer;
	}
	
	if (difftime(time(NULL), dttm(akt->systime)) > weekInSecs&&akt->isCompleted==0){
        
		signal(SIGUSR1, handler);
        pid=fork();
        if (pid == -1){
            perror("Fork hiba");
            exit(EXIT_FAILURE);
        }
        if(pid==0){//child
			printf("Gyerek.\n");
			struct data input;
			close(pipefd[1]);
			read(pipefd[0], &input, sizeof(struct data));
            close(pipefd[0]); 
            printf("Feladatok szama: 1\n");
            kill(getppid(), SIGUSR1);
            sleep(1);
            printf("Megoldva\n");
            kill(getppid(), SIGUSR1);
            char end[20]="A feladat elkeszult";
            close(pipefd[0]);
            write(pipefd[1], &end, sizeof(end));
            close(pipefd[1]);
            exit(0);
        }else{//parent
			printf("Szulo.\n");
			Job task=copyJob(akt);
            close(pipefd[0]);
            write(pipefd[1], &task,sizeof(Job));
            close(pipefd[1]); 
            pause();
            printf("Megjott a nyugta \n");
            pause();
            printf("Rendben \n");
            char end[20];
            close(pipefd[1]);
            read(pipefd[0], &end, sizeof(end));
            close(pipefd[0]);
            printf( "%s\n" ,end);
            akt->isCompleted=1;
        }
	}	else {
		int l=0;
		akt=first;
		struct data * akt2=NULL;
		while(akt->pointer!=NULL&&l!=1){
			akt2=akt->pointer;
			while(akt2->pointer!=NULL&&!(akt2->systemstarke==akt->systemstarke&&akt2->isCompleted==0&&akt->isCompleted==0))
				akt2=akt2->pointer;
			if(akt2->systemstarke==akt->systemstarke&&akt2->isCompleted==0&&akt->isCompleted==0)l=1;
			else akt=akt->pointer;
		}
		if(akt2!=NULL&akt2->systemstarke==akt->systemstarke&&akt2->isCompleted==0&&akt->isCompleted==0){
				signal(SIGUSR1, handler);
                pid=fork();
                if (pid == -1)
	            {
                     perror("Fork hiba");
                     exit(EXIT_FAILURE);
                }
                if(pid==0){//child
                   printf("Gyerek.\n");
                   Jobs input;
                   close(pipefd[1]);
                   read(pipefd[0], &input, sizeof(Jobs));
                   close(pipefd[0]);
                   printf("Feladatok szama: %d \n",input.numjobs);
                   kill(getppid(), SIGUSR1);
                   sleep(1);
                   printf("Megoldva\n");
                   kill(getppid(), SIGUSR1);
                   sleep(1);
                   printf("Megoldva\n");
                   kill(getppid(), SIGUSR1);
                   char end[20]="Minden kesz";
                   close(pipefd[0]);
                   write(pipefd[1], &end, sizeof(end));
                   close(pipefd[1]);
                   exit(0);
                }else{//parent
                   printf("Szulo.\n");
                   Job first=copyJob(akt);
                   Job second=copyJob(akt2);
                   Jobs works;
                   works.numjobs=2;
                   works.work[0]=first;
                   works.work[1]=second;
                   close(pipefd[0]);
                   write(pipefd[1], &works,sizeof(Jobs));
                   close(pipefd[1]); 
                   pause();//nyugta
                   printf("Megjott a nyugta \n");
                   pause();//elso
                   printf("Rendben 1 \n");
                   pause();//masodik
                   printf("Rendben 2 \n");
                   char end[20];
                   close(pipefd[1]);
                   read(pipefd[0], &end, sizeof(end));
                   close(pipefd[0]);
                   printf( "%s\n" ,end);
                   akt->isCompleted=1;
                   akt2->isCompleted=1;
                }
		} else {
			akt=first;
			while(akt->pointer!=NULL&&akt->isCompleted!=0){
				akt=akt->pointer;
			}
			if (akt->isCompleted==0){
				signal(SIGUSR1, handler);
				pid=fork();
				if (pid == -1){
					perror("Fork hiba");
					exit(EXIT_FAILURE);
				}
				if(pid==0){//child
					printf("Gyerek.\n");
					struct data input;
					close(pipefd[1]);
					read(pipefd[0], &input, sizeof(struct data));
					close(pipefd[0]); 
					printf("Feladatok szama: 1\n");
					kill(getppid(), SIGUSR1);
					sleep(1);
					printf("Megoldva\n");
					kill(getppid(), SIGUSR1);
					char end[20]="A feladat elkeszult";
					close(pipefd[0]);
					write(pipefd[1], &end, sizeof(end));
					close(pipefd[1]);
					exit(0);
				}else{//parent
					printf("Szulo.\n");
					Job task=copyJob(akt);
					close(pipefd[0]);
					write(pipefd[1], &task,sizeof(Job));
					close(pipefd[1]); 
					pause();
					printf("Megjott a nyugta \n");
					pause();
					printf("Rendben \n");
					char end[20];
					close(pipefd[1]);
					read(pipefd[0], &end, sizeof(end));
					close(pipefd[0]);
					printf( "%s\n" ,end);
					akt->isCompleted=1;
				}
			}		
		}
	}
}

int main()
{
	struct data* first=NULL;
	struct data* last=NULL;
	readfile(&first,&last);
	//listing(first,last);
	program(first,last);
	writefile(first,last);
	deallocate(&first,&last);
	return 0;
}
