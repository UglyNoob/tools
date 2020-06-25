#include<cstdio>
#include<cmath>

int p,q,r,B;
char a;

bool detect(int p,int q,int r){
	int P=0,Q=0,R=0,time=0;
	while(p){
		if(p%10>=B)return false;
		P+=(p%10)*(int)pow(B,time);
		p/=10;
		time++;
	}
	time=0;
	while(q){
		if(q%10>=B)return false;
		Q+=(q%10)*(int)pow(B,time);
		q/=10;
		time++;
	}
	time=0;
	while(r){
		if(r%10>=B)return false;
		R+=(r%10)*(int)pow(B,time);
		r/=10;
		time++;
	}
	if(P*Q==R)return true;
	return false;
}

int main(){
	scanf("%d %d %d",&p,&q,&r);
	for(B=2;B<=16;B++){
		if(detect(p,q,r)){
			printf("%d\n",B);
			return 0;
		}
	}
	printf("0\n");
	return 0;
}
