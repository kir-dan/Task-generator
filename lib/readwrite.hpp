//конкатенация строк
const char* stradd(const char* left, const char* right)
{
	char * conc = new char[strlen(left) + strlen(right) + 1];
	strcpy(conc, left);
	strcat(conc, right);
	return conc;
}

//открытие файла конфигурации для чтения
FILE * openReadFile(char* name)
{
	FILE * fd = fopen(name, "r");
	if (fd == NULL){
		throw Error(read_config);
	}
	return fd;
}

//открытие файла для записи
//Если указано имя файла в командной строке, то берем его, иначе дефолтное значение
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

//печать начала файла
void printBegin(FILE* fd)
{
    fprintf(fd, "%s", file_begin);
}

//печать конца файла
void printEnd(FILE* fd)
{
    fprintf(fd, "\n\n \\end{document}");
}

const char * beautyFrac(LReference ref, const char * str) {
    int frac_string_len = strlen(str) + 8;
    char * new_str = new char[frac_string_len];
    int n = ref.Car().GetInt();
    int m = ref.Cdr().Car().GetInt();

    if (n < 0) {
        sprintf(new_str, "-\\frac{%d}{%d}", -n, m);
    } else {
        sprintf(new_str, "\\frac{%d}{%d}", n, m);
    }

    return new_str;
}

const char * beautyString(LReference ref, const char * str) {
    char *new_str = new char[strlen(str)];
    int i, offset;

    for (i = 0, offset = 1; i < strlen(str) - offset - 1; ++i) {
        if (str[i + offset] == '\\') {
            ++offset;
        }

        new_str[i] = str[i + offset];
    }

    new_str[i] = '\0';

    return new_str;
}

const char * beautyNum(const char * str) {
    bool flt;
    int cnt = 0; //количество незначащих нулей

	for (int i = 0; i < strlen(str); ++i) {
        if (str[i] == '.') {
            flt = true;
        }

        if (flt) {
            cnt = str[i] == '0' ? ++cnt : 0;
        }
    }

    if (cnt == 0) {
        return str;
    } else {
        //если последний значащий символ точка, то точку тоже надо убрать
        if (str[strlen(str) - cnt - 1] == '.') {
            ++cnt;
        }

        char* new_str = new char[strlen(str) - cnt + 1];

        new_str[strlen(str) - cnt] = '\0';
        strncpy(new_str, str, strlen(str) - cnt);

        return new_str;
    }
}

//красивый вывод для переменных
const char * beautyPrint(LReference ref)
{
    const char * str = ref.TextRepresentation().GetString();

	if (str[0] == '(') {
	    return beautyFrac(ref, str);
	}

	if (str[0] == '"') {
	    return beautyString(ref, str);
	}

    return beautyNum(str);
}

const char * addSign(LReference ref, const char * str) {
    bool init = true;

	try {
		init = ref.GetFloat() >= 0;
	} catch(...) {
		if(str[0] == '-')
			init = false;
	}

	if (init) {
		return stradd("+", str);
	}

	return str;
}

//печать переменной в файл
//(reader, имя переменной, файл печати, тип: 0 -- со знаком, 1 -- без знака
void printVar(IntelibReader &reader, char* name, FILE* fd, int type)
{
    reader.FeedString(name);
    reader.FeedChar('\n');
    while (!reader.IsReady()) {}
    LReference ref = reader.Get();
	ref = ref.Evaluate();
	const char * str = beautyPrint(ref);

    if (type == 0) {
        str = addSign(ref, str);
    }

    fprintf(fd, "%s", str);
}

//печать ответа
void printAnswer(IntelibReader &reader, LexList* list, FILE* fd)
{
    fprintf(fd, "\n \n \\begin{task} \n \n");
    while (list != NULL) {
        if (list->leks->type == string) {
            fprintf(fd, "%s", list->leks->leks);
        } else if (list->leks->type == sign) {
            if (list->next != NULL) {
                list = list->next;
                if (list->leks->type == variable) {
                    printVar(reader, list->leks->leks, fd, 0);
                } else {
                    throw Error(wr_config);
                }
            } else {
                throw Error(wr_config);
            }
        } else {
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

