#include<cstdio>
#include<cmath>

int p,q,r,B;
char a;

bool detect(){
	int P=0,Q=0,R=0,time=0;
	while(){
		P+=(p%10)*pow(B,time);
		p/=10;
		time++;
	}
}

int main(){
	scanf("%d %d %d",&p,&q,&r);
	for(B=2;B<=16;B++){
		if(detect()){
			printf("%d\n",B);
			return 0;
		}
	}
	printf("0\n");
	return 0;
}
