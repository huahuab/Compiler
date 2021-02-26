把规则定义在 regular2DFA-input.txt 文本中，运行regular2DFA程序
即可输出 token 的种类和token的id和字符串的对应关系
然后输出DFA的图，规则是  当前节点编号 下一个字母 下一个节点编号，
其中下一个字母可能为空(epsilon)或者表示该节点是否为可接受状态(Accpting)。
因为NFA的图只是中间产物，没有将该图输出再输入，而是直接在源程序上进行了转化。
NFA转DFA的过程主要写在 类里的toDFA()函数中。