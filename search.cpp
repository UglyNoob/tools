#include<cstdio>
#include<cstdlib>
#include<cstring>
#include<unistd.h>
#include<dirent.h>

char *dirname,*text,*buffer;
struct dirent *filename;
DIR *dir;
int len;

void search(){
	FILE *file=fopen(filename->d_name,"r");
	if(file==NULL){
		printf("Could not open file %s\n",filename->d_name);
		return;
	}
	int index,line=0;
	char c;
	bool notend=true;
	bool title=false;
	while(notend){
		line++;
		index=0;
		while((c=fgetc(file))!='\n'){
			if(c==(char)EOF){
				notend=false;
				break;
			}
			buffer[index]=c;
			index++;
		}
		buffer[index]='\0';
		if(index<len)continue;
		bool found=false;
		int level=0,last=0;
		for(int i=0;i<index;i++){
			if(text[level]==buffer[i])level++;
			else level=0;
			if(level==len){
				if(!title){
					printf("\n%s:\n",filename->d_name);
					title=true;
				}
				found=true;
				int j;
				if(last==0)printf("%d: ",line);
				for(j=last;j<=(i-len);j++)putchar(buffer[j]);
				printf("\033[91m\033[1m");	//change the text colour into red
				for(;j<=i;j++)putchar(buffer[j]);
				printf("\033[0m");	//change back the text colour
				last=j+1;
			}
		}
		if(found){
			for(last--;last<index;last++){
				putchar(buffer[last]);
			}
			putchar('\n');
		}
	}
}

int main(int argc,char **argv){
	if(argc!=2){
		printf("Usage: %s [text to search]\n",argv[0]);
		return 1;
	}

	text=argv[1];
	dirname=getcwd(NULL,0);
	buffer=new char[1048577];
	len=strlen(text);

	if(dirname==NULL){
		printf("Error, could not get the working directory\n");
		return 2;
	}
	dir=opendir(dirname);
	if(dir==NULL){
		printf("Can't open %s",dirname);
	}
	printf("Working at %s, searching for %s\n",dirname,text);
	while(filename=readdir(dir)){
		if(!strcmp(filename->d_name,".")||!strcmp(filename->d_name,".."))continue;
		search();
	}
}
