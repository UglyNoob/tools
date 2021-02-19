#include<cstdio>

int main(){
	for(int i=0;i<127;i++){
		printf("%d:%c\t",i,(char)i);
	}
	putchar('\n');
	return 0;
}
