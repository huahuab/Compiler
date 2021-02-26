#include<bits/stdc++.h>
using namespace std;

const int maxn=1e4+5;

//pro_num指向最后一个产生式下标 
int pro_num;
//pro_left保存每行产生式左边的非终结符号 
vector<string> pro_left;
//pro_right保存产生式结果 
vector<string> pro_right[maxn];

//itemSetInd保存当前的项目集合数量
int itemSetInd;

//定义action表存储的结构
struct actionUnit{
	char op;
	int to;
}; 
//定义action表
actionUnit action[maxn][1005];
//定义goto表 
int goTo[maxn][1005];
//定义action表和goto表的列索引 
vector<string> actionCol,goToCol;

//strl,strr是字符串的临时变量
string strl,strr,str; 
//tokenText保存输入的token,tokenName保存输入的token名称
vector<string> tokenText,tokenName;
//parseStack为分析栈 
stack<int>  parseStack;
//Next指向需要识别的记号的位置 
int Next;
//parseNodeNum指明当前开辟的语法树结点
int parseNodeNum; 
//parseTree保存语法树的结点
struct tnode{
	int produceNum;
	string tokenName;
	string idName;
	string val;
	string dtype;
	bool isConst;
};
tnode parseTree[maxn];
//parseArc保存语法书的边 
vector<int> parseArc[maxn];

//定义符号表的结构
struct lnode{
	string type;
};
//tmpVarInd表示临时变量个数
int tmpVarInd;
//tmpLabInd表示程序块编号
int tmpLabInd; 
//定义临时变量数组 
int tmpVar[maxn];
//定义符号表
map<string,lnode> labelTable;
//标记语法错误 
int doIntermediateCodeFlag;

//操作数结构 
struct token {
    string name;
    string type;     //1为非临时变量,2为临时变量,3为数字,4为字符,5为程序块标识名,6为变量名 
    token(){
    	type = "0";
	}
    token(string _name,string _type){
    	name = _name;
    	type = _type;
	}
};
//四元式结构体
struct Quaternion {
    int block;           //基本块标识
    int type;            //类型-0 1 2 3
    string op;           //操作
    token num1;         //操作数1
    token num2;         //操作数2
    token num3;          //结果
    Quaternion(string _op,token _num1=token(),token _num2=token(),token _num3=token()){
    	op = _op;
    	num1 = _num1;
    	num2 = _num2;
    	num3 = _num3;
	}
};
vector<Quaternion> quaternionArray;


void readParseTable(ifstream &fin)
{
	int m;
	string tmp_str;
	fin>>pro_num;
	for(int i=0;i<=pro_num;i++){
		fin>>tmp_str;
		pro_left.push_back(tmp_str);
	}
	for(int i=0;i<=pro_num;i++){
		fin>>m;
		for(int j=0;j<m;j++){
			fin>>tmp_str;
			pro_right[i].push_back(tmp_str);
		}
	}
	fin>>itemSetInd;
	fin>>m;
	for(int i=0;i<m;i++){
		fin>>tmp_str;
		actionCol.push_back(tmp_str);
	}
	for(int i=0;i<itemSetInd;i++){
		for(int j=0;j<m;j++){
			fin>>action[i][j].op>>action[i][j].to;
		}
	}
	fin>>m;
	for(int i=0;i<m;i++){
		fin>>tmp_str;
		goToCol.push_back(tmp_str);
	}
	for(int i=0;i<itemSetInd;i++){
		for(int j=0;j<m;j++){
			fin>>goTo[i][j];
		}
	}
}


