#include<iostream>
#include<stdio.h>
#include<string.h> 
#include<bits/stdc++.h>
#include <fcntl.h>
#include<stdlib.h> 
#include<unistd.h> 
#include<sys/wait.h> 
#include<sys/types.h> 
#include<curses.h>
#include<termios.h>
#include <fstream>
#include <signal.h>
#include <setjmp.h>
#define clear() printf("\033[H\033[J")
using namespace std;


extern char **environ;
map<string,string> toalias;
string homepath;  ///////for cd ~
string path,home,user,hostname;
struct termios s2;
vector<string> hist;

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
		if((string)parsedList[1]=="~"){
			//cout<<"here"<<endl;
			cout<<homepath<<endl;
			chdir(homepath.c_str());
		}else{
			chdir(parsedList[1]);
		}
		return 1;
	}
	return 0;
}

int checkalias(char *p){
	////check for alias word
	string str1=p;
	int i=0;
	while(str1[i]==' '){
		i++;
	}
	string temp1;
	temp1=str1.substr(i,i+5);
	//cout<<"TEMP1 "<<temp1;
	if(temp1.compare("alias")!=0){
		return 0;
	}
	/////////////
	char *token;
	char *delimtbyequal[100];
	token = strtok (p,"=");
	int j=0;
	while (token != NULL)
  	{
    	delimtbyequal[j++]=token;
    	token = strtok (NULL, "=");
  	}
  	
	//cout<<delimtbyequal[0]<<endl;
	//cout<<delimtbyequal[1]<<endl;
	string a1=delimtbyequal[0];
	string a2=delimtbyequal[1];
	string a3=a1.substr(6,a1.length());
	string a4=a2.substr(1,a2.length()-2);
	toalias[a3]=a4;
	return 1;
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
	string s1;
	getline (cin, s1);
	hist.push_back(s1);   /////added for history command at 14:05 Sept 9
	int n = s1.length();
  	char char_array[n + 1]; 
    strcpy(char_array, s1.c_str()); 
	p=char_array;
	
	//cout<<p<<endl;


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
	//cout<<"INMAP "<<p<<endl;
}

//////////////////////////////////added on 6th sep 12:00
int checkalarm(char **parsedList){
	//cout<<"HI";
	string str1=parsedList[0];
	if(str1.compare("alarm")==0){
		return 1;
	}
	return 0;

}
static void sig_alrm(int);
static jmp_buf env_alrm;

void setalarm(char **parsedList){
	//cout<<"HI";
	if(signal(SIGALRM,sig_alrm)==SIG_ERR){
		cout<<"signal(SIGALRM) error"<<endl;
	}
	/*if(setjmp(env_alrm)!=0){
		cout<<"timeout"<<endl;
	}*/
	alarm(atoi(parsedList[1]));
	pause();
	
}
static void sig_alrm(int signo){
	cout<<"Done"<<endl;
	//longjmp(env_alrm,1);
}
////////////////////////////////////////////////////////
//////////////added 7th sept 00:55///////////////////////
int checkexport(char *p){
	string str1=p;
	string str2=str1.substr(0,6);
	if(str2.compare("export")==0){
		return 1;
	}
	return 0; 
}
int checkhistory(char *p){
	string str1=p;
	string str2=str1.substr(0,7);
	if(str2.compare("history")==0){
		return 1;
	}
	return 0;	
}

void displayhistory(){
	for(int i=hist.size()-2;i>=0;i--){
		cout<<hist[i]<<endl;
	}
}
int checkopen(char **parsedList){
	string s1=parsedList[0];
	if(s1.compare("open")==0){
		string s2=parsedList[1];
		if(s2[s2.length()-1]=='4'){
			string c="vlc";
			parsedList[0]=(char*)c.c_str();
		}else{
			string g="gedit";
			parsedList[0]=(char*)g.c_str();
		}
	}	
}

