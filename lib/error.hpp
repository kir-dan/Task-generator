const char* less_arg = "Less arg",
*wr_config = "Write config",
*read_config = "Read config",
*open_file = "Open file",
*file_begin = "\\documentclass[oneside, final, 12pt]{article}\n \\usepackage[utf8]{inputenc} \n \\usepackage[english, russianb]{babel} \n \\usepackage{vmargin} \n \\setpapersize{A4} \n \\setmarginsrb{2.5cm}{2cm}{1.5cm}{2cm}{0pt}{0mm}{0pt}{13mm} \n \\newcounter{task} \n \\usepackage{indentfirst} \n \\newenvironment{task}[1][] \n {\\refstepcounter{task} \\par \\medskip \\textbf{Задача~\\thetask. #1} \\rmfamily}{\\medskip} \n \\sloppy \n \\begin{document}\n\n";

class Error{
	const char* err;
public:
	Error(const char* er):err(er){};
	void what(){
		printf("%s\n", err);
	}
};