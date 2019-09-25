%{
#include <stdio.h>
#include <string.h>
FILE *hyplink;
char *yytext_buf;
int lastchar; /* if last output char is newline */
%}

/* pre-definition of some re-expr */
ASC			[\x00-\x7f]
ASCN		[\x00-\t\v-\x7f]
ASCNTRS		[^ \t\r\n]
U			[\x80-\xbf]
U2			[\xc2-\xdf]
U3			[\xe0-\xef]
U4			[\xf0-\xf4]

/* match unicode & ascii char */
UANY		({ASC}|({U2}{U})|({U3}{U}{U})|({U4}{U}{U}{U}))
/* like UANY, but don't match \n */
UANYN		({ASCN}|({U2}{U})|({U3}{U}{U})|({U4}{U}{U}{U}))
/* like UANY, but don't match \s \t \r \n */
UANYNTRS	({ASCNTRS}|({U2}{U})|({U3}{U}{U})|({U4}{U}{U}{U}))
/* only match unicode, don't match ascii char */
UONLY		({U2}{U}|{U3}{U}{U}|{U4}{U}{U}{U})

Ws			([ \t\r\n]*)
Letter		([a-zA-Z])
URL			(([a-zA-Z]+)"://"[^ \t\r\n\"']*)
HREF		({URL}|[^\"' \t\r\n]+)

%x SCRIPT
%x STYLE
%x FORM
%x LINK
%x LINK_HREF
%x TAG
%x COMMENT
%%

("<"[Ss][Cc][Rr][Ii][Pp][Tt][^>]*">")				{BEGIN(SCRIPT);}
("<"[Ss][Tt][Yy][Ll][Ee][^>]*">")					{BEGIN(STYLE);}
("<"[Ff][oO][rR][Mm][^>]*">")						{BEGIN(FORM);}
("<"[Aa][ ]*)										{BEGIN(LINK);}
("<")												{BEGIN(TAG);}
("<!--")											{BEGIN(COMMENT);}

<SCRIPT>("</"[Ss][Cc][Rr][Ii][Pp][Tt][^>]*">"(\n)*)	{BEGIN(INITIAL);}		/* suppose there is no <script><script></script></script> */
<SCRIPT>.|\n										{;}

<STYLE>("</"[Ss][Tt][Yy][Ll][Ee][^>]*">"(\n)*)		{BEGIN(INITIAL);}
<STYLE>.|\n											{;}

<FORM>("</"[Ff][oO][rR][Mm][^>]*">"(\n)*)			{BEGIN(INITIAL);}
<FORM>.|\n											{;}

<LINK>([Hh][Rr][Ee][Ff]{Ws}"=")						{BEGIN(LINK_HREF);}
<LINK_HREF>({Ws})									{;}
<LINK_HREF>(({HREF})|(\"{HREF}\")|("'"{HREF}"'"))	{
														yyout = hyplink;
														if (yytext[0] == '\'' || yytext[0] == '"') {
															yytext_buf = malloc(yyleng);
															sprintf(yytext_buf, "%s", yytext + 1);
															yytext_buf[yyleng - 2] = 0;
															fprintf(yyout, "%s\n", yytext_buf);
															free(yytext_buf);
														} else {
															fprintf(yyout, "%s\n", yytext);
														}
														yyout = stdout;
														BEGIN(LINK);
													}
<LINK>(">"(\n)*)									{
														yyout = stdout;
														BEGIN(INITIAL);
													}
<LINK>.|\n											{;}

<TAG>(">"+(\n)*)									{BEGIN(INITIAL);}
<TAG>.|\n											{;} 

<COMMENT>("-->")									{BEGIN(INITIAL);}
<COMMENT>.|\n										{;}

(&[qQ][oO][tT]";")									{  
														printf("'");
													}
(&[gG][tT]";")										{
														printf(">");
													}
(&[lL][tT]";")										{
														printf("<");
													}
(&[aA][mM][pP]";")									{
														printf("&");
													}
(&[nN][bB][sS][pP]";")								{
														printf(" ");
													}
{Ws}												{;}
({UANYNTRS})/("<"[ \t]*"/")							{
														printf("%s\n", yytext);
													}

                     
%%

int main(int argc, char **argv)
{
	++argv, --argc;
	if (argc > 0) {
		if ((yyin = fopen(argv[0], "rb")) == NULL) {
			printf("the file %s could'nt open!\n", argv[0]);
			exit(1);
		}
	} else {
		yyin = stdin;
	}

	if ((hyplink = fopen("hyplink.txt", "w")) == NULL) {
		printf("coud'nt create hyperlink file!\n");
		exit(1);
	}

	yylex();

	fclose(yyin);
	fclose(hyplink);
	return 0;
}

int yywrap()
{  
	return 1;
}

