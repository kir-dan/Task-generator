int Pow(int a, int b)
{
	int s = 1;
	while(b > 0){
		s *= a;
		--b;
	}
	return s;
}

int Abs(int a)
{
	if(a >= 0)
		return a;
	else
		return -a;
}

float Abs(float a)
{
	if(a >= 0)
		return a;
	else
		return -a;
}

int GenInt(int a, int b)
{
	if(a >= 0)
		return (int)(((intelib_float_t)(b - a)*rand()/(RAND_MAX+1.0)) + a);
	else
		return (int)(((intelib_float_t)(b - a + 1)*rand()/(RAND_MAX+1.0)) + a - 1);
}

float GenFloat(float a, float b, int min, int max)
{
	float r;
	int n = Pow(10, max);
	r = ((intelib_float_t)(b - a)*rand()/(RAND_MAX+1.0)) + a;
	r -= (r * n - (int)(r * n)) / n;
	if(min == 0){
		return r;	
	}
	int m = Pow(10, (max - min + 1));
	while((int)(r * n) % m == 0){
		r = ((intelib_float_t)(b - a)*rand()/(RAND_MAX+1.0)) + a;
		r -= (r * n - (int)(r * n)) / n;
	}
	return r;
}

int GenIntWithConfine(LReference lx, int a, int b)
{
	int r;
	do{
		r = GenInt(a, b);
		LReference x = lx;
		while(!x.IsEmptyList()){
			int m = x.Car().GetInt();
			if(m == r){
				break;
			}
			x = x.Cdr();
		}
		if(x.IsEmptyList())
			break;
	}while(1);
	return r;
}

float GenFloatWithConfine(LReference lx, float a, float b, int min, int max)
{
	float r;
	do{
		r = GenFloat(a, b, min, max);
		LReference x = lx;
		while(!x.IsEmptyList()){
			float m = x.Car().GetFloat();
			if(Abs(m - r) < 0.0000001){
				break;
			}
			x = x.Cdr();
		}
		if(x.IsEmptyList())
			break;
	}while(1);
	return r;
}

bool CheckNM(int n, int m, float a, float b)
{
	float fn = n;
	float fm = m;
	return (n != 0) && (Abs(n) % Abs(m) != 0) && (fn / fm > a) && (fn / fm < b);
}


static SReference QuoteExpression(const SReference &ref, void *m)
{
    return ~(LReference(ref));
}

//GENERATEINT
class LFunctionGenerateInt : public SExpressionFunction {
public:
    LFunctionGenerateInt() : SExpressionFunction(2, 3){}
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
	int r;
	if(paramsc > 2){
		r = GenIntWithConfine(paramsv[2], paramsv[0].GetInt(), paramsv[1].GetInt());
	}else{
		r = GenInt(paramsv[0].GetInt(), paramsv[1].GetInt());
	}
	LReference res(r);
    lf.RegularReturn(res);
}

//GENERATEFLOAT(MIN, MAX, MAX_C_ZN/[MIN_C_ZN, MAX_C_ZN, EXCEPTION])
class LFunctionGenerateFloat: public SExpressionFunction {
public:
    LFunctionGenerateFloat() : SExpressionFunction(3, 5){}
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
	int mx, mn;
	float r;
	switch(paramsc){
	case 3:
		mx = paramsv[2].GetInt();
		r = GenFloat(paramsv[0].GetFloat(), paramsv[1].GetFloat(), 0, mx);
		break;
	case 4:
		mn = paramsv[2].GetInt();
		mx = paramsv[3].GetInt();
		r = GenFloat(paramsv[0].GetFloat(), paramsv[1].GetFloat(), mn, mx);
		break;
	case 5:
		mn = paramsv[2].GetInt();
		mx = paramsv[3].GetInt();
		r = GenFloatWithConfine(paramsv[4], paramsv[0].GetFloat(),
			paramsv[1].GetFloat(), mn, mx);
		break;
	default: 
		break;
	}
    LReference res(r);
    lf.RegularReturn(res);
}

//GENERATFRAC(MIN, MAX, MIN_ZNAM, MAX_ZNAM)
class LFunctionGenerateFrac: public SExpressionFunction {
public:
    LFunctionGenerateFrac() : SExpressionFunction(4, 6){}
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
	int m;
	if(paramsc > 5){
		m = GenIntWithConfine(paramsv[5], paramsv[2].GetInt(), paramsv[3].GetInt());
	}else{
		m = GenInt(paramsv[2].GetInt(), paramsv[3].GetInt());
	}
	int a = - paramsv[3].GetInt()*paramsv[2].GetInt();
	int b = paramsv[3].GetInt()*paramsv[2].GetInt();
	int n = GenInt(a, b);
	if(paramsc > 4){
		while(1){
			n = GenIntWithConfine(paramsv[4], a, b);
			if(CheckNM(n, m, paramsv[0].GetFloat(), paramsv[1].GetFloat()))
				break;
		}
	}else{
		while(1){
			if(CheckNM(n, m, paramsv[0].GetFloat(), paramsv[1].GetFloat()))
				break;
			else
				n = GenInt(a, b);
		}
	}
	LListConstructor L;
	LFunctionalSymbol<LFunctionQuotient> fq("/");
	LReference re = (L| fq, n, m);
//	float res = re.Evaluate().GetFloat();
    lf.RegularReturn(re);
}

