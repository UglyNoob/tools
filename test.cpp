#include<cstdio>
#include<cmath>
int main(){
	int x,n;
	double re;
	scanf("%d%d",&x,&n);
	re=x;
	for(int i=0;i<n;i++)re*=1.001;
	printf("%.4lf",re);
}
