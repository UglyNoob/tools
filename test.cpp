#include<cstdio>
#include<cmath>
int main(){
	int k,a=1,b=1;
	scanf("%d",&k);
	if(k<3){puts("1");return 0;}
	for(int i=k;i>2;i--){
		k=b;b=a+b;a=k;
	}
	printf("%d\n",b);
}
