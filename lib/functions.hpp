#define EPSILON 0.000000000000001
//TODO написать функцию, которая генерирует числа float, способные преобразовываться в дробь?
//степень
int Pow(int a, int b)
{
	int s = 1;
	while(b > 0){
		s *= a;
		--b;
	}
	return s;
}

//модуль -- целые числа
int Abs(int a)
{
	if(a >= 0)
		return a;
	else
		return -a;
}

//модуль -- вещенстенные числа
float Abs(float a)
{
	if(a >= 0)
		return a;
	else
		return -a;
}

bool IsList(LReference x)
{
    return x.TextRepresentation().GetString()[0] == '(';
}

//генерация случайного числа
float GenSmth(float a, float b, int type = 0)
{
    if(type){
        return (b - a + 2)*rand()/(RAND_MAX) + a - 1;
    }
    return (b - a + 1)*rand()/(RAND_MAX+1.0) + a;
}

//генерация рандомного целого числа
int GenInt(int a, int b)
{
	if(a >= 0)
		return (int)(GenSmth(a, b));
	else
		return (int)(GenSmth(a, b, 1));
}

//генерация рандомного вещественного числа с заданным количеством цифр после запятой
float GenFloat(float a, float b, int min = 0, int max = 0)
{
	float r;
	if (!max) {
	    max = min;
	    min = max;
	}
	int n = Pow(10, max);
    int m = Pow(10, max - min + 1);
	do{
	    r = GenSmth(a, b - 1);
        r -= (r * n - (int)(r * n)) / n;
	}while((int)(r * n) % m == 0);
	return r;
}

LReference FindVar(LReference func)
{
    LReference X = NIL;
    while(!func.IsEmptyList() && strcmp(X.TextRepresentation().GetString(), "X")){
        if (IsList(func.Car())) {
            X = FindVar(func.Car());
        } else {
            X = func.Car();
        }
        func = func.Cdr();
    }
    return X;
}

bool Confine(float r, LReference func)
{
    LListConstructor L;
    LFunctionConstructor F;
    LFunctionalSymbol<LFunctionFuncall> FUNCALL("FUNCALL");
    LReference VALUE(r), X = FindVar(func);
    LReference ref = (L| FUNCALL, (F ^ (L| LAMBDA, (L| X), func)), VALUE);
    return ref.Evaluate().IsTrue();
}

bool ConfineFrac(LReference value, LReference func)
{
    LListConstructor L;
    LFunctionConstructor F;
    LFunctionalSymbol<LFunctionFuncall> FUNCALL("FUNCALL");
    LReference X = FindVar(func);
    LReference ref = (L| FUNCALL, (F ^ (L| LAMBDA, (L| X), func)), ~value);
    return ref.Evaluate().IsTrue();
}

//генерация целого с ограничениями по списку
int GenIntWithConfine(LReference lx, int a, int b)
{
	int r;
	do{
		r = GenInt(a, b);
	}while(!Confine(r, lx));
	return r;
}

//генерация вещественного с ограничениями по списку
float GenFloatWithConfine(LReference lx, float a, float b, int min, int max)
{
	float r;
	do{
		r = GenFloat(a, b, min, max);
	}while(!Confine(r, lx));
	return r;
}

//проверка двух чисел на то, что они могут являться рациональной дробью
bool CheckNumDenum(int n, int m, float a, float b)
{
	float fn = n, fm = m;
	int mn = Abs(n), mm = Abs(m);
	return (n != 0) &&
	    ((mn == 1) || ((mn > mm) && (mn % mm != 0)) || ((mm >= mn) && (mm % mn != 0)));
}

LReference GenFrac(float a, float b, int den_a, int den_b)
{
    int denum = GenInt(den_a, den_b);
	int num_a = a * denum;
	int num_b = b * denum;
	int num;

	do{
	    num = GenInt(num_a, num_b);
	}while(!CheckNumDenum(num, denum, a, b));

	LListConstructor L;
	LReference re = (L| num, denum);
	return re;
}

LReference GenFracWithConfine(float a, float b, int den_a, int den_b, LReference conf)
{
    LReference frac;
    do{
        frac = GenFrac(a, b, den_a, den_b);
    }while(!ConfineFrac(frac, conf));
    return frac;
}

void SeparateFrac(LReference x, int &a, int &b)
{
    if (!IsList(x)) {
        a = x.GetInt();
        b = 1;
    } else {
        a = x.Car().GetInt();
        b = x.Cdr().Car().GetInt();
    }
}

