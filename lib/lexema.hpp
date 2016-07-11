enum type{
	string, variable, sign, number
};

class Lexema{
public:
    int type;
    char *leks;
    int str_num;
    Lexema(const Lexema &l){
        leks = new char[strlen(l.leks) + 1];
        type = l.type;
        str_num = l.str_num;
        for(unsigned int i = 0; i < strlen(l.leks) + 1; i++)
            leks[i] = l.leks[i];
    };
    Lexema(char *name, int num, int t){
        leks = name;
        str_num = num;
        type = t;
    };
    ~Lexema(){delete [] leks;};
    void Print();
};

struct LexList{
    Lexema *leks;
    LexList *next;
    ~LexList(){delete leks;};
    LexList* Add(Lexema *l);
};

class LA{
protected:
    enum autom_status{
    	H, A, B, C, D, S
    };
    enum sym_type{
    	whitespace, letter, plus, sp_sym, num, symbol, equal, dote
    };
    int str_num;
    char *buf;
    int len;
    int len0;
    int state;
    int SymType(int c);
    char* MakeBuff(char *buff);
    virtual Lexema* HF(int c) = 0;
    virtual Lexema* AF(int c) = 0;
    virtual Lexema* BF(int c) = 0;
	virtual Lexema* CF(int c) = 0;
	virtual Lexema* DF(int c) = 0;
    virtual Lexema* SF(int c, int state) = 0;
public:
    LA(){
        str_num = 1;
        buf = new char[1024];
        len = 0;
        len0 = 0;
        state = 0;
    };
	~LA(){if (buf !=0) delete[] buf;};
	Lexema* GetSym(int c);
};

class LA_template: public LA{
    Lexema* HF(int c);
    Lexema* AF(int c);
    Lexema* BF(int c);
	Lexema* CF(int c);
	Lexema* DF(int c);
    Lexema* SF(int c, int state);
    void NulNum();
};

class LA_attribute: public LA{
	Lexema* HF(int c);
    Lexema* AF(int c);
    Lexema* BF(int c);
	Lexema* CF(int c){return 0;};
	Lexema* SF(int c, int state);
	Lexema* DF(int c);
};

class LA_solve: public LA{
	Lexema* HF(int c);
	Lexema* SF(int c, int state);
	Lexema* AF(int c){return 0;}
	Lexema* BF(int c){return 0;}
	Lexema* CF(int c){return 0;}
	Lexema* DF(int c){return 0;}
};

Lexema* LA_solve::HF(int c)
{
	buf[len++] = c;
	if(c == EOF)
		return SF(c, H);
	else
		return 0;
}

Lexema* LA_solve::SF(int c, int st)
{
	int type;
	char *buff = 0;
	buff = MakeBuff(buff);
	state = H;
	switch (st){
	case H:
		type = string;
		break;
	default:
		throw Error(wr_config);
	}
	Lexema *l = new Lexema(buff, str_num, type);
	if(c == '\n')
		str_num++;
	return l;
}

Lexema* LA_attribute::HF(int c)
{
	Lexema *l;
	if (len != 0){
		char ch = buf[0];
		len = 0;
		l = GetSym(ch);
		if (l != 0){
			buf[0] = c;
			len = 1;
			return l;
		}else{
			return GetSym(c);
		}
	}else{
		buf[len++] = c;
		switch (SymType(c)){
		case whitespace: case equal:
			if(c == '\n')
				str_num++;
			len = 0;
			return 0;
		case letter:
			state = A;
			return 0;
		case num:
			state = B;
			return 0;
		default:
			throw Error(wr_config);
			return 0;
		}
	}
}

Lexema* LA_attribute::AF(int c)
{
	buf[len++] = c;
	switch (SymType(c)){
	case letter:
		return 0;
	case whitespace: case equal:
		return SF(c, A);
	default:
		throw Error(wr_config);
		return 0;
	}
}

Lexema* LA_attribute::BF(int c)
{
	buf[len++] = c;
	switch (SymType(c)){
	case num:
		return 0;
	case dote:
		state = D;
		return 0;
	case whitespace:
		return SF(c, B);
	default:
		throw Error(wr_config);
		return 0;
	}
}

Lexema* LA_attribute::DF(int c)
{
	buf[len++] = c;
	switch (SymType(c)){
	case num:
		return 0;
	case whitespace:
		return SF(c, D);
	default:
		throw Error(wr_config);
		return 0;
	}
}

Lexema* LA_attribute::SF(int c, int st)
{
	int type;
	char *buff = 0;
	buff = MakeBuff(buff);
	state = H;
	switch (st){
	case A:
		type = variable;
		break;
	case B: case D:
		type = number;
		break;
	default:
		throw Error(wr_config);
		type = -1;
		return 0;
	}
	Lexema *l = new Lexema(buff, str_num, type);
	if(c == '\n')
		str_num++;
	return l;
}

Lexema* LA_template::HF(int c)
{
	Lexema *l;
	if (len != 0){
		char ch = buf[0];
		len = 0;
		l = GetSym(ch);
		if (l != 0){
			buf[0] = c;
			len = 1;
			return l;
		}else{
			return GetSym(c);
		}
	}else{
		buf[len++] = c;
		switch (SymType(c)){
		case whitespace:
			if(c == '\n')
				str_num++;
			state = A;
			return 0;
		case letter: case dote: case equal: case symbol: case plus: case num:
			state = A;
			return 0;
		case sp_sym:
			state = C;
			return 0;
		default:
			throw Error(wr_config);
			return 0;
		}
	}
}

