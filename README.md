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
1) Установить **InteLib**

1.1) Устновить флаг
```
	USE_READLINE = no
```

1.2)
```
	make ; make install
```

2) Настроить **pdflatex**

2.1) Установить pdflatex
```
	brew cask install basictex
```
2.2) Перезапустить сессию терминала

2.3) Установить пакеты русского языка
```
	sudo tlmgr install babel-russian hyphen-russian cyrillic t2 lh cm-super
```
2.4) Установить пакет для margin'a
```
	sudo tlmgr install vmargin
```

3) Скачать и собрать проект

```
	make all
```

4) Запустить проект

```
	./task-generator [config filename] [number of tasks]
```

For any questions, please contact: markitosha@gmail.com