//生成分析树或者语法树
void produceTree()
{
	//初始化需要的数据和结构
	int actionPos,goToPos,fenhaoPos;
	Next = 0;
	parseNodeNum=0;
	fenhaoPos=find(actionCol.begin(),actionCol.end(),";")-actionCol.begin();
	tokenName.push_back("$");
	while(!parseStack.empty()) parseStack.pop();
	parseTree[parseNodeNum].tokenName = "$";
	parseStack.push(parseNodeNum++);
	parseStack.push(0);
	
	//进行语法分析
	while(1){	
		actionPos = find(actionCol.begin(),actionCol.end(),tokenName[Next])-actionCol.begin();
		if(action[parseStack.top()][actionPos].op=='a') break;//设置循环结束出口
		else if(action[parseStack.top()][actionPos].op=='s'){//移入 
			int tmpTo = action[parseStack.top()][actionPos].to;
			parseTree[parseNodeNum].tokenName = tokenName[Next];
			if(tokenName[Next]=="Identifier") parseTree[parseNodeNum].idName = tokenText[Next];
			else if(tokenName[Next]=="Integer"||tokenName[Next]=="Char"){
				parseTree[parseNodeNum].isConst = true;
				if(tokenName[Next]=="Integer"){
					parseTree[parseNodeNum].val = tokenText[Next];
					parseTree[parseNodeNum].dtype = "Integer";
				}
				else{
					parseTree[parseNodeNum].dtype = "Char";
					if(tokenText[Next].size()==2) parseTree[parseNodeNum].val="";
					else parseTree[parseNodeNum].val = tokenName[Next][1];
				}
			}
			parseStack.push(parseNodeNum++);
			parseStack.push(tmpTo);
			Next++;
		}
		else if(action[parseStack.top()][actionPos].op=='r'){//归约 
			int proRow = action[parseStack.top()][actionPos].to;
			if(pro_right[proRow].size()==0){
				parseTree[parseNodeNum++].tokenName = "\0";
				parseArc[parseNodeNum].push_back(parseNodeNum-1);
			}
			parseTree[parseNodeNum].tokenName = pro_left[proRow];
			parseTree[parseNodeNum].produceNum = proRow;
			for(int i=0;i<pro_right[proRow].size();i++){//出栈产生式长度的2倍 
				parseStack.pop();
				parseArc[parseNodeNum].push_back(parseStack.top());
				parseStack.pop();	
			}
			//调整一下树的形状 
			reverse(parseArc[parseNodeNum].begin(),parseArc[parseNodeNum].end());
			//通过goto表产生新状态
			goToPos = find(goToCol.begin(),goToCol.end(),parseTree[parseNodeNum].tokenName)-goToCol.begin();
			int newState = goTo[parseStack.top()][goToPos];
			parseStack.push(parseNodeNum++);
			parseStack.push(newState); 
		}
		else{
			doIntermediateCodeFlag = 1;
			if(action[parseStack.top()][fenhaoPos].op=='r'){
				int tmpState = parseStack.top();
				parseStack.pop();
				cout<<parseTree[parseStack.top()].tokenName;
				printf("的后面缺少 ；\n");
				parseStack.push(tmpState);
				tokenName.insert(tokenName.begin()+Next,";");
				tokenText.insert(tokenText.begin()+Next,";");
			}
			else{
				cout<<tokenName[Next]<<"处的语法有错误！！"<<endl;
				if(tokenName[Next]!=";"&&tokenName[Next]!="}") Next++;
				else{
					int tmpState = parseStack.top();
					parseStack.pop();
					if(parseTree[parseStack.top()].tokenName!="stmt-sequence"&&parseTree[parseStack.top()].tokenName!="{"){
						parseStack.pop();
					}
					else{
						parseStack.push(tmpState);
						Next++;
					}
				}
			}
		}
	} 
} 


