把上下无关文法定义在 grammar-input.txt 文本中，把token序列放在 tokens.in 文件中，运行grammar.exe程序
即可读入文法到程序里，为了方便使用和后续操作，程序把每个符号都定义成一个数字，并且保存了双向的对应关系
然后程序调用 removeLeftRecursion() 方法消除左递归
然后调用 leftFactoring() 方法，消除式子中出现的左公共因子
再调用 judgeTerminal() 方法，把符号分进终极和非终极符号集合
之后 judgeLegal() 分别计算出First，Follow，Select集合，然后判断文法是不是符合LL(1)文法
然后调用 getLLmap() 方法得到LL(1)分析表
之后使用 analysis() 判断 tokens.in 文件中的token序列是否符合输入文法
然后程序会将结果输出到 grammar-output.txt 文本中
为了方便debug和观察中间结果，程序中有一系列输出函数 output...(),去掉注释会将对应的信息输出到grammar-output.txt文本中。