把上下无关文法定义在 grammar-input.txt 文本中，运行grammar.exe程序
即可读入文法到程序里，为了方便使用和后续操作，程序把每个符号都定义成一个数字，并且保存了双向的对应关系
然后拓广了文法，添加开始唯一符号，将用 | 连接的式子进行拆分，程序里用数据结构存储了拓广后的文法.
判断文法是否符合LR(0),SLR(1),LR(1).
求出first集合和follow集合,然后通过判断自动机的每个节点状态来确定输入文法属于哪种文法。
程序输出文法类型(1:LR(0),2:SLR(1),3:LR(1))到 grammar-output.txt