//遍历语法树进行语义分析
void semanticTraversalTree(int T)
{
	if(parseArc[T].size()==0) return ;
	if(parseTree[T].produceNum==21){
		semanticTraversalTree(parseArc[T][0]);
		parseTree[T].dtype = parseTree[parseArc[T][0]].dtype;
		parseTree[parseArc[T][1]].dtype = parseTree[T].dtype;
		semanticTraversalTree(parseArc[T][1]);
	}
	else if(parseTree[T].produceNum==22){
		parseTree[T].dtype = "Integer";
	}
	else if(parseTree[T].produceNum==23){
		parseTree[T].dtype = "Char";
	}
	else if(parseTree[T].produceNum==24){
		parseTree[parseArc[T][0]].dtype = parseTree[T].dtype;
		if(labelTable.find(parseTree[parseArc[T][2]].idName)==labelTable.end()){
			lnode tmp_lnode;
			tmp_lnode.type = parseTree[T].dtype;
			labelTable[parseTree[parseArc[T][2]].idName] = tmp_lnode;
		}
		else{
			doIntermediateCodeFlag=1;
			printf("变量重名定义！！\n");
		}
		semanticTraversalTree(parseArc[T][0]);
	}
	else if(parseTree[T].produceNum==25){
		if(labelTable.find(parseTree[parseArc[T][0]].idName)==labelTable.end()){
			lnode tmp_lnode;
			tmp_lnode.type = parseTree[T].dtype;
			labelTable[parseTree[parseArc[T][0]].idName] = tmp_lnode;
		}
		else{
			doIntermediateCodeFlag=1;
			printf("变量重名定义！！\n");
		}
	}
	else if(parseTree[T].produceNum==26||parseTree[T].produceNum==31||parseTree[T].produceNum==37||parseTree[T].produceNum==41){
		for(int i=0;i<parseArc[T].size();i++){
			semanticTraversalTree(parseArc[T][i]);
		}
		parseTree[T].dtype="Integer";
		parseTree[T].isConst = false;
		if(parseTree[parseArc[T][0]].dtype=="Integer"&&parseTree[parseArc[T][2]].dtype=="Integer"){
			parseTree[T].val = to_string(tmpVarInd++);
		}
		else{
			doIntermediateCodeFlag=1;
			printf("操作数不是整数！！\n");
		}
	}
	else if(parseTree[T].produceNum==27||parseTree[T].produceNum==32||parseTree[T].produceNum==38||parseTree[T].produceNum==42){
		semanticTraversalTree(parseArc[T][0]);
		parseTree[T].val = parseTree[parseArc[T][0]].val;
		parseTree[T].dtype = parseTree[parseArc[T][0]].dtype;
		parseTree[T].isConst = parseTree[parseArc[T][0]].isConst;
	}
	else if(parseTree[T].produceNum==45){
		semanticTraversalTree(parseArc[T][1]);
		parseTree[T].val = parseTree[parseArc[T][1]].val;
		parseTree[T].dtype = parseTree[parseArc[T][1]].dtype;
		parseTree[T].isConst = parseTree[parseArc[T][1]].isConst;
	}
	else if(parseTree[T].produceNum==28||parseTree[T].produceNum==33||parseTree[T].produceNum==39||parseTree[T].produceNum==43){
		parseTree[T].val = "0";
	}
	else if(parseTree[T].produceNum==29||parseTree[T].produceNum==34||parseTree[T].produceNum==40||parseTree[T].produceNum==44){
		parseTree[T].val = "1";
	}
	else if(parseTree[T].produceNum==30||parseTree[T].produceNum==35){
		parseTree[T].val = "2";
	}
	else if(parseTree[T].produceNum==36){
		parseTree[T].val = "3";
	}
	else if(parseTree[T].produceNum==46){
		parseTree[T].isConst = false;
		parseTree[T].dtype = "Integer";
		parseTree[T].val = to_string(tmpVarInd++);
	}
	else if(parseTree[T].produceNum==47){
		parseTree[T].isConst = false;
		parseTree[T].dtype = "Char";
		parseTree[T].val = to_string(tmpVarInd++);
	}
	else if(parseTree[T].produceNum==48){
		if(labelTable.find(parseTree[parseArc[T][0]].idName)!=labelTable.end()){
			parseTree[T].dtype = labelTable[parseTree[parseArc[T][0]].idName].type;
			parseTree[T].isConst = false;
			parseTree[T].val = parseTree[parseArc[T][0]].idName;
		}
		else{
			parseTree[T].isConst = false;
			parseTree[T].dtype = "Integer";
			doIntermediateCodeFlag=1;
			cout<< parseTree[parseArc[T][0]].idName;
			printf("变量没有定义！！\n");
		}
	}
	else if(parseTree[T].produceNum==13){
		semanticTraversalTree(parseArc[T][2]);
		if(labelTable.find(parseTree[parseArc[T][0]].idName)==labelTable.end()){
			doIntermediateCodeFlag=1;
			cout<< parseTree[parseArc[T][0]].idName;
			printf("变量没有定义！！\n");
		}
		else{
			if(labelTable[parseTree[parseArc[T][0]].idName].type!=parseTree[parseArc[T][2]].dtype){
				doIntermediateCodeFlag=1;
				printf("赋值语句等号两边类型不一致！！\n");
			}
		}
	}
	else if(parseTree[T].produceNum==15){
		semanticTraversalTree(parseArc[T][0]);
		if(labelTable.find(parseTree[parseArc[T][2]].idName)==labelTable.end()){
			doIntermediateCodeFlag=1;
			cout<< parseTree[parseArc[T][2]].idName;
			printf("变量没有定义！！\n");
		}
	}
	else if(parseTree[T].produceNum==16){
		if(labelTable.find(parseTree[parseArc[T][0]].idName)==labelTable.end()){
			doIntermediateCodeFlag=1;
			cout<< parseTree[parseArc[T][0]].idName;
			printf("变量没有定义！！\n");
		}
	}
	else if(parseTree[T].produceNum==20){
		if(labelTable.find(parseTree[parseArc[T][0]].idName)==labelTable.end()){
			doIntermediateCodeFlag=1;
			cout<< parseTree[parseArc[T][0]].idName;
			printf("变量没有定义！！\n");
		}
	}
	else{
		for(int i=0;i<parseArc[T].size();i++){
			semanticTraversalTree(parseArc[T][i]);
		}
	}
	return ;
}


