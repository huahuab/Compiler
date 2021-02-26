就是对建图程序和分析程序的一个整合
把规则定义在 regular2minDFA-input.txt 文本中，运行regular2minDFA程序
即可输出 token 的种类和token的id和字符串的对应关系
然后输出minDFA的图，规则是  当前节点编号 下一个字母的asicll 下一个节点编号，
其中下一个字母可能为空(epsilon=256)或者表示该节点是否为可接受状态(Accpting=257)。
为了方便分析程序的读入，这程序直接输出了数字，最后输出-1，方便图的读入。
从 analysis-dfaMap-input.txt 中读入输出的token和图。
重定向输入到 analysis-test.cpp ，读入该文件的所有内容去掉空格和tab后放入分析程序逐词分析
每行输出一个单词以及一个类型，要是出现不能继续走，并且当前状态不合法的情况，输出error，从当前字母重新开始继续判断。
出现错误，函数会返回0，否则返回1；
输出到文件analysis-ouput.txt中。