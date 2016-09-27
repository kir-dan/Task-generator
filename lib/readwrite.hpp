const char* stradd(const char* left, const char* right)
{
	char * conc = new char[strlen(left) + strlen(right) + 1];
	strcpy(conc, left);
	strcat(conc, right);
	return conc;
}

FILE * openReadFile(char* name)
{
	FILE * fd = fopen(name, "r");
	if (fd == NULL){
		throw Error(read_config);
	}
	return fd;
}

FILE * openWriteFile(int argc, char** argv, const char *def)
{
	FILE * fd;
	if(argc == 4){
		fd = fopen(stradd(argv[3], ".tex"), "w");
	}else{
		fd = fopen(stradd(def, ".tex"), "w");
	}
	if (fd == NULL){
		throw Error(open_file);
	}
	return fd;
}

void printBegin(FILE* fd)
{
    fprintf(fd, "%s", file_begin);
}

void printEnd(FILE* fd)
{
    fprintf(fd, "\n\n \\end{document}");
	printf("END PRINTED\n");
}

const char * beautynum(IntelibReader &reader, const char* str)
{
	bool flt = false;
	int cnt = 0;
	printf("STR: %s\n", str);
	if(str[0] == '('){
		char * new_str = new char[strlen(str) + 9];
		reader.FeedString(str);
		reader.FeedChar('\n');
		while(!reader.IsReady()){}
		LReference ref = reader.Get();
		printf("%s\n", ref.TextRepresentation().GetString());
		int n = ref.Cdr().Car().GetInt();
		int m = ref.Cdr().Cdr().Car().GetInt();
		if(n < 0){
			n = -n;
			sprintf(new_str, "-\\frac{%d}{%d}", n, m);
		}else{
			sprintf(new_str, "\\frac{%d}{%d}", n, m);
		}
		return new_str;
	}
	if(str[0] == '"'){
		char *new_str = new char[strlen(str) - 1];
		for(int i = 0; i < strlen(str) - 2; ++i){
			new_str[i] = str[i+1];
		}
		return new_str;
	}
	for(int i = 0; i < strlen(str); ++i){
		if(str[i] == '.')
			flt = true;
		if(flt){
			if(str[i] == '0')
				++cnt;
			else
				cnt = 0;
		}
	}
	if(cnt == 0){
		return str;
	}else{
		if(str[strlen(str) - cnt - 1] == '.'){
			--cnt;
		}
		char* new_str = new char[strlen(str) - cnt + 1];
		new_str[strlen(str) - cnt] = '\0';
		strncpy(new_str, str, strlen(str) - cnt);
		return new_str;
	}
}

void printVar(IntelibReader &reader, char* name, FILE* fd, int type)
{
    reader.FeedString(name);
    reader.FeedChar('\n');
    while(!reader.IsReady()){}
    LReference ref = reader.Get();
	ref = ref.Evaluate();
	printf("BEGIN\n");
	const char* str = beautynum(reader, ref.TextRepresentation().GetString());
	printf("END_STR: %s\n END\n", str);
	bool init = false;
	try{
		init = ref.GetFloat() >= 0;
	}catch(...){
		if(str[0] == '-')
			init = false;
		else
			init = true;
	}
	if(type == 0 && init){
		printf("1\n");
		fprintf(fd, "+ %s", str);
		printf("2\n");
	}else{
		printf("3\n");
		fprintf(fd, "%s", str);
		printf("4\n");
	}
	printf("VAR PRINTED\n");
}

void printAnswer(IntelibReader &reader, LexList* list, FILE* fd)
{
		fprintf(fd, "\n \n \\begin{task} \n \n");
		while(list != NULL){
				if(list->leks->type == string){
						printf("Begin string\n");
						fprintf(fd, "%s", list->leks->leks);
						printf("End string\n");
				}else if (list->leks->type == sign){
						printf("Begin sign\n");
						if(list->next != NULL){
								list = list->next;
								printf("Go to next\n");
								if(list->leks->type == variable){
										printf("Begin var 1\n");
										printVar(reader, list->leks->leks, fd, 0);
										printf("End var 1\n");
								}else{
										throw Error(wr_config);
								}
						}else{
								throw Error(wr_config);
						}
						printf("End sign\n");
				}else{
						printf("Begin var 2\n");
						printVar(reader, list->leks->leks, fd, 1);
						printf("End var 2\n");
				}
				list = list -> next;
				printf("GOING TO NEXT\n");
		}
		fprintf(fd, "\n \n \\end{task} \n \n");
		printf("END PRINTING\n");
}

void makePdf()
{
    system("pdflatex task.tex > log_task.txt");
    system("pdflatex answer.tex > log_ans.txt");
    system("open task.pdf");
    system("open answer.pdf");
    system("rm answer.tex");
    system("rm task.tex");
    system("rm task.log");
    system("rm answer.aux");
    system("rm task.aux");
    system("rm answer.log");
    system("rm log_task.txt");
    system("rm log_ans.txt");
}

