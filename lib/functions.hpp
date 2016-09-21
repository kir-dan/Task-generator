int pow(int a, int b){
	int res = 1;
	while(b > 0){
		res *= a;
		--b;
	}
	return res;
}

//GENERATEINT
class LFunctionGenerateInt : public SExpressionFunction {
public:
    LFunctionGenerateInt() : SExpressionFunction(2, 2){}
    virtual SString TextRepresentation() const;
    void DoApply(int paramsc, const SReference *paramsv, IntelibContinuation &lf) const;
};

SString LFunctionGenerateInt :: TextRepresentation() const
{
    return SString("#<FUNCTION GENERATEINT>");
}

void LFunctionGenerateInt::
DoApply(int paramsc, const SReference paramsv[], IntelibContinuation& lf) const
{
    LReference res((int)
                   (((intelib_float_t)(paramsv[1].GetInt() - paramsv[0].GetInt())
                     *rand()/(RAND_MAX+1.0)) + paramsv[0].GetInt()));
    lf.RegularReturn(res);
}

//GENERATEFLOAT
class LFunctionGenerateFloat: public SExpressionFunction {
public:
    LFunctionGenerateFloat() : SExpressionFunction(3, 3){}
    virtual SString TextRepresentation() const;
    void DoApply(int paramsc, const SReference *paramsv, IntelibContinuation &lf) const;
};

SString LFunctionGenerateFloat:: TextRepresentation() const
{
    return SString("#<FUNCTION GENERATEFLOAT>");
}

void LFunctionGenerateFloat::
DoApply(int paramsc, const SReference paramsv[], IntelibContinuation& lf) const
{
	float re = ((intelib_float_t)(paramsv[1].GetFloat() - paramsv[0].GetFloat())
                     *rand()/(RAND_MAX+1.0)) + paramsv[0].GetFloat();
	int n = pow(10, paramsv[2].GetInt());
	re -= (re * n - (int)(re * n)) / n;
    LReference res(re);
    lf.RegularReturn(res);
}

//GENERATFRAC(MIN, MAX, MIN_ZNAM, MAX_ZNAM)
class LFunctionGenerateFrac: public SExpressionFunction {
public:
    LFunctionGenerateFrac() : SExpressionFunction(4, 4){}
    virtual SString TextRepresentation() const;
    void DoApply(int paramsc, const SReference *paramsv, IntelibContinuation &lf) const;
};

SString LFunctionGenerateFrac:: TextRepresentation() const
{
    return SString("#<FUNCTION GENERATEFRAC>");
}

void LFunctionGenerateFrac::
DoApply(int paramsc, const SReference paramsv[], IntelibContinuation& lf) const
{
	int m = ((intelib_float_t)(paramsv[3].GetInt() - paramsv[2].GetInt())
        *rand()/(RAND_MAX+1.0)) + paramsv[2].GetInt();
	int n = 0;
	while(1){
		n = ((intelib_float_t)(paramsv[3].GetInt() - 1)
		    *rand()/(RAND_MAX+1.0)) + 1;
		if(((float)n / m > paramsv[0].GetFloat()) && ((float)n / m < paramsv[1].GetFloat()))	
			break;
	}
	char * re;
	sprintf(re, "%d/%d", n, m);
	LReference res(re);
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
    LFunctionalSymbol<LFunctionGenerateFloat> s_6("GENERATEFLOAT");
    p->Import(s_6);
    LFunctionalSymbol<LFunctionGenerateInt> s_7("GENERATEINT");
    p->Import(s_7);
    LFunctionalSymbol<LFunctionGenerateFrac> s_8("GENERATEFRAC");
    p->Import(s_8);
    while(table != NULL){
        LSymbol symb(table->name);
        symb->SetDynamicValue(table->value);
        p->Import(symb);
        table = table->next;
    }
    return p;
}
