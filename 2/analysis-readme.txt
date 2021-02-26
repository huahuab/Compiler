从 analysis-dfaMap-input.txt 中读入输出的token和图。
重定向输入到 analysis-test.cpp ，读入该文件的所有内容去掉空格和tab后放入分析程序逐词分析
每行输出一个单词以及一个类型，要是出现不能继续走，并且当前状态不合法的情况，输出error，从当前字母重新开始继续判断。
出现错误，函数会返回0，否则返回1；
输出到文件analysis-ouput.txt中。