LReference FracSum(LReference x, LReference y)
{
    LListConstructor L;
    int a, b, c, d;
    SeparateFrac(x, a, b);
    SeparateFrac(y, c, d);
    return (L| a * d + c * b, b * d);
}

LReference FracDiff(LReference x, LReference y)
{
    LListConstructor L;
    int a, b, c, d;
    SeparateFrac(x, a, b);
    SeparateFrac(y, c, d);
    return (L| a * d - c * b, b * d);
}

LReference FracMulti(LReference x, LReference y)
{
    LListConstructor L;
    int a, b, c, d;
    SeparateFrac(x, a, b);
    SeparateFrac(y, c, d);
    return (L| a * c, b * d);
}

LReference FracDivision(LReference x, LReference y)
{
    LListConstructor L;
    int a, b, c, d;
    SeparateFrac(x, a, b);
    SeparateFrac(y, c, d);
    return (L| a * d, b * c);
}

//вычисление выражение с кавычкой
static SReference QuoteExpression(const SReference &ref, void *m)
{
    return ~(LReference(ref));
}

//генерация целого значение
//generateint(min, max, confines)
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

//генерация вещественного значенияс определенным количеством знаков после запятой
//GENERATEFLOAT(MIN, MAX, MAX_C_ZN/[MIN_C_ZN, MAX_C_ZN, EXCEPTION])
//TODO переделать определение параметров -- сделать функцию ограничений по-другому
class LFunctionGenerateFloat: public SExpressionFunction {
public:
    LFunctionGenerateFloat() : SExpressionFunction(2, 5){}
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
	float r, a = paramsv[0].GetFloat(), b = paramsv[1].GetFloat();
	switch(paramsc){
	case 2:
	    mx = 6;
        r = GenFloat(a, b, mx);
        break;
	case 3:
		mx = paramsv[2].GetInt();
		r = GenFloat(a, b, mx);
		break;
	case 4:
		mn = paramsv[2].GetInt();
		mx = paramsv[3].GetInt();
		r = GenFloat(a, b, mn, mx);
		break;
	case 5:
		mn = paramsv[2].GetInt();
		mx = paramsv[3].GetInt();
		r = GenFloatWithConfine(paramsv[4], a, b, mn, mx);
		break;
	default: 
		break;
	}
    LReference res(r);
    lf.RegularReturn(res);
}

//генерация дроби
//GENERATFRAC(MIN, MAX, MIN_ZNAM, MAX_ZNAM)
//TODO написать арифметику для дробей or переписать всю арифметику так, чтобы она срабатывала с дробями
class LFunctionGenerateFrac: public SExpressionFunction {
public:
    LFunctionGenerateFrac() : SExpressionFunction(4, 5){}
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
    float a = paramsv[0].GetFloat(), b = paramsv[1].GetFloat();
    int den_a = paramsv[2].GetInt(), den_b = paramsv[3].GetInt();
	LReference re;
	if(paramsc == 5){
        re = GenFracWithConfine(a, b, den_a, den_b, paramsv[4]);
	}else{
	    re = GenFrac(a, b, den_a, den_b);
	}
    lf.RegularReturn(re);
}

//Вычисляем из дроби вещественное число
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
		float f = paramsv[0].Car().GetFloat() / paramsv[0].Cdr().Car().GetInt();
		lf.RegularReturn(f);
	}catch(...){
		lf.RegularReturn(paramsv[0]);
	}
}

//выделение числителя из дроби
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
		int f = paramsv[0].Car().GetInt();
		lf.RegularReturn(f);
	}catch(...){
		lf.RegularReturn(paramsv[0].GetFloat());
	}
}

//выделение знаменателя из дроби
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
		int f = paramsv[0].Cdr().Car().GetInt();
		lf.RegularReturn(f);
	}catch(...){
		lf.RegularReturn(paramsv[0].GetFloat());
	}
}

//получение дроби из двух целых чисел
//makefrac
class LFunctionMakeFrac : public SExpressionFunction {
public:
    LFunctionMakeFrac() : SExpressionFunction(2, 2){}
    virtual SString TextRepresentation() const;
    void DoApply(int paramsc, const SReference *paramsv, IntelibContinuation &lf) const;
};

SString LFunctionMakeFrac :: TextRepresentation() const
{
    return SString("#<FUNCTION MAKEFRAC>");
}

void LFunctionMakeFrac::
DoApply(int paramsc, const SReference paramsv[], IntelibContinuation& lf) const
{
	LListConstructor L;
	LReference res = (L| paramsv[0].GetInt(), paramsv[1].GetInt());
    lf.RegularReturn(res);
}

