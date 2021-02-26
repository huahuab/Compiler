把上下无关文法定义在 grammar-input.txt 文本中，需要识别的token序列放在 token.in 文本中，
运行grammar.exe程序，即可读入文法到程序里，为了方便使用和后续操作，程序把每个符号都定义成一个数字，
并且保存了双向的对应关系，然后拓广了文法，添加开始唯一符号，将用 | 连接的式子进行拆分，
程序里用数据结构存储了拓广后的文法. 
getLALR1() 方法判断是否可以由LR1转化为LALR1，并完成相关函数的转化
getLALR1map() 方法得到LALR1map分析表
analysis() 方法分析 tokens.in 中的文本。
程序输出规约用到的式子到 grammar-output.txt，识别失败也会输出相应的提示到文本中。
程序中也写了 outputProductionSeq();outputStatus();outputG();outputLR0map();
等方法来进行debug和中间过程的输出，方便观察中间过程和修改bug。
LALR1相比较于LR1主要是合并LR1的同心项，过程中可以判断是否可以使用LR1