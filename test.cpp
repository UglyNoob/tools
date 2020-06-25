#include<cstdio>

int main(){
	//printf("%d",EOF);
	FILE *f=fopen("b","r");
	char c;
	while((c=fgetc(f))!=EOF){
		printf("%d\n",c);
	}
}
