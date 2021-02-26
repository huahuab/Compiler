把语法树放在 semantic-input.txt 文件中，每个节点的格式是输入格式是：

tokenName(string类型，token名字) 
produceNum(int类型，产生式编号)
val(int类型，节点的值)
dtype(string类型，节点的数据类型)
sons(多个int值在一行，表示儿子节点的编号)

然后运行 semantic.exe 文件，
程序会把语法树按照深度优先的方式进行遍历，在这个过程中将每个节点该补齐的属性补齐
然后将处理完的语法树输出到 semantic-output.txt 文件中，
为了方便观察：输出的格式为

节点号:x  
tokenName(string类型，token名字) 
produceNum(int类型，产生式编号)
val(int类型，节点的值)
dtype(string类型，节点的数据类型)
sons(多个int值在一行，表示儿子节点的编号,没有儿子会输出 noson)

程序简单实现了一个形如 a=10+2 这种带加减号的赋值语句的语法树的计算
输入是 a=10+2 的语法树
从样例的输出可以看到 2号节点，也就是标识符(a)节点的值已经成功赋值为12;
用到的产生式是大作业的产生式（产生式编号的来源），实现了其中一个简单的部分。