//生成token
token produceToken(int index)
{
	string tmp_name,tmp_type;
	if(labelTable.find(parseTree[index].val)==labelTable.end()){
		if(parseTree[index].isConst){
			if(parseTree[index].dtype=="Integer"){
				tmp_type = "3";
				tmp_name = parseTree[index].val;
			}
			else{
				tmp_type = "4";
				tmp_name = parseTree[index].val;
			}
		}
		else{
			tmp_name = parseTree[index].val + "T";
			tmp_type = "2";
		}
	}
	else{
		tmp_name = parseTree[index].val;
		tmp_type = "1";
	}
	token token1(tmp_name,tmp_type);
	return token1;
} 


//遍历语法树生成中间代码
void interTraversalTree(int T)
{
	if(parseArc[T].size()==0) return ;
	if(parseTree[T].produceNum==26){
		interTraversalTree(parseArc[T][0]);
		interTraversalTree(parseArc[T][2]);
		token token1=produceToken(parseArc[T][0]);
		token token2=produceToken(parseArc[T][2]);
		token token3(parseTree[T].val+"T","2");
		if(parseTree[parseArc[T][1]].val=="0"){
			str = "lt";
		}
		else if(parseTree[parseArc[T][1]].val=="1"){
			str = "gt";
		}
		else if(parseTree[parseArc[T][1]].val=="2"){
			str = "eq";
		}
		Quaternion tmp_quaternion(str,token1,token2,token3);
		quaternionArray.push_back(tmp_quaternion);
	}
	else if(parseTree[T].produceNum==31){
		interTraversalTree(parseArc[T][0]);
		interTraversalTree(parseArc[T][2]);
		token token1=produceToken(parseArc[T][0]);
		token token2=produceToken(parseArc[T][2]);
		token token3(parseTree[T].val+"T","2");
		if(parseTree[parseArc[T][1]].val=="0"){
			str = "and";
		}
		else if(parseTree[parseArc[T][1]].val=="1"){
			str = "or";
		}
		else if(parseTree[parseArc[T][1]].val=="2"){
			str = "not";
		}
		else if(parseTree[parseArc[T][1]].val=="3"){
			str = "xor";
		}
		Quaternion tmp_quaternion(str,token1,token2,token3);
		quaternionArray.push_back(tmp_quaternion);
	}
	else if(parseTree[T].produceNum==37){
		interTraversalTree(parseArc[T][0]);
		interTraversalTree(parseArc[T][2]);
		token token1=produceToken(parseArc[T][0]);
		token token2=produceToken(parseArc[T][2]);
		token token3(parseTree[T].val+"T","2");
		if(parseTree[parseArc[T][1]].val=="0"){
			str = "add";
		}
		else if(parseTree[parseArc[T][1]].val=="1"){
			str = "sub";
		}
		Quaternion tmp_quaternion(str,token1,token2,token3);
		quaternionArray.push_back(tmp_quaternion);
	}
	else if(parseTree[T].produceNum==41){
		interTraversalTree(parseArc[T][0]);
		interTraversalTree(parseArc[T][2]);
		token token1=produceToken(parseArc[T][0]);
		token token2=produceToken(parseArc[T][2]);
		token token3(parseTree[T].val+"T","2");
		if(parseTree[parseArc[T][1]].val=="0"){
			str = "mul";
		}
		else if(parseTree[parseArc[T][1]].val=="1"){
			str = "div";
		}
		Quaternion tmp_quaternion(str,token1,token2,token3);
		quaternionArray.push_back(tmp_quaternion);
	}
	else if(parseTree[T].produceNum==46||parseTree[T].produceNum==47){
		token token1=produceToken(parseArc[T][0]);
		token token3(parseTree[T].val+"T","2");
		Quaternion tmp_quaternion("mov",token1,token3);
		quaternionArray.push_back(tmp_quaternion);
	}
	else if(parseTree[T].produceNum==13){
		interTraversalTree(parseArc[T][2]);
		token token1 = produceToken(parseArc[T][2]);
		token token3(parseTree[parseArc[T][0]].idName,"1");
		Quaternion tmp_quaternion("mov",token1,token3);
		quaternionArray.push_back(tmp_quaternion);
	}
	else if(parseTree[T].produceNum==15){
		interTraversalTree(parseArc[T][0]);
		token token1(parseTree[parseArc[T][2]].idName,"1");
		Quaternion tmp_quaternion("read",token1);
		quaternionArray.push_back(tmp_quaternion);
	}
	else if(parseTree[T].produceNum==16){
		token token1(parseTree[parseArc[T][0]].idName,"1");
		Quaternion tmp_quaternion("read",token1);
		quaternionArray.push_back(tmp_quaternion);
	}
	else if(parseTree[T].produceNum==20){
		token token1(parseTree[parseArc[T][0]].idName,"1");
		Quaternion tmp_quaternion("write",token1);
		quaternionArray.push_back(tmp_quaternion);
	}
	else if(parseTree[T].produceNum==10){
		interTraversalTree(parseArc[T][2]);
		token token1 = produceToken(parseArc[T][2]);
		token token2("0","3");
		str = to_string(tmpLabInd++);
		token token3("L"+str,"5");
		Quaternion tmp_quaternion("if_f",token1,token2,token3);
		quaternionArray.push_back(tmp_quaternion);
		interTraversalTree(parseArc[T][5]);
		quaternionArray.push_back(Quaternion("lab",token3));
	}
	else if(parseTree[T].produceNum==11){
		interTraversalTree(parseArc[T][2]);
		token token1 = produceToken(parseArc[T][2]);
		token token2("0","3");
		token token3("L"+to_string(tmpLabInd++),"5");
		Quaternion tmp_quaternion("if_f",token1,token2,token3);
		quaternionArray.push_back(tmp_quaternion);
		interTraversalTree(parseArc[T][5]);
		quaternionArray.push_back(Quaternion("lab",token3));
		interTraversalTree(parseArc[T][9]);
	}
	else if(parseTree[T].produceNum==12){
		token token3("L"+to_string(tmpLabInd++),"5");
		quaternionArray.push_back(Quaternion("lab",token3));
		interTraversalTree(parseArc[T][2]);
		token token1 = produceToken(parseArc[T][2]);
		token token2("0","3");
		token token4("L"+to_string(tmpLabInd++),"5");
		Quaternion tmp_quaternion("if_f",token1,token2,token4);
		quaternionArray.push_back(tmp_quaternion);
		interTraversalTree(parseArc[T][5]);
		quaternionArray.push_back(Quaternion("goto",token3));
		quaternionArray.push_back(Quaternion("lab",token4));
	}
	else{
		for(int i=0;i<parseArc[T].size();i++){
			interTraversalTree(parseArc[T][i]);
		}
	}
	return ;
}


