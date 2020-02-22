#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
typedef char* Tinfo;
#define inf 100000000 /*vom folosi inf pentru returnarea valorilor in functii recursive*/
 typedef struct nod
 {
 	Tinfo info;
 	struct nod *st,*dr;
 }TNod,*TArb,**AArb;
/*functie care construiese un nod cu informatia x*/
TArb ConstrFr( char *x)
{
	TArb a;
	a=(TArb)malloc(sizeof(TNod));
	if(!a)
		return NULL;
	a->info=malloc(10*sizeof(char));
	if(!a->info)
	{
		free(a);
		return NULL;
	}
	strcpy(a->info,x);
	a->st=NULL;
	a->dr=NULL;
	return a;
}
void distruge(TArb a)
{
	if(!a)
		return ;
	distruge(a->st);
	distruge(a->dr);
	free(a->info);
	free(a);
}
/*functie care elibereaza un arbore din memorie*/
void DistrugeArb(TArb *a)
{
	if(!*a)
		return ;
	distruge(*a);
	*a=NULL;
}
/*functie care construieste arborele de expresie*/
int ConstrArb(TArb *a,char * x)
{

	if(x == NULL)/*condirie de oprire*/
		return 0;
	char delims[]="= \n";
	TArb aux=ConstrFr(x);/*construim un nod nou*/
	if(!aux)
		return -1;
	if( *a == NULL)
		*a=aux;
	/*conditie ca un nod sa fie frunza si conditie de oprire*/
	if( strcmp(x,"+")!=0 && strcmp(x,"-")!=0 && strcmp(x,"*")!=0 
		&& strcmp(x,"/")!=0 &&strcmp(x,"^")!=0 && strcmp(x,"sqrt")!=0 )
		return 0;
	/*tratam cazul in care operatia este radicalul*/
	if(strcmp(x,"sqrt")==0)
	{
		ConstrArb(&((*a)->st),strtok(NULL,delims));/*construim subarborele doar in fiul stang*/
		return 0;
	}
	else
	{/*construim arborele prin recursivitate construind subarborele stang si respectiv drept*/
		ConstrArb(&((*a)->st),strtok(NULL,delims));
		ConstrArb(&((*a)->dr),strtok(NULL,delims));
		return 0;
	}

}
/*functie care ferifica daca o variabila este sau nu in vectorul de variabile*/
int verif(char *a , char val_char[][10],int n)
{
	int i,ok=0;
	for(i=0;i<n;i++)
	{
		if(strcmp(a,val_char[i])==0)
			ok=1;
	}
	return ok;
}
/*functie care verifica daca un arbore are o variabila nedeclarata*/
int verif_variabila(TArb a,char val_char[][10],int n,FILE* f,int *ok)
{
	if(!a)
		return 0;
	/*verificam daca este prima intrare in functie cu ok 
	si verificam daca nodul e frunza si contine variabile nedeclarate*/
	if(*ok==0 && a->st== NULL && a->dr == NULL &&  verif(a->info,val_char,n) == 0 &&
		*(a->info)!=45 && !(*(a->info) >= 48 && *(a->info) <= 57) )
	{
		fprintf(f,"Variable %s undeclared\n",a->info );
		*ok=1;
		return 0;
	}
	/*cautam in continuare in fiii nodului curent*/
	verif_variabila(a->st,val_char,n,f,ok);
	verif_variabila(a->dr,val_char,n,f,ok);
	return 0;
}
/*functie care calculeaza valoarea expresiei dintr-un arbore*/
int evaluare_arbore(TArb a,FILE* f,char val_char[][10],int* val_int,int nr_var,int *ok)
{	/*conditii de oprire*/
	if(!a)
		return inf;
		
	if(a->st == NULL && a->dr == NULL)
	{	/*verificam dacainformatia este un numar*/
		if(*(a->info) >= 48 && *(a->info) <= 57)
			return atoi(a->info);
		int i;
		
		/* cautam valoarea variabilei din a in vectorul de variabile*/
		for(i=0;i<nr_var;i++)
		{	
			if(strcmp(a->info,val_char[i])==0)
				break;
		}
		
		return val_int[i];
	}
	/*calulam recursiv fiul drept si stang si tratam fiecare operatie in parte*/
	int fiu_stanga=evaluare_arbore(a->st,f,val_char,val_int,nr_var,ok);
	int fiu_dreapta=evaluare_arbore(a->dr,f,val_char,val_int,nr_var,ok);
	
	if(strcmp(a->info,"+")==0)
		return fiu_stanga + fiu_dreapta ;
	if(strcmp(a->info,"-")==0)
		return fiu_stanga - fiu_dreapta ;
	if(strcmp(a->info,"*")==0)
		return fiu_stanga * fiu_dreapta ;
	if(*ok==0 && strcmp(a->info,"/")==0)
	{	/*cazl in care se imparte la 0*/
		if(fiu_dreapta==0)
		{
			fprintf(f, "Invalid expression\n");
			*ok=1;
			return inf;
		}
		return fiu_stanga / fiu_dreapta ;
	}
	if(strcmp(a->info,"^")==0)
		return pow(fiu_stanga,fiu_dreapta) ;

	if(*ok ==0 &&strcmp(a->info,"sqrt")==0)
	{	/*cazul in care scoatem radical dintr-un numar negativ*/
		if(fiu_stanga < 0)
		{
			fprintf(f, "Invalid expression\n");
			*ok=1;
			return inf;
		}
		return sqrt(fiu_stanga);
	}
	return inf;
}

 int main(int argc, char const *argv[])
 {
 	/*deschidem fisierle necesare*/
	FILE *file_in=fopen(argv[1],"r");
	if(!file_in)
	{
		printf("Nu s-a putut deschide fisierul\n");
		return -1;
	}

	FILE *file_out=fopen(argv[2],"w");
	if(!file_out)
	{
		printf("Nu s-a putut deschide fisierul\n");
		return -1;
	}
	int n,i;
	TArb a=NULL;
	size_t bufsize=32;
	char delims[]="= \n";
	char *buffer=malloc(bufsize);
	getline(&buffer,&bufsize,file_in);/*citim numarul de variabile*/
	n=atoi(buffer);
	char var[n][10];/*vector ce contine numele fiecarei variabile*/
	int  val[n];/*vector in care salvam valoarea fiecarei variabile*/
	
	for(i=0;i<n;i++)
	{/*formam vectorul de variabile si pe cel de valori*/
		getline(&buffer,&bufsize,file_in);
		char* variable=strtok(buffer,delims);
		strcpy(var[i],variable);
		int value=atoi(strtok(NULL,delims));
		val[i]=value;

	}
	getline(&buffer,&bufsize,file_in);/* citim numarul de operatii*/
	int nr_op=atoi(buffer);
	for(i=1;i<=nr_op;i++)
	{
		int ok1=0,ok2=0;/*variabile care vor fi 0 daca nu avem variabile nedeclarate,respectiv operatii invalide*/
		getline(&buffer,&bufsize,file_in);
		char* op=strtok(buffer,delims);
		ConstrArb(&a,op);
		verif_variabila(a,var,n,file_out,&ok1);/*verificam daca avem variabile nedeclarate*/
		if(ok1 ==1 )
			DistrugeArb(&a);/*distrugem arborele in caz afirmativ*/
		else
		{
		int x=evaluare_arbore(a,file_out,var,val,n,&ok2);/*calculam valoarea expresiei din arbore*/
		if(x!=inf && ok2==0)/*afisam rezultatul daca arborele nu contine operatii invalide 
		si daca a returnat un rezultat valid*/
			fprintf(file_out,"%d\n",x );
		DistrugeArb(&a);/*distrugem arborele*/
		}
	}
	/*eliberam buffer-ul si inchidem fisierele*/
	free(buffer);
	fclose(file_in);
	fclose(file_out);
 	return 0;
 }