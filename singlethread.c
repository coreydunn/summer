#include<stdlib.h>
#include<stdio.h>
#include<unistd.h>
#include<string.h>
#include<stdint.h>
#include<fcntl.h>
#include<pthread.h>

#define SUM_TYPE uint64_t
#define BLOCK_SIZE 8192

int main(int argc,char**argv)
{
	SUM_TYPE hash=0;
	char buf[BLOCK_SIZE];
	int fd=0;
	size_t chars=0;
	size_t nr=0;

	for(size_t fi=1;fi<argc;++fi)
	{
		if(argc>1)
			fd=open(argv[fi],O_RDONLY);
		if(fd<0)
		{
			write(2,"error: file\n",12);
			exit(1);
		}

		while((nr=read(fd,buf,BLOCK_SIZE)))
		{
			for(size_t i=0;i<nr;++i)
				hash+=buf[i];
			chars+=nr;
			//hash+=b;
		}

		//printf("total read: %lu\n",chars);
		printf("%lu\t%s\n",hash,argv[fi]);
		//write(1,&hash,sizeof(hash));

		if(fd!=0)
			close(fd);
	}
}
