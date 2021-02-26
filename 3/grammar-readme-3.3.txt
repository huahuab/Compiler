把上下无关文法定义在 grammar-input.txt 文本中，运行grammar.exe程序
即可读入文法到程序里，为了方便使用和后续操作，程序把每个符号都定义成一个数字，并且保存了双向的对应关系
然后程序将调用 removeLeftRecursion() 方法，消除所有出现的左递归
然后程序会将输入的文法和改写的文法输出到 grammar-output.txt 文本中，方便观察