void writewhenExported(string a3,string a4){

	int lineno=0;
	if(a3.compare("PATH")==0){
		lineno=1;
		path="PATH="+a4;
	}
	else if(a3.compare("HOME")==0){
		lineno=2;
		home="HOME="+a4;
		homepath=a4;

	}else if(a3.compare("USER")==0){
		lineno=3;
		user="USER="+a4;
	}else if(a3.compare("HOSTNAME")==0){
		lineno=4;
		hostname="HOSTNAME"+a4;
	}
	ifstream filein("mybashrc"); //File to read from
    ofstream fileout("mybashrc1"); //Temporary file
    string linedata;
    int k=1;
    //cout<<lineno<<endl;
    while(lineno>0 && filein>>linedata){
    	if(k==lineno){
    		linedata=a4;
    	}
    	
    	fileout<<linedata<<endl;
    	
    	k++;
    }
    char *env_args[100];
    if(lineno>0){
    	env_args[0]=(char *)path.c_str();
    	env_args[1]=(char *)home.c_str();
    	env_args[2]=(char *)user.c_str();
    	env_args[3]=(char *)hostname.c_str();
    	memcpy(environ,env_args,sizeof(env_args));
    }
}
void exportinfile(char *p){

	char *token;
	char *delimtbyequal[100];
	token = strtok (p,"=");
	int j=0;
	while (token != NULL)
  	{
    	delimtbyequal[j++]=token;
    	token = strtok (NULL, "=");
  	}
  	string a1=delimtbyequal[0];
	string a2=delimtbyequal[1];
	string a3=a1.substr(7,a1.length());
	string a4=a2.substr(1,a2.length()-2);
	//cout<<a3<<" "<<a4<<endl;

	writewhenExported(a3,a4);

}
////////////////////////////////////////////////////////////
int parseString(char *p,char **parsedList,char **parsedStrPiped){
	char *pipedstr[100];
	char* parsedStrPiped2[1000];//////to store the second command in case of pipe
	bool ifpipe=delimitepipe(p,pipedstr);
	if(!ifpipe){
		///////////////added 14:06 9th sept
		if(checkhistory(p)){
			displayhistory();
			return 1;
		}

		//////////added 15:41
		if(checkalias(p)){
			return 1;
		}
		searchinMap(p);

		/////////added 00:55 7th sep
		if(checkexport(p)){
			exportinfile(p);
			return 1;
		}
		/////////////////////////////
		parsewithSpace(p, parsedList);
		
		//////////////////added 11:54 6thsept
		if(checkalarm(parsedList)){

			setalarm(parsedList);
			return 1;
		}
		/////////////////////////
		checkopen(parsedList);


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

		//cout<<parsedList[0]<<endl;
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

	ifstream file("mybashrc");
	if (file.is_open()) {
    	string line,s1,s2,s3,s4;
    	int i=0;
    	while (getline(file, line)) {
       		
       		if(i==0){
       			s1=line;
       		}
       		if(i==1){
       			s2=line;
       		}
       		if(i==2){
       			s3=line;	
       		}
       		if(i==3){
       			s4=line;
       		}
        	i++;
    	}
    	file.close();
    	homepath=s2; ////for cd command
    	path="PATH="+s1;
    	home="HOME="+s2;
    	user="USER="+s3;
    	hostname="HOSTNAME="+s4;
    	
    	char *env_args[]={(char *)path.c_str(),(char *)home.c_str(),(char *)user.c_str(),(char *)hostname.c_str(),(char *)"DBUS_SESSION_BUS_ADDRESS=unix:path=/run/user/1000/bus",(char *)"DISPLAY=:0",NULL};
    	/*char *env_args[100];
    	env_args[0]=(char *)path.c_str();
    	env_args[1]=(char *)home.c_str();
    	env_args[2]=(char *)user.c_str();
    	env_args[3]=(char *)hostname.c_str();

    	///for DISPLAY =:0
    	
    	string d1="DISPLAY=:0";
    	env_args[5]=(char *)d1.c_str();
    	//setenv("DISPLAY=",":0",1);
    	string x1="DBUS_SESSION_BUS_ADDRESS=unix:path=/run/user/1000/bus";
    	//setenv("DBUS_SESSION_BUS_ADDRESS=","unix:path=/run/user/1000/bus",1);
    	env_args[4]=(char *)x1.c_str();
    	env_args[6]=NULL;*/
    	//environ=env_args;
    	memcpy(environ,env_args,sizeof(env_args));
    	
    	/*for (char **env = environ; *env != 0; env++)
  		{
    		char *thisEnv = *env;
    		printf("%s\n", thisEnv);    
  		}*/

    	
	}

}
void setEnvVar(){

	//cout<<getenv("PS1");
	ofstream myfile;
  	myfile.open ("mybashrc");
  	myfile << getenv("PATH")<<endl;
  	myfile << getenv("HOME")<<endl;
  	myfile << getenv("USER")<<endl;
  	char hostbuffer[100];
  	hostbuffer[100]='\0';
   	gethostname(hostbuffer, 100); 
   	myfile << hostbuffer <<endl;
  	myfile.close();

}
int main(){
	char inputStr[1000],*parsedList[1000];
	char* parsedStrPiped[1000]; 
	shellinit();
	
	setEnvVar();
	readMyBash();
	int flag;
	int setenvflag=0;
	while(1){
		//cout<<"PID:"<<geteuid()<<endl;
		if(geteuid()==0){
			cout<<"# ";
		}else
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