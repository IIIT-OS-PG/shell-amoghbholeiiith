#include<iostream>
#include<string.h> 
#include<stdlib.h> 
#include<unistd.h> 
#include<sys/wait.h> 
#include<sys/types.h> 
#include <curses.h>
#include<readline/history.h> 
#include<readline/readline.h> 
#define clear() printf("\033[H\033[J")
#define WRITE_END 1
#define READ_END 0

using namespace std;

void execparseStringPiped(char **,char **);
void execparsePiped(char **);

int inputcommand(char *str){
	char *p;
	p = readline("\n$ ");
	if (strlen(p) != 0) {
		add_history(p); 
		strcpy(str, p); 
		return 0; 
	} else { 
		return 1; 
	}
}
int numberpipes=0;
void delimit(char* p, char** parsedList,char const* del){
	int i; 

	for (i = 0; i < 1000; i++) { 
		parsedList[i] = strsep(&p,del); 

		if (parsedList[i] == NULL) 
			break; 
		if (strlen(parsedList[i]) == 0) 
			i--;
	}
	numberpipes=i;
}
void shellinit(){
	clear();
}
int delimitepipe(char *p,char **pipedstr){
	
	delimit(p,pipedstr,"|");
	//cout<<"HI";
	if(pipedstr[1]==NULL)
		return 0;
	else 
	{
		return 1;
	}
}


/////parse for spaces
void parsewithSpace(char* p, char** parsedList) 
{ 
	//cout<<"HI";
	delimit(p,parsedList," ");
}
///////parse with pipe
/*void parsewithSpacePipe(char* p, char** parsedStrPiped) 
{ 
	
	int i;
	for (i = 0; i < 1000; i++) { 
		parsedStrPiped[i] = strsep(&p," "); 

		if (parsedStrPiped[i] == NULL) 
			break; 
		if (strlen(parsedStrPiped[i]) == 0) 
			i--;
	}
}*/

int parseString(char *p,char **parsedList,char **parsedStrPiped){
	char *pipedstr[100];
	char* parsedStrPiped2[1000];//////to store the second command in case of pipe
	bool ifpipe=delimitepipe(p,pipedstr);
	if(!ifpipe){
		parsewithSpace(p, parsedList);
		return 0;
	}else{
		//int count=0;
		
		//for(int i=0;i<numberpipes;i++){
			//if(i+1<numberpipes){
				//cout<<"HI";
				//cout<<pipedstr[i]<<" ";
		execparsePiped(pipedstr);
				//parsewithSpace(pipedstr[i],parsedStrPiped);
				//parsewithSpace(pipedstr[i+1],parsedStrPiped2);
				//cout<<parsedStrPiped[0]<<" "<<parsedStrPiped[1]<<endl;
				//cout<<parsedStrPiped2[0]<<" "<<parsedStrPiped2[1]<<endl;
				
				//execparseStringPiped(parsedStrPiped,parsedStrPiped2);
			//}
		//}
		
		return 1;
	}
	
}
void execparseString(char **parsedList){
	pid_t pid = fork(); 

	if (pid == -1) { 
		printf("\nFailed forking child.."); 
		return; 
	} else if (pid == 0) { 
		if (execvp(parsedList[0], parsedList) < 0) { 
			printf("\nCould not execute command.."); 
		} 
		exit(0); 
	} else { 
		// waiting for child to terminate 
		wait(NULL); 
		return; 
	} 
}

void execparsePiped(char **pipedstr){
	int p[2];
	pid_t pid;
	int fdd = 0;
	while(*pipedstr != NULL)
	{
		pipe(p);
		pid = fork();
		if(pid < 0)
			perror("Fork Failed !!!");
		else if(pid == 0)
		{
			dup2(fdd, 0);
			if(*(pipedstr + 1) != NULL)
				dup2(p[1], 1);
			close(p[0]);
			close(p[1]);
			char *processed_input2[1000];
			parsewithSpace(pipedstr[0] , processed_input2);
			if(execvp(processed_input2[0], processed_input2) == -1)
					cout << "Command not executed !!!" << endl;
			exit(0);
		}
		else
		{
			wait(NULL);
			close(p[1]);
			fdd = p[0];
			pipedstr++;
		}
	}
}
/*int flag_pipe_read=0;
void execparseStringPiped(char **parsedStrPiped,char **parsedStrPiped2){
	int fd[2]; 
	pid_t pid,pid2;
	pipe(fd);
	pid = fork();

	if(pid==0 && flag_pipe_read==0)
	{
		flag_pipe_read=1;
		close(fd[READ_END]);
	    dup2(fd[WRITE_END], STDOUT_FILENO);
	    close(fd[READ_END]);
	    close(fd[WRITE_END]);
	    if (execlp(parsedStrPiped[0],parsedStrPiped[0], parsedStrPiped[1], (char*) NULL) < 0) { 
			printf("\nCould not execute command.."); 
			exit(0);
		}
	    //execlp(firstcmd, firstcmd, frsarg, (char*) NULL);
	    //fprintf(stderr, "Failed to execute '%s'\n", firstcmd);
	    
	}
	else
	{ 
	    pid2=fork();

	    if(pid2==0)
	    {
	    	close(fd[WRITE_END]);
	        dup2(fd[READ_END], STDIN_FILENO);
	        close(fd[WRITE_END]);
	        close(fd[READ_END]);
	        
	        if(execlp(parsedStrPiped2[0],parsedStrPiped2[0],parsedStrPiped2[1],(char*) NULL)<0){
	        	printf("\nCould not execute command..");
	        	exit(0);
	    	}
	    }
	    else
	    {
	        int sts;
	        close(fd[READ_END]);
	        close(fd[WRITE_END]);
	        //wait(NULL);
	        //wait(NULL);
	        
	        waitpid(pid2, &sts, 0);
	        //exit(0);
	    }
	}
}*/

int main(){
	char inputStr[1000],*parsedList[1000];
	char* parsedStrPiped[1000]; 
	shellinit();
	int flag;
	while(1){
		if(inputcommand(inputStr))
			continue;

		flag=parseString(inputStr,parsedList,parsedStrPiped);
		if(flag==0)
			execparseString(parsedList);
		//if(flag==1)
			//execparseStringPiped(parsedStrPiped);
	}


}