//将声明变量转换为中间代码
void defineVarible()
{
	for(auto it=labelTable.begin();it!=labelTable.end();it++){
		token tmp_token(it->first,"6");
		quaternionArray.push_back(Quaternion("def",tmp_token));
	}
} 


//划分程序块
void partitionBlock()
{
	int blockInd = 0;
	for(int i=0;i<quaternionArray.size();i++){
		Quaternion x=quaternionArray[i];
		if(x.op=="if_f"||x.op=="goto"){
			x.block=blockInd++;
		}
		else if(x.op=="lab"){
			x.block=++blockInd;
		}
		else{
			x.block=blockInd;
		}
		quaternionArray[i]=x;
	}
} 


//代码优化
void optimization() {
    vector<int> index(tmpVarInd, -1), ans;
    set<string> st{"add", "sub", "mul", "div", "and","or",  "xor", "lt",  "gt",  "eq"};
    for (int i = 0; i < (int)quaternionArray.size(); i++) {
        Quaternion q = quaternionArray[i], p("none");
        // 2为临时变量,3为数字,4为字符
        if (q.num1.type == "3" && q.num2.type == "2") {
            string s = q.num2.name;
            s.pop_back();
            index[atoi(s.c_str())] = i;
            // cout<<atoi(s.c_str())<<endl;
        }
        if (st.count(q.op) &&q.num1.type == "2" && q.num2.type == "2") {
            string s1 = q.num1.name, s2 = q.num2.name, s3 = q.num3.name;
            s1.pop_back(), s2.pop_back(), s3.pop_back();
            int ina = index[atoi(s1.c_str())], inb = index[atoi(s2.c_str())];
            if (ina != -1 && inb != -1) {
                // cout << "yes" << endl;
                int a = atoi(quaternionArray[ina].num1.name.c_str()),
                    b = atoi(quaternionArray[inb].num1.name.c_str());
                //"add","sub","mul","div","and","or","xor","lt","gt","eq"
                if (q.op == "add") {
                    quaternionArray[i] =
                        Quaternion("mov", token(to_string(a + b), "3"), q.num3);
                } else if (q.op == "sub") {
                    quaternionArray[i] =
                        Quaternion("mov", token(to_string(a - b), "3"), q.num3);
                } else if (q.op == "mul") {
                    quaternionArray[i] =
                        Quaternion("mov", token(to_string(a * b), "3"), q.num3);
                } else if (q.op == "div") {
                    quaternionArray[i] =
                        Quaternion("mov", token(to_string(a / b), "3"), q.num3);
                } else if (q.op == "and") {
                    quaternionArray[i] =
                        Quaternion("mov", token(to_string(a & b), "3"), q.num3);
                } else if (q.op == "or") {
                    quaternionArray[i] =
                        Quaternion("mov", token(to_string(a | b), "3"), q.num3);
                } else if (q.op == "xor") {
                    quaternionArray[i] =
                        Quaternion("mov", token(to_string(a ^ b), "3"), q.num3);
                } else if (q.op == "lt") {
                    quaternionArray[i] = Quaternion(
                        "mov", token(to_string((int)(a < b)), "3"), q.num3);
                } else if (q.op == "gt") {
                    quaternionArray[i] = Quaternion(
                        "mov", token(to_string((int)(a > b)), "3"), q.num3);
                } else if (q.op == "eq") {
                    quaternionArray[i] = Quaternion(
                        "mov", token(to_string((int)(a == b)), "3"), q.num3);
                }
                ans.push_back(ina);
                ans.push_back(inb);
                index[atoi(s3.c_str())] = i;
            }
        }
    }
    vector<Quaternion> tmp;
	sort(ans.begin(),ans.end());
    for (int i = 0, j = 0; i < (int)quaternionArray.size(); i++) {
        if (i == ans[j]) {
            j++;
            continue;
        }
        tmp.push_back(quaternionArray[i]);
    }
    quaternionArray = tmp;
}

