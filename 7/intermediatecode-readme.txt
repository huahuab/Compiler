该代码是在编译原理课程设计中大家一起完成的中间代码生成的代码，做了小部分修改，作为作业上传。
语法分析，语义分析，中间代码的生成、优化:
    intermediatecode.cpp 文件是语法分析，语义分析，中间代码的生成、优化的源代码，相应的可执行文件为intermediatecode.exe
    该文件的输入有两个，1.grammar_parse_table-input.txt,文件中的内容是分析表，用来进行分析表
			2.grammar_token-input.txt，是词法分析生成的token序列
    文件的输出也有两个，1.quaternion_file-output.txt 存放传给目标代码生成的格式的四元式
			2.show_quaternion-output.txt 存放比较直观的四元式，展示给用户
代码先读入分析表文件  ，然后读入token序列文件，先用函数 produceTree() 产生分析树，然后将分析树用 semanticParse()进行语义分析，
若通过了语义分析就进行中间代码的生成由 produceIntermediateCode()执行。生成的过程也是跟据分析树节点的产生式编号，对生成对应的中间代码
并在合适的位置进行递归，保证中间代码的顺序正确。