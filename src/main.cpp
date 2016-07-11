#include <string.h>
#include <stdlib.h>
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

class LFunctionGenerate : public SExpressionFunction {
public:
    LFunctionGenerate() : SExpressionFunction(2, 2){}
    virtual SString TextRepresentation() const;
    void DoApply(int paramsc, const SReference *paramsv, IntelibContinuation &lf) const;
};

SString LFunctionGenerate :: TextRepresentation() const
{
    return SString("#<FUNCTION GENERATE>");
}

void LFunctionGenerate::
DoApply(int paramsc, const SReference paramsv[], IntelibContinuation& lf) const
{
    LReference res((int)
                   (((intelib_float_t)(paramsv[1].GetInt() - paramsv[0].GetInt())
                     *rand()/(RAND_MAX+1.0)) + paramsv[0].GetInt()));
    lf.RegularReturn(res);
}

LExpressionPackage * MakeMyPackage(Table* table)
{
    LExpressionPackage *p = new LExpressionPackageIntelib;
    LFunctionalSymbol<LFunctionPlus> s_1("+");
    p->Import(s_1);
    LFunctionalSymbol<LFunctionDifference> s_2("-");
    p->Import(s_2);
    LFunctionalSymbol<LFunctionDefvar> s_3("=");
    p->Import(s_3);
    LFunctionalSymbol<LFunctionTimes> s_4("*");
    p->Import(s_4);
    LFunctionalSymbol<LFunctionQuotient> s_5("/");
    p->Import(s_5);
    LFunctionalSymbol<LFunctionGenerate> s_6("GENERATE");
    p->Import(s_6);
    while(table != NULL){
        LSymbol symb(table->name);
        symb->SetDynamicValue(table->value);
        p->Import(symb);
        table = table->next;
    }
    return p;
}

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
	try{
		if (argc < 3){
			throw Error(less_arg);
		}
		//open files and other information
		FILE *f_config = openReadFile(argv[1]);
		FILE *f_var = openWriteFile(argc, argv, "task");
		FILE *f_ans = openWriteFile(argc, argv, "answer");
		printBegin(f_var);
		fprintf(f_var, "\n \\section*{Задачи} \n");
		printBegin(f_ans);
		fprintf(f_ans, "\n \\section*{Ответы} \n");
		int n = atoi(argv[2]);
		
		//all parts has lex analizator
		LexList *list_templ = 0, *list_attr = 0, *list_solv = 0, *list_ans = 0;
		list_templ = makeListDirect(f_config, templ);
		list_attr = makeListDirect(f_config, attr);
		list_solv = makeListDirect(f_config, solv);
		list_ans = makeListDirect(f_config, templ);

		//make variable table and little sintax analize maybe for some parts
		Table *table = NULL;
		table = Add(table, list_templ);
		table = Add(table, list_attr);
		
		//include readintelib and solve 3rd part
        IntelibReader reader;
        LReference package(MakeMyPackage(table));
        reader.SetPackage(static_cast<LExpressionPackage*>(package.GetPtr()));
		for(int i = 0; i < n; ++i){
			LexList *list_curr = list_solv;
			while(list_curr != NULL){
			    reader.FeedString(list_curr->leks->leks);
			    reader.FeedChar('\n');
			    while(!reader.IsReady()){}
			    while(!reader.IsEmpty()){
			        LReference ref = reader.Get();
			        ref = ref.Evaluate();
			    }
			    list_curr = list_curr->next;
			}

			//print task and answer
			printAnswer(reader, list_templ, f_var);
			printAnswer(reader, list_ans, f_ans);
		}
		//close files
		printEnd(f_var);
		printEnd(f_ans);
		fclose(f_var);
		fclose(f_ans);
		fclose(f_config);

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