#include <iostream>
#include <string>
#include <stdio.h>
#include <conio.h>
#include <fstream>
#include<cstring>
#include <cstdlib>
using namespace std;

void lexan();
void factor();
void term();
void exp();
void parser();
void match(int t);
//void emit(int t, int tval);
void emit1(int t1, int tval1);
int lookup(char s[]);
int minsert(char s[], int typetoken);
void init();
void error(char m[], int lineno);

const int	
BSIZE = 128, 
lpara = '(', 
rpara = ')',
none = '#', 
mplus = '+',
num = 256, 
mminus = '-',
DIV = 257, 
star = '*',
MOD = 258, 
slash = '/',
id = 259, 
semicolon = ';',
done = 260,
strmax = 999,
symax = 100,
eos = '\0';

struct Entry
{
	char* lexptr;
	int token;
};

int tokenval, lineno, typetoken, lastentry, lastchar;
int lookahead;
Entry symtable[symax];
char lexbuf[BSIZE];
char lexemes[strmax];
ifstream read;

void lexan()
{
	int p, b, i;
	char t;
	t = read.get();

	if (t == ' ' || t == '\t')
	{
		do
		{
			t = read.get();
		} while (t == ' ' || t == '\t');
	}
	else if (t == '\n')
	{
		lineno++;
		t = read.get();
	}
	if (read.eof())
	{
		typetoken = done;
		tokenval = none;
	}
	else if (t >= '0' && t <= '9')
	{
		int val = t - 48;
		tokenval = 0;
		while (val >= 0 && val <= 9 && !read.eof())
		{
			tokenval = tokenval * 10 + val;
			t = read.get();
			val = t - 48;
		}
		read.unget();

		typetoken = num;
		//    emit1(num, tokenval);
	}
	else if ((t >= 'a' && t <= 'z') || (t >= 'A' && t <= 'Z'))
	{
		p = 0;
		b = 0;
		while (((t >= '0' && t <= '9') || (t >= 'a' && t <= 'z') || (t >= 'A' && t <= 'Z')) && !read.eof())
		{
			lexbuf[b] = t;
			t = read.get();
			b++;
			if (b >= BSIZE)
				error("Run time error !", lineno);
		}
		read.unget();
		lexbuf[b] = eos;

		p = lookup(lexbuf);
		if (p == -1)
			p = minsert(lexbuf, id);
		tokenval = p;
		typetoken = symtable[p].token;
		//	emit1(id, tokenval);
	}
	else
	{
		typetoken = t;
		tokenval = none;
		//	emit1(t, none);
	}
}
void factor()
{
	switch (lookahead)
	{
	case lpara:
		match(lpara);
		//	emit(lpara, tokenval);//
		emit1(lpara, tokenval);//
		exp();
		match(rpara);
		break;
	case num:
		//	emit(num, tokenval);//
		emit1(num, tokenval);//
		match(num);
		break;
	case id:
		//	emit(id, tokenval);//
		emit1(id, tokenval);//
		match(id);
		break;
	default:
		error("loi cu phap", lineno);
	}
}
void term()
{
	int t;

	factor();

	while (lookahead == star || lookahead == slash || lookahead == DIV || lookahead == MOD)
	{
		t = lookahead;
		match(lookahead);
		factor();
		//emit(t, none);//
		emit1(t, none);//
	}
}
void exp()
{
	int t;

	term();

	while (lookahead == mplus || lookahead == mminus)
	{
		t = lookahead;
		match(lookahead);
		term();
		//	emit(t, none);//
		emit1(t, none);//
	}
}

void parser()
{
	lexan();
	lookahead = typetoken;
	while (lookahead != done)
	{
		exp();
		match(semicolon);
		cout << endl;
	}
}

void match(int t)
{
	if (lookahead == t)
	{
		lexan();
		lookahead = typetoken;
	}
	else
		error("Systax error !", lineno);
}

void emit1(int t1, int tval1)
{
	cout << "\t";
	switch (t1)
	{
	/*case lpara:
		cout << (char)t1 << "\t" << "<lpara, " << "( >" << "\n";
		break;
	case rpara:
		cout << "\t" << "<rpara, " << ") >" << "\n";
		break;*/
	case mplus: case mminus: case star: case slash:
		cout << (char)t1 << "\t" << "<" << (char)t1 << " , Operation>" << "\n";
		break;
	case DIV:
		cout << "<DIV, " << "DIV>" << "\n";
		break;
	case MOD:
		cout << "<MOD, " << "MOD>" << "\n";
		break;
	case num:
		cout << '[' << tval1 << ']' << "\t" << "<NUM, " << tval1 << ">" << "\n";
		break;
	case id:
		cout << symtable[tval1].lexptr << " \t" << "<ID, " << symtable[tval1].lexptr << ">" << "\n";
		break;
	default:
		cout << (char)t1;// << tval;
		//printf(" token %d , tokenval %d \n ", t1, tval1);
	}
}

int lookup(char s[])
{
	int p;
	p = lastentry;
	while (p >= 0 && std::strcmp(symtable[p].lexptr, s) != 0)
	{
		p--;
	}
	return p;
}

int minsert(char s[], int typetoken)
{
	int len = strlen(s);
	// cout<<"sds "<<len;
	if (lastentry >= symax)
		error("Bang danh bieu day", lineno);

	if (lastchar + len >= strmax)
		error("Day lexemes day", lineno);

	lastentry++;

	symtable[lastentry].token = typetoken;
	symtable[lastentry].lexptr = &lexemes[lastchar + 1];

	lastchar = lastchar + len + 1;
	strcpy(symtable[lastentry].lexptr, s);
	return lastentry;
}
void init()
{
	minsert("DIV", DIV);
	minsert("MOD", MOD);
}
void error(char m[], int lineno)
{
	cout << m << " " << lineno << endl;
	system("PAUSE");
	exit(1);
}

int main()
{
	lastentry = -1; lineno = 0; tokenval = -1;
	lastchar = -1;
	read.open("t.txt");
	init();
	parser();
	system("PAUSE");

	return 0;
}
