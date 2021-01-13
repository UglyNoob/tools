#include<cstdio>
#include<cmath>
int main(){
	int n,result=1,temp=1;
	scanf("%d",&n);
	for(int i=2;i<=n;i++){
		temp*=i;
		result+=temp;
	}
	printf("%d\n",result);
}