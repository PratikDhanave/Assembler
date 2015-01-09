//Assembler Pass 1

#include<stdio.h>
#include<conio.h>
#include<stdlib.h>
#include<string.h>

typedef struct strMOT
{
	char instruction[10],code[3],type[3];
	int size;
}strMOT;

typedef struct strSYM
{
	char symbol[10];
	int address;
}strSYM;

typedef struct strLIT
{
	char literal[6];
	int address,poolID;
}strLIT;

typedef struct strIC
{
	int LC;
	char insType[3],insCode[3],op1Type[3],op1Code[3],op2Type[3],op2Code[5];
}strIC;


char MOFileLoc[] = "E:\\sdl\\assem\\MOT.txt";
char ASFileLoc[] = "E:\\sdl\\assem\\Assembly.txt";
char ICFileLoc[] = "E:\\sdl\\assem\\InterCode.txt";
char SYFileLoc[] = "E:\\sdl\\assem\\STable.txt";
char LTFileLoc[] = "E:\\sdl\\assem\\LTable.txt";

int poolID=0;

int readMOT(strMOT []);
void showMOT(strMOT [], int);
int insertSym(strSYM [], char [], int);
int insertLit(strLIT [], char [], int);
strIC makeBlank(strIC);
void writeIC(strIC [], int);
void writeSYM(strSYM [], int);
void writeLIT(strLIT [], int);
int getAddress(strSYM [], char  [], int);

int main()
{
	strMOT MOTable[30];
	strIC InterCode[50];
	strSYM SymTable[50];
	strLIT litTable[30];
	int noMO=0,noSYM=0,noLIT=0,LC=0,LCIncr,i=0,j;
	FILE *ASFile;
	char label[10],instruction[10],op1[10],op2[10];

	clrscr();

	noMO = readMOT(MOTable);
	printf("Instructions in MOT : %d\n",noMO);

	if((ASFile=fopen(ASFileLoc,"r"))==NULL)
	{
		printf("Error opening ASM File.");
		getchar();
		return 1;
	}

	while(fscanf(ASFile,"%s\t%s\t%s\t%s",label,instruction,op1,op2) != EOF)
	{
		printf("\n%s\t%s\t%s\t%s",label,instruction,op1,op2);

		if(strcmp(instruction,"START") == 0)
		{
			LC = atoi(op2);
			continue;
		}
		else if(i == 0)
			LC=1;

		if(strcmp(label,"-") != 0 && strcmp(instruction,"EQU") != 0)
		{
			insertSym(SymTable,label,LC);
			noSYM++;
		}

		if(strcmp(instruction,"END") == 0)
			break;
		else if(strcmp(instruction,"ORIGIN") == 0)
		{
			LC = atoi(op2);
			continue;
		}
		else if(strcmp(instruction,"LTORG") == 0)
		{
			poolID++;

			for(j=0;j<noLIT;j++)
			{
				if(litTable[j].address == 0)
				{
					InterCode[i].LC = LC;
					InterCode[i] = makeBlank(InterCode[i]);
					insertLit(litTable,litTable[j].literal,LC);
					strcpy(InterCode[i].op2Type,"LT");
					strcpy(InterCode[i].op2Code,litTable[j].literal);
					i++;
					LC++;
				}
			}
			continue;
		}
		else if(strcmp(instruction,"EQU") == 0)
		{
			insertSym(SymTable,label,getAddress(SymTable,op2,noSYM));
			noSYM++;
			continue;
		}
		else if(strcmp(instruction,"STOP")==0)
		{
			poolID++;
			for(j=0;j<noLIT;j++)
			{
				if(litTable[j].address == 0)
				{
					InterCode[i].LC = LC;
					InterCode[i] = makeBlank(InterCode[i]);
					insertLit(litTable,litTable[j].literal,LC);
					strcpy(InterCode[i].op2Type,"LT");
					strcpy(InterCode[i].op2Code,litTable[j].literal);
					i++;
					LC++;
				}                   //repeated code
			}
		}

		for(j=0;j<noMO;j++)
		{
			if(strcmp(MOTable[j].instruction,instruction) == 0)
			{
				if(strcmp(instruction,"DS")==0)
					LCIncr = atoi(op2);
				else
					LCIncr = MOTable[j].size;
				InterCode[i].LC = LC;
				InterCode[i] = makeBlank(InterCode[i]);
				strcpy(InterCode[i].insType,MOTable[j].type);
				strcpy(InterCode[i].insCode,MOTable[j].code);
			}

			if(strcmp(MOTable[j].instruction,op1) == 0)
			{
				strcpy(InterCode[i].op1Type,MOTable[j].type);
				strcpy(InterCode[i].op1Code,MOTable[j].code);
			}
		}

		if(op2[0]  == '\'')
		{
			strcpy(InterCode[i].op2Type,"CN");
			strcpy(InterCode[i].op2Code,op2);
		}
		else if(op2[0] == '=')
		{
			char *tempstr = (char *)malloc(sizeof(char)*6);

			j = insertLit(litTable,op2,0);
			itoa(j,tempstr,10);
			strcpy(InterCode[i].op2Type,"LT");
			strcpy(InterCode[i].op2Code,tempstr);
			noLIT++;
		}
		else if(op2[0] >= 'A' && op2[0] <= 'Z')
		{
			char *tempstr = (char *)malloc(sizeof(char)*5);

			j = insertSym(SymTable,op2,0);
			itoa(j,tempstr,10);
			strcpy(InterCode[i].op2Type,"SM");
			strcpy(InterCode[i].op2Code,tempstr);
		}

		LC = LC + LCIncr;
		i++;
	}

	printf("\n\n\n");
	for(j=0;j<i;j++)
		printf("\n%d\t%s,%s\t%s,%s\t%s,%s",InterCode[j].LC,InterCode[j].insType,InterCode[j].insCode,InterCode[j].op1Type,InterCode[j].op1Code,InterCode[j].op2Type,InterCode[j].op2Code);

	printf("\n\nNo. of symbols: %d",noSYM);
	printf("\n\nNo. of literals: %d",noLIT);

	writeIC(InterCode,i);
	writeSYM(SymTable,noSYM);
	writeLIT(litTable,noLIT);

	getchar();
	return 0;
}

