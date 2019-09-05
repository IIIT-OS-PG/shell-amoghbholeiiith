#include<iostream>
#include<string.h> 
#include<bits/stdc++.h>
#include <fcntl.h>
#include<stdlib.h> 
#include<unistd.h> 
#include<sys/wait.h> 
#include<sys/types.h> 
#include <curses.h>
#include <fstream>
//#include<readline/history.h> 
//#include<readline/readline.h> 
#define clear() printf("\033[H\033[J")
#define WRITE_END 1
#define READ_END 0

using namespace std;

char *env[6]; //env variable containing path
extern char **environ;
map<string,string> toalias;
string inputstr;

void searchinMap(char *);
void execparseStringPiped(char **,char **);
void execparsePiped(char **);
void createfileCopy(char **parsedList,int i){
	int fdfilecopy;
	char *path=parsedList[i+1];
	//cout<<"HI";
	fdfilecopy = open(path, O_CREAT | O_TRUNC| O_WRONLY,0777);
	dup2(fdfilecopy,1);
	close(fdfilecopy);
	parsedList[i]=NULL;
}
void appendinfile(char **parsedList,int i){
	int fdfilecopy;
	char *path=parsedList[i+1];
	//cout<<"HI";
	fdfilecopy = open(path, O_CREAT | O_APPEND| O_WRONLY,0777);
	dup2(fdfilecopy,1);
	close(fdfilecopy);
	parsedList[i]=NULL;
}

int myCDcommand(char **parsedList){
	string a="cd";
	string s1=parsedList[0];
	if(a.compare(s1)==0){
		chdir(parsedList[1]);
		return 1;
	}
	return 0;
}
int checkalias(char *p){
	char *token;
	char *delimtbyequal[100];
	token = strtok (p,"=");
	int j=0;
	while (token != NULL)
  	{
    	delimtbyequal[j++]=token;
    	token = strtok (NULL, "=");
  	}
  	if(j>1){
  		cout<<delimtbyequal[0]<<endl;
  		cout<<delimtbyequal[1]<<endl;
  		string a1=delimtbyequal[0];
  		string a2=delimtbyequal[1];
  		string a3=a1.substr(6,a1.length());
  		string a4=a2.substr(1,a2.length()-2);
  		toalias[a3]=a4;
		return 1;
  	}
	else 
		return 0;
}

void exceptionCommands(char **parsedList){
	int i=0;
	int count=0;
	searchinMap(parsedList[0]);
	while(parsedList[i]!=NULL){
		string temp_s=parsedList[i];
		if(temp_s.compare(">")==0){
			createfileCopy(parsedList,i);
			break;
		}
		if(temp_s.compare(">>")==0){
			appendinfile(parsedList,i);
			break;
		}
		i++;
	}
}

int inputcommand(char *str){
	char *p;
	//p = readline("\n$ ");
	string s1;
	getline (cin, s1);
	inputstr=s1;
	int n = s1.length(); 
  	char char_array[n + 1]; 
    strcpy(char_array, s1.c_str()); 
	p=char_array;
	if (strlen(p) != 0) {
		//add_history(p); 
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

void searchinMap(char *p){
	string s1=p;
	char *q;
	if(toalias.find(s1)!=toalias.end()){
		//cout<<"HI"<<endl;
		string temp=toalias[s1];
		//cout<<"Inside"<<temp<<endl;
		
		int n = temp.length(); 
			char char_array[n + 1]; 
		strcpy(char_array, temp.c_str()); 
		q=char_array;
		strcpy(p, q);
	}
	cout<<"INMAP "<<p<<endl;
}

int parseString(char *p,char **parsedList,char **parsedStrPiped){
	char *pipedstr[100];
	char* parsedStrPiped2[1000];//////to store the second command in case of pipe
	bool ifpipe=delimitepipe(p,pipedstr);
	if(!ifpipe){
		//////////added 15:41
		if(checkalias(p)){
			return 1;
		}
		searchinMap(p);
		parsewithSpace(p, parsedList);
		
		return 0;
		//return exceptionCommands(parsedList);
	}else{
		execparsePiped(pipedstr);
		return 1;
	}
}
void execparseString(char **parsedList){
	pid_t pid = fork(); 
	if (pid == -1) { 
		printf("\nFailed forking child.."); 
		return; 
	} else if (pid == 0) {
		///to check for >> or >
		exceptionCommands(parsedList);
		//write(0,parsedList[0],10);

		cout<<parsedList[0]<<endl;
		if (execvp(parsedList[0], parsedList) < 0) { 
			cout << "Command not executed !!!" << endl;
		}
		//close(fdfilecopy); 
		exit(0); 
	} else {
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
			/////check if the command is not >> or > or cd
			exceptionCommands(processed_input2);
			//cout<<"Here"<<processed_input2[0]<<endl;
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
void readMyBash(){

	ifstream file("mybashrc.txt");
	if (file.is_open()) {
    	string line,s1,s2,s3;
    	int i=0;
    	while (getline(file, line)) {
       		
       		//int n = line.length(); 
  			//char char_array[n + 1]; 
    		//strcpy(char_array, line.c_str()); 
    		//char *p=char_array;
    		if(i==0){
       			//setenv("PATH",p,1);
    			s1=line;
       		}
       		if(i==1){
       			//setenv("HOME",p,1);
       			s2=line;
       		}
       		if(i==2){
       			//setenv("USER",p,1);
       			s3=line;	
       		}
    		/*env[i]=p;
    		if(i==1)
    			strcpy(environ[44],line.c_str());*/
        	//cout<<p<<endl;
        	i++;
    	}
    	string path="PATH="+s1;
    	string home="HOME="+s2;
    	string user="USER="+s3;
    	
    	//char *env_args[]={(char *)path.c_str(),(char *)home.c_str(),(char *)user.c_str(),NULL};
    	//cout<<env_args[0]<<endl;
    	//environ=env_args;
    	
    	/*for (char **env = environ; *env != 0; env++)
  		{
    		char *thisEnv = *env;
    		printf("%s\n", thisEnv);    
  		}*/

    	file.close();
	}

}
void setEnvVar(){

	//cout<<getenv("PS1");
	ofstream myfile;
  	myfile.open ("mybashrc.txt");
  	myfile << getenv("PATH")<<endl;
  	myfile << getenv("HOME")<<endl;
  	myfile << getenv("USER")<<endl;
  	char hostbuffer[100];
   	int hostname = gethostname(hostbuffer, sizeof(hostbuffer)); 
   	//myfile >> hostbuffer <<endl;
  	myfile.close();

}
int main(){
	char inputStr[1000],*parsedList[1000];
	char* parsedStrPiped[1000]; 
	shellinit();
	setEnvVar();
	int flag;
	int setenvflag=0;
	while(1){
		
		if(setenvflag==0){
			readMyBash();
			setenvflag=1;
		}
		cout<<"$ ";
		if(inputcommand(inputStr))
			continue;

		flag=parseString(inputStr,parsedList,parsedStrPiped);
		if(flag==0){
			int a=myCDcommand(parsedList);
			if(a==0)
				execparseString(parsedList);
		}
	}


}