#include<cstdio>
#include<unistd.h>
#include<dirent.h>
#include<cstring>
#include<sys/stat.h>
void scan_one_dir( const char * dir_name) {
	if( NULL == dir_name ) {
		printf(" dir_name is null ! \n");
		return;
	}
	struct stat s;
	lstat( dir_name , &s );
	if( ! S_ISDIR( s.st_mode ))return;
	struct dirent * filename;
	DIR * dir;
	dir = opendir( dir_name );
	if( NULL == dir )return;
	int iName=0;
	while( ( filename = readdir(dir) ) != NULL ) {
		if(	strcmp( filename->d_name , "." ) == 0 || 
			strcmp( filename->d_name , "..") == 0) continue;
		char wholePath[128] = {0};
		sprintf(wholePath, "%s/%s", dir_name, filename->d_name);
		printf("wholePath= %s\n",wholePath);
	}
}

int main(){
	scan_one_dir("/root");
}
