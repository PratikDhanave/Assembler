//Assembler Pass 2

#include<stdio.h>
#include<conio.h>
#include<stdlib.h>
#include<string.h>

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

typedef struct strMC
{
	int LC,insCode,op1Code,op2Addr;
}strMC;


char MOFileLoc[] = "E:\\sdl\\assem\\MOT.txt";
char ICFileLoc[] = "E:\\sdl\\assem\\InterCod.txt";
char SYFileLoc[] = "E:\\sdl\\assem\\STable.txt";
char LTFileLoc[] = "E:\\sdl\\assem\\LTable.txt";
char MCFileLoc[] = "E:\\sdl\\assem\\MCode.txt";

int readSYM(strSYM []);
void showSYM(strSYM [], int);
int readLIT(strLIT []);
void showLIT(strLIT [], int);
strMC makeBlank(strMC);
void writeMC(strMC [], int);

int main()
{
	strIC InterCode[50];
	strMC MC[50];
	strSYM SymTable[50];
	strLIT litTable[50];
	int noSYM=0,noLIT=0,i=0,j;
	FILE *ICFile;
	strIC temp;
	char LC[5];

	clrscr();

	noSYM = readSYM(SymTable);
	noLIT = readLIT(litTable);

	showSYM(SymTable,noSYM);
	printf("\n");
	showLIT(litTable,noLIT);

	if((ICFile=fopen(ICFileLoc,"r"))==NULL)
	{
		printf("Error opening InterCode File.");
		getchar();
		return 1;
	}

	while(fscanf(ICFile,"%s %s %s %s %s %s %s\n",LC,temp.insType,temp.insCode,temp.op1Type,temp.op1Code,temp.op2Type,temp.op2Code) != EOF)
	{
		printf("\n%s\t%s\t%s\t%s",LC,temp.insType,temp.op1Type,temp.op2Type);
		MC[i] = makeBlank(MC[i]);
		MC[i].LC = atoi(LC);

		if(strcmp(temp.insCode,"-") != 0)
		{
			MC[i].insCode = atoi(temp.insCode);
		}

		if(strcmp(temp.op1Code,"-") != 0)
		{
			MC[i].op1Code = atoi(temp.op1Code);
		}

		if(temp.op2Type[0] == 'L')
		{
			if(temp.op2Code[0] == '=')
			{
				char string[2];
				string[0] = temp.op2Code[2];
				string[1] = '\0';
				MC[i].op2Addr = atoi(string);
			}
			else
				MC[i].op2Addr = litTable[atoi(temp.op2Code)].address;
		}
		else if(temp.op2Type[0] == 'S')
			MC[i].op2Addr = SymTable[atoi(temp.op2Code)].address;
		else if(temp.op2Type[0] == 'C')
		{
			char string[2];
			string[0] = temp.op2Code[1];
			string[1] = '\0';
			MC[i].op2Addr = atoi(string);
		}
		i++;
	}

	printf("\n\n\n");
	for(j=0;j<i;j++)
		printf("\n%d\t+%d\t%d\t%d",MC[j].LC,MC[j].insCode,MC[j].op1Code,MC[j].op2Addr);

	writeMC(MC,i);

	getchar();
	return 0;
}

int readSYM(strSYM SymTable[])
{
	FILE *fp;
	char tmpAddr[10];
	int i=0;

	if((fp=fopen(SYFileLoc,"r"))==NULL)
	{
		printf("Error opening the SYMBOL file.");
		return 0;
	}
	else
	{
		while(fscanf(fp,"%s %s\n",tmpAddr,SymTable[i].symbol) != EOF)
		{
			SymTable[i].address = atoi(tmpAddr);
			i++;
		}
		return i;
	}
}

void showSYM(strSYM SymTable[], int n)
{
	int i;

	for(i=0;i<n;i++)
		printf("%s\t%d\n",SymTable[i].symbol,SymTable[i].address);
}

void showLIT(strLIT litTable[], int n)
{
	int i;

	for(i=0;i<n;i++)
		printf("%s\t%d\t%d\n",litTable[i].literal,litTable[i].address,litTable[i].poolID);
}

int readLIT(strLIT litTable[])
{
	FILE *fp;
	char tmpAddr[10],tmpPoolID[10];
	int i=0;

	if((fp=fopen(LTFileLoc,"r"))==NULL)
	{
		printf("Error opening the LITERAL file.");
		return 0;
	}
	else
	{
		while(fscanf(fp,"%s %s %s",litTable[i].literal,tmpAddr,tmpPoolID) != EOF)
		{
			litTable[i].address = atoi(tmpAddr);
			litTable[i].poolID = atoi(tmpPoolID);
			i++;
		}
		return i;
	}
}

strMC makeBlank(strMC MC)
{
	MC.LC = MC.insCode = MC.op1Code = MC.op2Addr = 0;

	return MC;
}

void writeMC(strMC MC[], int num)
{
	FILE *fp;
	int i=0;

	if((fp=fopen(MCFileLoc,"w"))==NULL)
	{
		printf("Error opening the OUTPUT file.");
		return;
	}
	else
	{
		for(i=0;i<num;i++)
			fprintf(fp,"%d\t+%d\t%d\t%d\n",MC[i].LC,MC[i].insCode,MC[i].op1Code,MC[i].op2Addr);

		fclose(fp);
	}
}
