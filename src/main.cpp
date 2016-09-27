#include <ctime>
#include <cstring>
#include <cstdlib>
#include <cstdio>

#include <intelib/lisp/lcont.hpp>
#include <intelib/lisp/lpackage.hpp>
#include <intelib/lfun_std.hpp>

enum{
	templ, attr, solv
};

#include "../lib/error.hpp"
#include "../lib/lexema.hpp"
#include "../lib/table.hpp"
#include "../lib/readwrite.hpp"
#include "../lib/functions.hpp"

LexList* makeListDirect(FILE* f_conf, int type)
{
	int c = ' ';
	while(c != '\n' && c != EOF){
		c = getc(f_conf);
	}
	return MakeList(f_conf, type);
}

int main(int argc, char** argv)
{
	srand(time(0));
	try{
		if (argc < 3){
			throw Error(less_arg);
		}
		printf("Open files...\n");
		//open files and other information
		FILE *f_config = openReadFile(argv[1]);
		FILE *f_var = openWriteFile(argc, argv, "task");
		FILE *f_ans = openWriteFile(argc, argv, "answer");
		printBegin(f_var);
		fprintf(f_var, "\n \\section*{Задачи} \n");
		printBegin(f_ans);
		fprintf(f_ans, "\n \\section*{Ответы} \n");
		int n = atoi(argv[2]);
		
		printf("Lex analisation...\n");
		//all parts has lex analizator
		LexList *list_templ = 0, *list_attr = 0, *list_solv = 0, *list_ans = 0;
		list_templ = makeListDirect(f_config, templ);
		list_attr = makeListDirect(f_config, attr);
		list_solv = makeListDirect(f_config, solv);
		list_ans = makeListDirect(f_config, templ);

		printf("Sintax analize...\n");
		//make variable table and little sintax analize maybe for some parts
		Table *table = NULL;
		table = Add(table, list_templ);
		table = Add(table, list_attr);
		
		printf("Solve 3 part...\n");
		//include readintelib and solve 3rd part
		IntelibReader reader;
		LReference package(MakeMyPackage(table));
		reader.SetPackage(static_cast<LExpressionPackage*>(package.GetPtr()));
		for(int i = 0; i < n; ++i){
			LexList *list_curr = list_solv;
			while(list_curr != NULL){
				printf("%s\n", list_curr->leks->leks);
			    reader.FeedString(list_curr->leks->leks);
			    reader.FeedChar('\n');
			    while(!reader.IsReady()){}
			    while(!reader.IsEmpty()){
			        LReference ref = reader.Get();
					printf("%s\n", ref.TextRepresentation().GetString());
					ref = ref.Evaluate();
			    }
			    list_curr = list_curr->next;
			}
	
			printf("Print task and answers...\n");
			//print task and answer
			printf("***PRINT VARIANTS***\n");
			printAnswer(reader, list_templ, f_var);
			printf("***PRINT ANSWERS***\n");
			printAnswer(reader, list_ans, f_ans);
		}
		//close files
		printEnd(f_var);
		printEnd(f_ans);
		fclose(f_var);
		printf("f_var closed\n");
		fclose(f_ans);
		printf("f_ans closed\n");
		fclose(f_config);
		printf("f_config closed\n");

		//make pdf
		makePdf();
	}catch(Error &err){
		err.what();
    }catch(const IntelibX& ex) {
        fprintf(stderr, "Caught IntelibX: %s\n--\n%s\n",
                ex.Description(),
                ex.Parameter().GetPtr() ?
                ex.Parameter()->TextRepresentation().c_str() : "#<unbound>");
        return 1;
    }catch(...){
		perror("Something wrong\n");
	}
}