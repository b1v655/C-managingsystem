#include <sys/ipc.h> 
#include <sys/msg.h> 
#include <sys/types.h> 
#include <stdio.h> 
#include <stdlib.h> 
#include <string.h> 
#include <unistd.h> 
#include <wait.h>
#include <fcntl.h>
#include <errno.h>
#include <signal.h>

struct uzenet { 
     long mtype;//ez egy szabadon hasznalhato ertek, pl uzenetek osztalyozasara
     char mtext [ 1024 ]; 
};
void uzenet(int uzisor,struct uzenet uz){
	int status; 
     status = msgsnd( uzisor, &uz, strlen ( uz.mtext ) + 1 , 0 ); 
	// a 3. param ilyen is lehet: sizeof(uz.mtext)
     	// a 4. parameter gyakran IPC_NOWAIT, ez a 0-val azonos
     if ( status < 0 ) 
          perror("msgsnd"); 
}	
struct uzenet fogad(int uzisor,int mi){
	 struct uzenet uz; 
     int status; 
     // az utolso parameter(0) az uzenet azonositoszama
	 // ha az 0, akkor a sor elso uzenetet vesszuk ki
	 // ha >0 (5), akkor az 5-os uzenetekbol a kovetkezot
	 // vesszuk ki a sorbol 
    status = msgrcv(uzisor, &uz, 1024, mi, 0 ); 
     
    if ( status < 0 ) 
          perror("msgsnd"); 
    return uz; 
}
void betoro(int uzenetsor){
	struct uzenet uz =fogad(uzenetsor,5);
	printf("Elestem %s darab jatekon \n",uz.mtext);
	strcpy(uz.mtext,"Varja elkaplak!!!");
	//printf("Mit is hallok: %s",uz.mtext);
	uz.mtype=2;
	uzenet(uzenetsor,uz);
	printf("elkuldve\n");
}
void kevin(int uzenetsor){
	srand(time(NULL)); //the starting value of random number generation
    int r=rand()%30+20; //number between 0-99
	struct uzenet uz;
	uz.mtype=5;
    sprintf(uz.mtext, "%d", r);
	uzenet(uzenetsor,uz);
	//strcpy(uz.mtext,"Varja elkaplak!!!");
	//uzenet(uzenetsor,uz);
	uz=fogad(uzenetsor,2);
	printf("Mit is hallok: %s\n",uz.mtext);
	
}
void handler(int signumber){
  printf("Jonnek");
}
int main (int argc, char* argv[]) { 
     signal(SIGUSR1,handler);
     pid_t child1; 
	 int fd;
     int uzenetsor, status; 
     key_t kulcs; 
     int fid=mkfifo("fifo.ftc", S_IRUSR|S_IWUSR ); // creating named pipe file
    // S_IWGRP, S_IROTH (other jog), file permission mode
    // the file name: fifo.ftc
    // the real fifo.ftc permission is: mode & ~umask 
	
    if (fid==-1)
    {
	printf("Error number: %i",errno);
	exit(EXIT_FAILURE);
    }
     // msgget needs a key, amelyet az ftok generál 
     kulcs = ftok(argv[0],1); 
     //printf ("A kulcs: %d\n",kulcs);
     uzenetsor = msgget( kulcs, 0600 | IPC_CREAT );//kevin a  
     if ( uzenetsor < 0 ) { 
          perror("msgget"); 
          return 1; 
     } 
     
     child1 = fork(); 
     if ( child1 > 0 ) { //szulo
	    pid_t child2 =fork();
		if(child2>0){//szulo nem csinal semmit
			wait(NULL);
		}else{//betoro
			betoro(uzenetsor);
			sleep(1);
			kill(child1,SIGUSR1);
			char s[1024]="Semmi";
			fd=open("fifo.ftc",O_RDONLY);
	        read(fd,s,sizeof(s));
	        printf("Ezt olvastam a csobol: %s \n",s);
	        close(fd);
			printf("vegeb/n");
		}
        wait(NULL);  
     } else if ( child1 == 0 ) {//Kevin 
	     sigset_t sigset;
		 sigfillset(&sigset);
		 sigprocmask(SIG_BLOCK, &sigset, NULL);
		
         kevin(uzenetsor);
		 printf("vegec/n");
		 
		 struct sigaction sigact;
		 sigemptyset(&sigact.sa_mask);
		 sigact.sa_flags = 0;
		 sigact.sa_handler = handler;
		 sigaction(SIGUSR1, &sigact, NULL);
		
		 sigdelset(&sigset, SIGUSR1);
		 sigsuspend(&sigset);
		
		fd=open("fifo.ftc",O_WRONLY);
        write(fd,"festek",10);
	    close(fd);
     } else { 
          perror("fork"); 
          return 1; 
     }
     unlink("fifo.ftc");	 
     
     return 0; 
} 