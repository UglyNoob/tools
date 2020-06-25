#include<cstdio>
#include<conio.h>

int main(){
	while(true){
		char c=getch();
		printf("%d:\t%c\n",c,32<c&&127>c?c:' ');
	}
}