Lexema* LA_template::AF(int c)
{
	buf[len++] = c;
	if(c == EOF)
		return SF(c, A);
	if(c == '\n'){
		++str_num;
		++len0;
	}else
		len0 = 0;
	switch (SymType(c)){
	case sp_sym:
		state = C;
		return 0;
	default:
		return 0;
	}
}

Lexema* LA_template::CF(int c)
{
	buf[len++] = c;
	if(c == '\n')
		str_num++;
	switch (SymType(c)){
	case plus:
		return SF(c, S);
	case letter:
		if(len != 2)
			return SF(c, C);
		state = B;
		buf[0] = buf[1];
		len = 1;
		return 0;
	default:
		state = A;
		return 0;
	}
}

Lexema* LA_template::BF(int c)
{
	buf[len++] = c;
	if(c == '\n')
		str_num++;
	switch (SymType(c)){
	case letter: case num:
		return 0;
	default:
		return SF(c, B);
	}
}

Lexema* LA_template::DF(int c)
{
	buf[len++] = c;
	if(c == '\n')
		str_num++;
	return SF(c, D);
}

char* LA::MakeBuff(char * buff)
{
	if(state == C){
		buff = new char[len - 1];
		for(int i = 0; i < len - 2; i++)
			buff[i] = buf[i];
		buff[len-2] = '\0';
	}else{
		buff = new char[len];
		for(int i = 0; i < len - 1; i++)
			buff[i] = buf[i];
		buff[len-1] = '\0';
	}
	buf[0] = buf[len-1];
	len = 1;
	return buff;
}

void LA_template::NulNum()
{
	len -= len0;
	str_num -= len0;
	len0 = 0;
}

Lexema* LA_template::SF(int c, int st)
{
	int type;
	if (len0 != 0){
		NulNum();
	}
	char *buff = 0;
	buff = MakeBuff(buff);
	state = H;
	switch (st){
	case A:
		type = string;
		break;
	case B:
		type = variable;
		break;
	case C:
		type = string;
		state = B;
		break;
	case S:
		type = string;
		state = D;
		break;
	case D:
		type = sign;
		state = B;
		break;
	default:
		throw Error(wr_config);
		type = -1;
		return 0;
	}
	Lexema *l = new Lexema(buff, str_num, type);
	if(c == '\n')
		str_num++;
	return l;
}

int LA::SymType(int c)
{
	if((c >= 'a' && c <= 'z') || (c >= 'A' && c <= 'Z') || c == '_')
		return letter;
	if(c >= '0' && c <= '9')
		return num;
	switch (c){
	case '=':
		return equal;
	case '.':
		return dote;
	case '+':
		return plus;
	case ' ': case'\t': case'\n':case EOF:case '\r':
		return whitespace;
	case '%':
		return sp_sym;
	default:
		return symbol;
	}
}

Lexema* LA::GetSym(int c)
{
	switch (state){
	case H: 
		return HF(c);
	case A:
		return AF(c);
	case B:
		return BF(c);
	case C:
		return CF(c);
	case D:
		return DF(c);
	default:
		throw Error(wr_config);
		return 0;
	}
}

void Lexema::Print()
{
	char *t = NULL;
	switch (type){
	case sign:
		t = (char *)"sign";
		break;
	case string:
		t = (char *)"string";
		break;
	case variable:
		t = (char *)"variable";
		break;
	case number:
		t = (char *)"number";
		break;
	default:
		throw Error(wr_config);
	}
	printf("   %d %s %s\n", str_num, t, leks);
}

void print(LexList *list)
{
	if (list -> next == 0){
		list -> leks -> Print();
		delete list;
	}else{
		list -> leks -> Print();
		print(list -> next);
		delete list;
	}
}

LexList* LexList::Add(Lexema *l)
{
	LexList * li = new LexList, *o_li = this;
	li->leks = new Lexema(*l);
	li->next = 0;
	if (o_li == 0)
		return li;
	else{
		while (o_li -> next != 0)
			o_li = o_li -> next;
		o_li -> next = li;
		return this;
	}
}

LexList *MakeList(FILE *fd_rd, int type)
{
	LA *A;
	if(type == templ)
		A = new LA_template;
	else if (type == attr)
		A = new LA_attribute;
	else 
		A = new LA_solve;
	int c = ' ';
	Lexema *l;
	LexList *list = 0;
	int ok = 0;
	while (ok == 0){
		c = getc(fd_rd);
		if(c == '%'){
			c = getc(fd_rd);
			if(c == '%'){
				c = getc(fd_rd);
				if(c == '%'){
					c = EOF;
				}else{
					l = A -> GetSym('%');
					if (l != 0)
						list = list -> Add(l);
				}
			}else{
				l = A -> GetSym('%');
				if (l != 0)
					list = list -> Add(l);
			}
		}
		l = A -> GetSym(c);
		if (l != 0)
			list = list -> Add(l);
		delete l;
		if(c == EOF)
			ok = 1;
	}
	return list;
}