int readMOT(strMOT MOTable[])
{
	FILE *fp;
	char tmpIns[10],tmpCode[3],tmpType[3],tmpSize[4];
	int i=0;

	if((fp=fopen(MOFileLoc,"r"))==NULL)
	{
		printf("Error opening the file.");
		return 0;
	}
	else
	{
		while(fscanf(fp,"%s %s %s %s",tmpIns,tmpCode,tmpType,tmpSize) != EOF)
		{
			strcpy(MOTable[i].instruction,tmpIns);
			strcpy(MOTable[i].code,tmpCode);
			strcpy(MOTable[i].type,tmpType);
			MOTable[i].size = atoi(tmpSize);
			i++;
		}
		return i;
	}
}

void showMOT(strMOT MOTable[], int n)
{
	int i;

	for(i=0;i<n;i++)
		printf("%s\t%s\t%s\n",MOTable[i].instruction,MOTable[i].code,MOTable[i].type);
}

int insertSym(strSYM symTable[], char name[], int line)
{
	static int i=0,j;

	for(j=0;j<i;j++)
	{
		if(strcmp(symTable[j].symbol,name)==0)
		{
			if(symTable[j].address==0)
			{
				symTable[j].address = line;
				return j;
			}
			else
				return j;
		}
	}

	strcpy(symTable[i].symbol,name);
	symTable[i].address = line;

	i++;

	return (i-1);
}

int insertLit(strLIT litTable[], char literal[], int line)
{
	static int i=0;
	int j;

	for(j=0;j<i;j++)
	{
		if(strcmp(litTable[j].literal,literal)==0)
		{
			if(litTable[j].address==0)
			{
				litTable[j].address = line;
				litTable[j].poolID = poolID;
				return j;
			}
			else
				return j;
		}
	}

	strcpy(litTable[i].literal,literal);
	litTable[i].address = line;
	litTable[i].poolID = poolID;

	i++;

	return(i-1);
}

strIC makeBlank(strIC InterCode)
{
	strcpy(InterCode.insType,"-");
	strcpy(InterCode.insCode,"-");
	strcpy(InterCode.op1Type,"-");
	strcpy(InterCode.op1Code,"-");
	strcpy(InterCode.op2Type,"-");
	strcpy(InterCode.op2Code,"-");

	return InterCode;
}

void writeIC(strIC InterCode[], int num)
{
	FILE *fp;
	int i=0;

	if((fp=fopen(ICFileLoc,"w"))==NULL)
	{
		printf("Error opening the file.");
		return;
	}
	else
	{
		for(i=0;i<num;i++)
			fprintf(fp,"%d\t%s, %s\t%s, %s\t%s, %s\n",InterCode[i].LC,InterCode[i].insType,InterCode[i].insCode,InterCode[i].op1Type,InterCode[i].op1Code,InterCode[i].op2Type,InterCode[i].op2Code);

		fclose(fp);
	}
}

void writeSYM(strSYM symTable[], int num)
{
	FILE *fp;
	int i=0;

	if((fp=fopen(SYFileLoc,"w"))==NULL)
	{
		printf("Error opening the file.");
		return;
	}
	else
	{
		for(i=0;i<num;i++)
			fprintf(fp,"%d\t%s\n",symTable[i].address,symTable[i].symbol);

		fclose(fp);
	}
}

void writeLIT(strLIT litTable[], int num)
{
	FILE *fp;
	int i=0;

	if((fp=fopen(LTFileLoc,"w"))==NULL)
	{
		printf("Error opening the file.");
		return;
	}
	else
	{
		for(i=0;i<num;i++)
			fprintf(fp,"%s\t%d\t%d\n",litTable[i].literal,litTable[i].address,litTable[i].poolID);

		fclose(fp);
	}
}

int getAddress(strSYM SymTable[], char symbol[], int n)
{
	int i;

	for(i=0;i<=n;i++)
	{
		if(strcmp(SymTable[i].symbol,symbol)==0)
			return SymTable[i].address;
	}
	return 0;
}
