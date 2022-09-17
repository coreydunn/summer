#pragma once
#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include"str.h"

#define VEC_DEFSIZE 128

typedef struct Vec
{
	size_t n;
	size_t c;
	Str*b;
}Vec;

Vec vec_new(void)
{
	Vec v={0};
	return v;
}

void vec_grow(Vec*v)
{
	Str*tmp;

	if(!v)return;
	if(!v->b)
	{
		v->b=malloc(sizeof(Str)*VEC_DEFSIZE);
		if(!v->b)
		{
			v->c=0;
			v->n=0;
			return;
		}
		v->c=VEC_DEFSIZE;
	}

	tmp=realloc(v->b,sizeof(Str)*(v->c+VEC_DEFSIZE));
	if(!tmp)
	{
		v->c=0;
		v->n=0;
		v->b=NULL;
	}
	v->b=tmp;
	v->c+=VEC_DEFSIZE;
}

void vec_push(Vec*v,char*s)
{
	Str str;

	if(!v)return;
	if(!s)return;
	if(!v->b||v->n+1>v->c)
		vec_grow(v);
	if(!v->b)return;
	str=str_new(s);
	if(!str.b)return;
	v->b[v->n]=str;
	++v->n;
}

void vec_free(Vec*v)
{
	if(!v)return;

	if(v->b)
	{
		for(size_t i=0;i<v->n;++i)
			str_free(v->b+i);
		free(v->b);
	}
	v->c=0;
	v->c=0;
	v->b=NULL;
}
