#include<cstdio>
#include<cmath>

int main(){
	double a,b,c,delta,x1,x2;
	scanf("%lf%lf%lf",&a,&b,&c);
	delta=b*b-4*a*c;
	printf("delta=%.3lf\n",delta);
	if(delta<0){
		printf("莫得实数根\n");
	} else {
		x1=( sqrt(delta)-b)/(2*a);
		x2=(-sqrt(delta)-b)/(2*a);
		printf(delta==0?"x1=x2=%.3lf":"x1=%.3lf,x2=%.3lf",x1,x2);
		putchar('\n');
	}
}
