把上下无关文法定义在 grammar-input.txt 文本中，运行grammar.exe程序
即可读入文法到程序里，为了方便使用和后续操作，程序把每个符号都定义成一个数字，并且保存了双向的对应关系
然后拓广了文法，添加开始唯一符号，将用 | 连接的式子进行拆分，程序里用数据结构存储了拓广后的文法，
输出拓广后的文法到 grammar-output.txt