//生成中间代码 
void produceIntermediateCode(ofstream &fout)
{
	defineVarible();
	interTraversalTree(parseNodeNum-1);
	quaternionArray.push_back(Quaternion("end"));
	// partitionBlock();
	// optimization();
	// partitionBlock();
	for(auto x:quaternionArray){
		fout<<x.op<<endl;
		fout<<x.num1.type<<' '<<x.num1.name<<endl;
		fout<<x.num2.type<<' '<<x.num2.name<<endl;
		fout<<x.num3.type<<' '<<x.num3.name<<endl;
	}
	ofstream ffout("show_quaternion-output.txt");
	for(auto x:quaternionArray){
		ffout<<"(";
		ffout<<x.op<<",";
		if(x.num1.name!="")
			ffout<<x.num1.name<<",";
		else ffout<<"_"<<",";
		if(x.num2.name!="")
			ffout<<x.num2.name<<",";
		else ffout<<"_"<<",";
		if(x.num3.name!="")
			ffout<<x.num3.name;
		else ffout<<"_";
			ffout<<")"<<endl;
	}
}


//语义分析 
void semanticParse(ofstream &fout)
{
	tmpVarInd = 1;
	tmpLabInd = 1;
	labelTable.clear();
	semanticTraversalTree(parseNodeNum-1);
	if(doIntermediateCodeFlag==0){
		produceIntermediateCode(fout);
	}
}


int main()
{ 
	//打开文件 
	ifstream fin("grammar_parse_table-input.txt");
	ofstream fout("quaternion_file-output.txt");
	
	
	readParseTable(fin);
	
	
	//关闭文法输入的文件，打开记号输入的文件 
	fin.close();
	ifstream ffin("grammar_token-input.txt");
	
	
	//输入token文本和名称 
	tokenText.clear();
	tokenName.clear();
	while(ffin>>strl>>strr){
		if(strr!="haserror"){
			tokenText.push_back(strl);
			tokenName.push_back(strr);
		}
		else{
			if(strl=="1") doIntermediateCodeFlag=1;
			else doIntermediateCodeFlag=0;
		}
	}
	
	
	//生成分析树或者语法树
	produceTree();
	
	
	//语义分析
	semanticParse(fout); 
	

	//关闭文件 
	ffin.close();
	fout.close();
	
	return 0;
}
