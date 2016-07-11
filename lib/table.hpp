struct Table {
	char* name;
	double value;
	Table* next;
	Table(char* n, double val){
		name = new char[strlen(n)];
		strcpy(name, n);
		value = val;
		next = NULL;
	}
};

Table* Add(Table* table, LexList* list)
{
	if(list == NULL)
		return table;
	if(list -> leks -> type == variable){
		double dub = 0;
		if(list -> next != NULL){
			if(list -> next -> leks -> type == number){
				dub = atof(list -> next -> leks -> leks);
			}
		}
		Table* tab = new Table(list -> leks -> leks, dub);
		Table * table_cnt = table;
		if(table == NULL){
			return Add(tab, list -> next);
		}else{
			while(table_cnt -> next != NULL)
				table_cnt = table_cnt -> next;
			table_cnt -> next = tab;
		}
	}
	return Add(table, list -> next);
}