//сложение дробей и чисел
//+ plus
class LFunctionSum : public SExpressionFunction {
public:
    LFunctionSum() : SExpressionFunction(2, 2){}
    virtual SString TextRepresentation() const;
    void DoApply(int paramsc, const SReference *paramsv, IntelibContinuation &lf) const;
};

SString LFunctionSum :: TextRepresentation() const
{
    return SString("#<FUNCTION PLUS>");
}

void LFunctionSum::
DoApply(int paramsc, const SReference paramsv[], IntelibContinuation& lf) const
{
	LReference a = paramsv[0],
	    b = paramsv[1],
	    res;
    if (IsList(a) || IsList(b)) {
        res = FracSum(a, b);
    } else {
        res = a.GetFloat() + b.GetFloat();
    }
    lf.RegularReturn(res);
}

//разность дробей и чисел
//- minus
class LFunctionMinus : public SExpressionFunction {
public:
    LFunctionMinus() : SExpressionFunction(2, 2){}
    virtual SString TextRepresentation() const;
    void DoApply(int paramsc, const SReference *paramsv, IntelibContinuation &lf) const;
};

SString LFunctionMinus :: TextRepresentation() const
{
    return SString("#<FUNCTION MINUS>");
}

void LFunctionMinus::
DoApply(int paramsc, const SReference paramsv[], IntelibContinuation& lf) const
{
	LReference a = paramsv[0],
	    b = paramsv[1],
	    res;
    if (IsList(a) || IsList(b)) {
        res = FracDiff(a, b);
    } else {
        res = a.GetFloat() - b.GetFloat();
    }
    lf.RegularReturn(res);
}

//умножение дробей и чисел
//* multi
class LFunctionMulti : public SExpressionFunction {
public:
    LFunctionMulti() : SExpressionFunction(2, 2){}
    virtual SString TextRepresentation() const;
    void DoApply(int paramsc, const SReference *paramsv, IntelibContinuation &lf) const;
};

SString LFunctionMulti :: TextRepresentation() const
{
    return SString("#<FUNCTION MULTI>");
}

void LFunctionMulti::
DoApply(int paramsc, const SReference paramsv[], IntelibContinuation& lf) const
{
	LReference a = paramsv[0],
	    b = paramsv[1],
	    res;
    if (IsList(a) || IsList(b)) {
        res = FracMulti(a, b);
    } else {
        res = a.GetFloat() * b.GetFloat();
    }
    lf.RegularReturn(res);
}

//деление дробей и чисел
// / division
class LFunctionDivision : public SExpressionFunction {
public:
    LFunctionDivision() : SExpressionFunction(2, 2){}
    virtual SString TextRepresentation() const;
    void DoApply(int paramsc, const SReference *paramsv, IntelibContinuation &lf) const;
};

SString LFunctionDivision :: TextRepresentation() const
{
    return SString("#<FUNCTION DIVISION>");
}

void LFunctionDivision::
DoApply(int paramsc, const SReference paramsv[], IntelibContinuation& lf) const
{
	LReference a = paramsv[0],
	    b = paramsv[1],
	    res;
    if (IsList(a) || IsList(b)) {
        res = FracDivision(a, b);
    } else {
        res = a.GetFloat() / b.GetFloat();
    }
    lf.RegularReturn(res);
}

//TODO описать сокращение дробей
//TODO ограничение для дроби сделать одно
LExpressionPackage * MakeMyPackage(Table* table)
{
    LExpressionPackage *p = new LExpressionPackageIntelib;
    LFunctionalSymbol<LFunctionSum> s_1("+");
    p->Import(s_1);
    LFunctionalSymbol<LFunctionMinus> s_2("-");
    p->Import(s_2);
    LFunctionalSymbol<LFunctionDefvar> s_3("=");
    p->Import(s_3);
    LFunctionalSymbol<LFunctionMulti> s_4("*");
    p->Import(s_4);
    LFunctionalSymbol<LFunctionDivision> s_5("/");
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
    LFunctionalSymbol<LFunctionMakeFrac> s_34("MAKEFRAC");
    p->Import(s_34);
    LFunctionalSymbol<LFunctionFuncall> s_35("FUNCALL");
    p->Import(s_35);
    while(table != NULL){
        LSymbol symb(table->name);
        symb->SetDynamicValue(table->value);
        p->Import(symb);
        table = table->next;
    }
    return p;
}
