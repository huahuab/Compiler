import os
while 1:
    print('1-生成新的词法分析表 2-生成新的语法分析表 3-编译代码 4-退出')
    op=input('请输入要进行的操作：')
    if op=='1':
        if os.path.getsize(r'regular2minDFA-input.txt')==0 :
            print('请把新的正则表达式放在 regular2minDFA-input.txt 文件中')
        else :
            os.system(r'regular2minDFA')
            print('已经生成新的词法分析表，分析表文件为 analysis-dfaMap-input.txt')
    elif op=='2':
        if os.path.getsize(r'grammar_rules.txt')==0 :
            print('请把新的正则文法放在 grammar_rules.txt 文件中')
        else :
            os.system(r'grammar2parsetable')
            print('已经生成新的语法分析表，分析表文件为 grammar_parse_table.txt')
    elif op=='3':
        compileFile=input('请输入要编译的文件全名：');
        os.system(r'lexical_analysis'+' '+compileFile)##注意每个参数之间必须用空格隔开
        file_object = open('lexical_errors.txt',encoding='utf-8')
        try:
            all_the_text = file_object.read()
        finally:
            file_object.close()
        print(all_the_text)
        os.system(r'grammar_semantic_intermediatecode')
        if os.path.getsize(r'quaternion_file.txt')!=0:
            os.system(r'mips_generation')
    elif op=='4':
        break
    else :
        print('请输入正确的操作')
    print('\n')