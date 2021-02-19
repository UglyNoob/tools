#include<cstdio>
#include<conio.h>
#include<cstring>

int main(int argc,char **argv){
	if(argc!=1){
		if(!strcmp(argv[1],"true")){
			for(int i=32;i<128;i++){
				printf("%d:\t%c\n",i,(char)i);
			}
		}
	}
	while(true){
		char c=getch();
		printf("%d:\t%c\n",c,32<c&&127>c?c:' ');
	}
}