//MAKEFLOATFROMFRAC
class LFunctionFloatFFrac: public SExpressionFunction {
public:
    LFunctionFloatFFrac() : SExpressionFunction(1, 1){}
    virtual SString TextRepresentation() const;
    void DoApply(int paramsc, const SReference *paramsv, IntelibContinuation &lf) const;
};

SString LFunctionFloatFFrac:: TextRepresentation() const
{
    return SString("#<FUNCTION FLOATFROMFRAC>");
}

void LFunctionFloatFFrac::
DoApply(int paramsc, const SReference paramsv[], IntelibContinuation& lf) const
{
	try{
		float f = paramsv[0].Cdr().Car().GetFloat() / paramsv[0].Cdr().Cdr().Car().GetInt();
		lf.RegularReturn(f);
	}catch(...){
		lf.RegularReturn(paramsv[0]);
	}
}

//NUMERATOR
class LFunctionFracNumerator: public SExpressionFunction {
public:
    LFunctionFracNumerator() : SExpressionFunction(1, 1){}
    virtual SString TextRepresentation() const;
    void DoApply(int paramsc, const SReference *paramsv, IntelibContinuation &lf) const;
};

SString LFunctionFracNumerator:: TextRepresentation() const
{
    return SString("#<FUNCTION NUMERATOR>");
}

void LFunctionFracNumerator::
DoApply(int paramsc, const SReference paramsv[], IntelibContinuation& lf) const
{
	try{
		int f = paramsv[0].Cdr().Car().GetInt();
		lf.RegularReturn(f);
	}catch(...){
		lf.RegularReturn(paramsv[0].GetFloat());
	}
}

//DENUMERATOR
class LFunctionFracDenumerator: public SExpressionFunction {
public:
    LFunctionFracDenumerator() : SExpressionFunction(1, 1){}
    virtual SString TextRepresentation() const;
    void DoApply(int paramsc, const SReference *paramsv, IntelibContinuation &lf) const;
};

SString LFunctionFracDenumerator:: TextRepresentation() const
{
    return SString("#<FUNCTION DENUMERATOR>");
}

void LFunctionFracDenumerator::
DoApply(int paramsc, const SReference paramsv[], IntelibContinuation& lf) const
{
	try{
		int f = paramsv[0].Cdr().Cdr().Car().GetInt();
		lf.RegularReturn(f);
	}catch(...){
		lf.RegularReturn(paramsv[0].GetFloat());
	}
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
    LFunctionalSymbol<LFunctionFloatFFrac> s_9("FLOATFFRAC");
    p->Import(s_9);
    LFunctionalSymbol<LFunctionIf> s_10("IF");
    p->Import(s_10);
    LFunctionalSymbol<LFunctionMathequal> s_11("==");
    p->Import(s_11);
    LFunctionalSymbol<LFunctionMathnotequal> s_12("!=");
    p->Import(s_12);
    LFunctionalSymbol<LFunctionMathgeq> s_13(">=");
    p->Import(s_13);
    LFunctionalSymbol<LFunctionMathleq> s_14("<=");
    p->Import(s_14);
    LFunctionalSymbol<LFunctionLessp> s_15("<");
    p->Import(s_15);
    LFunctionalSymbol<LFunctionGreaterp> s_16(">");
    p->Import(s_16);
	LFunctionalSymbol<SFunctionAnd> s_17("AND");
	p->Import(s_17);
	LFunctionalSymbol<SFunctionOr> s_18("OR");
	p->Import(s_18);
	LFunctionalSymbol<LFunctionWhile> s_19("WHILE");
	p->Import(s_19);
	LFunctionalSymbol<LFunctionFracNumerator> s_20("NUMERATOR");
	p->Import(s_20);
	LFunctionalSymbol<LFunctionFracDenumerator> s_21("DENUMERATOR");
	p->Import(s_21);
	LFunctionalSymbol<LFunctionMod> s_22("MOD");
	p->Import(s_22);
	LFunctionalSymbol<LFunctionNot> s_23("NOT");
	p->Import(s_23);
    LFunctionalSymbol<SFunctionAcos> s_24("ACOS");
    p->Import(s_24);
    LFunctionalSymbol<SFunctionAsin> s_25("ASIN");
    p->Import(s_25);
    LFunctionalSymbol<SFunctionAtan> s_26("ATAN");
    p->Import(s_26);
    LFunctionalSymbol<SFunctionCos> s_27("COS");
    p->Import(s_27);
    LFunctionalSymbol<SFunctionExp> s_28("EXP");
    p->Import(s_28);
    LFunctionalSymbol<SFunctionExpt> s_29("EXPT");
    p->Import(s_29);
	LFunctionalSymbol<SFunctionLog> s_30("LOG");
    p->Import(s_30);
    LFunctionalSymbol<SFunctionSin> s_31("SIN");
    p->Import(s_31);
    LFunctionalSymbol<SFunctionSqrt> s_32("SQRT");
    p->Import(s_32);
    LFunctionalSymbol<SFunctionTan> s_33("TAN");
    p->Import(s_33);
    while(table != NULL){
        LSymbol symb(table->name);
        symb->SetDynamicValue(table->value);
        p->Import(symb);
        table = table->next;
    }
    return p;
}
