#include<fcntl.h>
#include<math.h>
#include<pthread.h>
#include<signal.h>
#include<stdint.h>
#include<stdio.h>
#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<unistd.h>
#include<stdbool.h>

#define SUM_TYPE uint64_t
#define BLOCK_SIZE 8192
#define MAX_THREADS 4
#define min(a,b) ((a<b)?(a):(b))
#define HELPMSG "usage: summer [-jN] [-vh] [--help] [FILENAME]"

// Data structure to pass arguments to thread
typedef struct FileInfo
{
	SUM_TYPE sum; // Store value
	bool quiet; // Print to stdout?
	char*file_name; // File name
	size_t offset; // Where to start reading (in bytes)
	size_t size; // How many bytes to read
	size_t thread_id; // To tell them apart
} FileInfo;

// Thread to handle part of file
void*t(void*f)
{
	FileInfo fi=*(FileInfo*)f;
	int fd=0; // Open stdin by default
	size_t pos=fi.offset;

	if(fi.file_name) // Open file if specified
		fd=open(fi.file_name,O_RDONLY);
	if(fd<0)
		fprintf(stdout,"error: failed to open file '%s'\n",fi.file_name);

	if(!fi.quiet)
		printf("thread #%lu\n",fi.thread_id);
	lseek(fd,fi.offset,SEEK_SET);
	while(pos<fi.offset+fi.size)
	{
		char buf[BLOCK_SIZE];
		size_t nr=0;
		if(!(nr=read(fd,buf,min(fi.size,BLOCK_SIZE))))
			break;
		for(size_t i=0;i<nr;++i)
			fi.sum+=buf[i];
		pos+=nr;
	}
	if(!fi.quiet)
		printf("Thread #%lu sum: %lu\n",fi.thread_id,fi.sum);

	FileInfo*y=(FileInfo*)f;
	y->sum=fi.sum;
	return NULL;
}

// Treat n as number of bytes,
// return string in human readable form
char*human_number(SUM_TYPE n)
{
	static char s[128]="";

	if(n>=1000000000000)
		sprintf(s,"%.1fT",n/1000000000000.0);
	else if(n>=1000000000)
		sprintf(s,"%.1fG",n/1000000000.0);
	else if(n>=1000000)
		sprintf(s,"%.1fM",n/1000000.0);
	else if(n>=1000)
		sprintf(s,"%.1fK",n/1000.0);
	else
		sprintf(s,"%lu",n);
	return s;
}

// Entry point
int main(int argc,char**argv)
{
	FileInfo fi[MAX_THREADS];
	bool human = false;
	bool quiet = true;
	char*file_name=NULL;
	pthread_t thread_pool[MAX_THREADS];
	size_t bytes_per_thread=0;
	size_t file_size=0;
	size_t nthreads=MAX_THREADS;
	uint32_t nproc=1;


	nproc=sysconf(_SC_NPROCESSORS_ONLN);

	// Parse command line options
	// (short options, long options, filename)
	for(size_t i=1;i<argc;++i)
	{
		if(argv[i][0]=='-')
		{
			for(size_t j=1;argv[i][j];++j)
				switch(argv[i][j])
				{
					// Long option
					case '-':
						if(strcmp("--help",argv[i])==0)
						{
							printf("%s\n",HELPMSG);
							exit(0);
						}
						break;

					case 'v':
						quiet=false;
						break;

					case 'h':
						human=true;
						break;

					case 'j':
						if(!quiet)
							printf("setting nproc to '%s'\n",argv[i]+j+1);
						nproc=atoi(argv[i]+j+1);
						//++i;
						break;

					default:
				}
		}

		else
		{
			if(!quiet)
				printf("filename '%s'\n",argv[i]);
			file_name=argv[i];
		}
	}

	// Check if file exists, get file size
	{
		int fd=0;
		if(file_name)
			fd=open(file_name,O_RDONLY);
		if(fd<0)
		{
			fprintf(stdout,"error: failed to open file '%s'\n",file_name);
			exit(1);
		}
		file_size=lseek(fd,0,SEEK_END);
		close(fd);
	}

	// Get # of processors for machine
	nthreads=nproc;
	bytes_per_thread=ceil((double)file_size/nthreads);
	if(!quiet)
	{
		printf("File Size: %lu\n",file_size);
		printf("Number of threads: %lu\n",nthreads);
		printf("Bytes to Read Per Thread: %lu\n",bytes_per_thread);
		printf("___\n");
	}

	if(file_name==NULL)
	{
		//fprintf(stderr,"error: stdin not implemented yet, sorry\n");
		printf("%s\n",HELPMSG);
		exit(1);
	}

	// Divide file into nthreads parts, open threads for each part
	for(size_t i=0;i<nthreads;++i)
	{
		if(!quiet)
			printf("Creating thread #%lu: ",i);
		size_t size=(i*bytes_per_thread+bytes_per_thread<file_size)?
			(bytes_per_thread):
			(file_size-i*bytes_per_thread);

		fi[i]=(FileInfo){
			.quiet=quiet,
			.thread_id=i,
			.file_name=file_name,
			.offset=i*bytes_per_thread,
			.size=size
		};

		if(!quiet)
			printf("Reading %lu starting at %lu\n",fi[i].size,fi[i].offset);
		pthread_create(thread_pool+i,NULL,t,fi+i);
	}

	if(!quiet)
		printf("main\n");
	SUM_TYPE sum=0;
	for(size_t i=0;i<nthreads;++i)
	{
		pthread_join(thread_pool[i],NULL);
		sum+=fi[i].sum;
	}

	if(!quiet)
		printf("___\n");
	if(human)
		printf("%s\t%s\n",human_number(sum),file_name);
	else
		printf("%lu\t%s\n",sum,file_name);
}
