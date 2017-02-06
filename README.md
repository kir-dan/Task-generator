Year project in 3rd year "Task generator" 11/07/2016

GENERAL USAGE NOTES
---------------------------------------------
- Program creates tasks and answers for it on the given instructions in configuration file
- It has to launch from command line with following parameters:
```
    ./task-generator [config filename] [number of tasks]
```
- Many information about configuration file and possibilities read in 'lit/text.pdf'

INSTALLATION ON MAC OS
---------------------------------------------
1. Установить **InteLib**:
a) В Makefile поменять 37 строку
```
	ifeq ($(OSTYPE), MinGW-win)
```
На строку
```
	ifeq ($(OSTYPE), $(filter $(OSTYPE), MinGW-win darwin16))
```
где вместо **darwin16** может стоять другая строка, которая выдается по команде "echo $OSTYPE"
b)
```
	make ; make install
```
2. Настроить **pdflatex**
a) Установить pdflatex
```
	brew cask install basictex
```
b) Перезапустить сессию терминала
c) Установить пакеты русского языка
```
	sudo tlmgr install babel-russian hyphen-russian cyrillic t2 lh cm-super
```
d) Установить пакет для margin'a
```
	sudo tlmgr install vmargin
```
3. Скачать и собрать проект
```
	make all
```
4. Запустить проект
```
	./task-generator [config filename] [number of tasks]
```

For any questions, please contact: markitosha@gmail.com