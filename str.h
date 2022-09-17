#pragma once
#define STR_DEFSIZE 32

typedef struct Str
{
	char*b;   // Buffer
	size_t n; // Size
	size_t c; // Capacity
}Str;

Str str_new(char*s);
void str_free(Str*s);
void str_grow(Str*s,size_t n);

Str str_new(char*s)
{
	Str str={.b=NULL,.n=0,.c=0};
	if(!s)return str;
	if(str.c<strlen(s))
		str_grow(&str,strlen(s));
	if(!str.b)
		return str;
	memcpy(str.b,s,strlen(s)+1);
	return str;
}

void str_free(Str*s)
{
	if(s->b)
		free(s->b);
	s->c=0;
	s->n=0;
	s->b=NULL;
}

void str_grow(Str*s,size_t n)
{
	if(!s->b)
		s->b=malloc(n+1);
	if(!s->b)
		return;
	s->c=n;
	s->b[n]=0;
}
