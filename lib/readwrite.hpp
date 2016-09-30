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
}

const char * beautynum(IntelibReader &reader, const char* str)
{
	bool flt = false;
	int cnt = 0;
	if(str[0] == '('){
		char * new_str = new char[strlen(str) + 9];
		reader.FeedString(str);
		reader.FeedChar('\n');
		while(!reader.IsReady()){}
		LReference ref = reader.Get();
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
			++cnt;
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
	const char* str = beautynum(reader, ref.TextRepresentation().GetString());
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
		fprintf(fd, "+ %s", str);
	}else{
		fprintf(fd, "%s", str);
	}
}

void printAnswer(IntelibReader &reader, LexList* list, FILE* fd)
{
		fprintf(fd, "\n \n \\begin{task} \n \n");
		while(list != NULL){
				if(list->leks->type == string){
						fprintf(fd, "%s", list->leks->leks);
				}else if (list->leks->type == sign){
						if(list->next != NULL){
								list = list->next;
								if(list->leks->type == variable){
										printVar(reader, list->leks->leks, fd, 0);
								}else{
										throw Error(wr_config);
								}
						}else{
								throw Error(wr_config);
						}
				}else{
						printVar(reader, list->leks->leks, fd, 1);
				}
				list = list -> next;
		}
		fprintf(fd, "\n \n \\end{task} \n \n");
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

