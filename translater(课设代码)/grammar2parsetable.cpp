#include<bits/stdc++.h>
using namespace std;

const int maxn=1e4+5;

//pro_num指向最后一个产生式下标 
int pro_num;
//S保存拓广后的开始符号 
string S;
//NT_lines保存每个非终结字符的产生式在哪几行 
map<string, vector<int> > NT_lines;
//strl暂存产生式左边的非终结符号，strr暂存产生式右边的各个符号 
string strl,strr,str;
//pro_left保存每行产生式左边的非终结符号 
vector<string> pro_left;
//pro_right保存产生式结果 
vector<string> pro_right[maxn];
//NT集合为非终结符号集合，T集合为终结符号集合 
set<string> NT,T;

//NT_first保存各个符号的first集合 
map<string, set<string> > NT_first;

//NT_folow保存各个非终结符号的follow集合 
map<string, set<string> > NT_follow; 

//定义项目结构item 
struct item{
	int row;//指明第几行的产生式
	int col;//指明当前产生式所处状态 
	set<string> search;
	bool operator< (const item &itemp) const
	{
		return this->row==itemp.row?this->col<itemp.col:this->row<itemp.row;
	}
};
//定义项目集的结点类型
struct node{
	set<item> itemSet;
	bool haveR;
}; 
//定义有效项目集的DFA的结点DFANodes
node DFANodes[maxn];
//定义DFA的邻接链表DFAArc
int DFAArc[maxn][1005];
//totalCharacter保存所有符号
vector<string> totalCharacter;
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

//LALR1中合并同心项后对项目结点是否已经更新的标记 
int amendVis[maxn];


//对集合进行并运算 
void Union(set<string> &dest,set<string> src,int op=0){
    set<string>::iterator it;
    if(op==1){
    	src.erase("#");
	}
    it = src.begin();
    while(it != src.end()){
        dest.insert(*it);
        it++;
    }
}


//读入上下文无关文法 
void readCFG(ifstream &fin)
{
	T.clear();
	NT.clear();
	NT_lines.clear();
	pro_left.clear();
	for(int i=0;i<maxn;i++){
		pro_right[i].clear();
	}
	NT.insert("S'"); 
	NT_lines["S'"].push_back(0);
	pro_left.push_back("S'");
	pro_num = 0;
	while(fin>>strl){
		NT.insert(strl);
		fin>>str;
		fin.get();
		getline(fin,strr);
		pro_left.push_back(strl);
		pro_num++;
		NT_lines[strl].push_back(pro_num);
		stringstream ss(strr);
		while(ss>>strr){
			if(strr=="|"){
				pro_num++;
				pro_left.push_back(strl);
				NT_lines[strl].push_back(pro_num);
			}
			else{
				if(strr!="ε"){
					for(int i=0;i<strr.size();){
						if(strr[i]=='\\'){
							strr.erase(i,1);
						}
						else i++;
					} 
					pro_right[pro_num].push_back(strr);
				}  
			}
		}
	}
	//确定开始符号和拓广的产生式 
	S = "S'";
	pro_right[0].push_back(pro_left[1]); 
	//确定终结符号的集合 
	for(int i=0;i<=pro_num;i++){
		for(int j=0;j<pro_right[i].size();j++){
			if(NT_lines.find(pro_right[i][j])==NT_lines.end()){
				T.insert(pro_right[i][j]);
			}
		}
	}
}


//输出分析表
void printParseTable(ofstream & fout)
{
	fout<<pro_num<<endl;
	for(int i=0;i<=pro_num;i++){
		fout<<pro_left[i]<<' ';
	} 
	fout<<endl;
	for(int i=0;i<=pro_num;i++){
		fout<<pro_right[i].size()<<endl;
		for(int j=0;j<pro_right[i].size();j++){
			fout<<pro_right[i][j]<<' ';
		}
		fout<<endl;
	}
	fout<<itemSetInd<<endl;
	fout<<actionCol.size()<<endl;
	for(int i=0;i<actionCol.size();i++){
		fout<<actionCol[i]<<' ';
	}
	fout<<endl;
	for(int i=0;i<itemSetInd;i++){
		for(int j=0;j<actionCol.size();j++){
			fout<<action[i][j].op<<action[i][j].to<<' ';
		}
		fout<<endl;
	}
	fout<<goToCol.size()<<endl;
	for(int i=0;i<goToCol.size();i++){
		fout<<goToCol[i]<<' ';
	}
	fout<<endl;
	for(int i=0;i<itemSetInd;i++){
		for(int j=0;j<goToCol.size();j++){
			fout<<goTo[i][j]<<' ';
		}
		fout<<endl;
	}
} 


//计算某个项目集合的项目闭包
void calClosure(node &p,int op)
{
	while(1){
		set<item> addItem;
		for(auto it = p.itemSet.begin();it!=p.itemSet.end();++it){
			if(it->col<pro_right[it->row].size()&&NT.find(pro_right[it->row][it->col])!=NT.end()){
				item itemp;
				itemp.col = 0;
				if(op!=0){
					int flag = 0;
					for(int i=(it->col)+1;i<pro_right[it->row].size();++i){
						if(NT_first[pro_right[it->row][i]].find("#")==NT_first[pro_right[it->row][i]].end()){
							Union(itemp.search,NT_first[pro_right[it->row][i]]);
							flag = 1;
							break;
						}
						else{
							Union(itemp.search,NT_first[pro_right[it->row][i]],1);
						}
					}
					if(flag==0){
						Union(itemp.search,it->search);
					}
				}
				for(int i=0;i<NT_lines[pro_right[it->row][it->col]].size();i++){
					itemp.row = NT_lines[pro_right[it->row][it->col]][i];
					if(op!=0){
						if(addItem.find(itemp)!=addItem.end()){
							auto iter = addItem.find(itemp);
							(const_cast<item*>(&(*iter))->search).insert(itemp.search.begin(),itemp.search.end());
						}
						else 
							addItem.insert(itemp);
					}
					else
						addItem.insert(itemp);
				}
			}
		}
		int prePSize = p.itemSet.size();
		p.itemSet.insert(addItem.begin(),addItem.end());
		if(p.itemSet.size()==prePSize) break;
	}
	for(auto it = p.itemSet.begin();it!=p.itemSet.end();++it){
		if(it->col>=pro_right[it->row].size()){
			p.haveR = true;
			break;
		}
	}
	return ;
}


//LALR1中只用做计算核心项的闭包
void justCalClosure(node &p)
{
	while(1){
		set<item> addItem;//每次保存增加的项目 
		for(auto it = p.itemSet.begin();it!=p.itemSet.end();++it){//遍历核心项 
			if(it->col<pro_right[it->row].size()&&NT.find(pro_right[it->row][it->col])!=NT.end()){//如果该核心项的转移字符是非终结符号 
				item itemp;
				itemp.col = 0;
				int flag = 0;
				for(int i=(it->col)+1;i<pro_right[it->row].size();++i){//新的闭包项目从对应的核心项目中生成搜索符 
					if(NT_first[pro_right[it->row][i]].find("#")==NT_first[pro_right[it->row][i]].end()){
						Union(itemp.search,NT_first[pro_right[it->row][i]]);
						flag = 1;
						break;
					}
					else{
						Union(itemp.search,NT_first[pro_right[it->row][i]],1);
					}
				}
				if(flag==0){//如果该核心项目后面进度的所有符号的First集合都包含空符号，则新生成的搜索符要加上该核心项目的搜索符号
					Union(itemp.search,it->search);
				}
				for(int i=0;i<NT_lines[pro_right[it->row][it->col]].size();i++){//将该非终结符号的所有产生时都变为初始项目放入集合中 
					itemp.row = NT_lines[pro_right[it->row][it->col]][i];
					if(addItem.find(itemp)!=addItem.end()){
						auto iter = addItem.find(itemp);
						(const_cast<item*>(&(*iter))->search).insert(itemp.search.begin(),itemp.search.end());
					}
					else 
						addItem.insert(itemp);
				}
			}
		}
		int prePSize = p.itemSet.size();
		p.itemSet.insert(addItem.begin(),addItem.end());
		if(p.itemSet.size()==prePSize) break;//结点的项目集合大小不变时循环结束 
	}
	for(auto it = p.itemSet.begin();it!=p.itemSet.end();++it){//判断当前结点的项目集合包不包含归约项目 
		if(it->col>=pro_right[it->row].size()){
			p.haveR = true;
			break;
		}
	}
	return ;
}


//合并同心项后对其后衍生出的状态结点的搜索符集合进行修改
void amendDFS(int index,int now,string symbol)
{
	node p;
	p.haveR = false;
	//根据转移字符生成核心项 
	for(auto it=DFANodes[index].itemSet.begin();it!=DFANodes[index].itemSet.end();++it){
		if(it->col<pro_right[it->row].size()&&pro_right[it->row][it->col]==symbol){
			item itemp;
			itemp.row = it->row;
			itemp.col = (it->col)+1;
			itemp.search = it->search;
			p.itemSet.insert(itemp);
		}
	}
	justCalClosure(p);//计算闭包项目
	for(auto it=DFANodes[now].itemSet.begin(),iter=p.itemSet.begin();it!=DFANodes[now].itemSet.end();++it,++iter){
		const_cast<item*>(&(*it))->search = iter->search;
	}//更新旧结点搜索符，不能直接换结点，要不然在最外层DFS会找不到 
	amendVis[now] = 1; 
	for(int j=0;j<totalCharacter.size();j++){//更新后面各结点的搜索符 
		if(DFAArc[now][j]!=0&&amendVis[DFAArc[now][j]]==0){
			amendDFS(now,DFAArc[now][j],totalCharacter[j]);
		}
	}
}
 

//查询是否已经存在与当前求出的项目集合相同或者可合并的项目集合，若有返回已有结点号，若没有返回0
int findEqual(const node &p,int op)
{
	for(int i=1;i<itemSetInd;++i){
		if(DFANodes[i].itemSet.size()==p.itemSet.size()){
			int flag = 0,LALRFlag=0;
			vector<set<string> > tmpMergeSearch(p.itemSet.size()); //暂时保存两个项目集合合并各个搜索符集合后的集合 
			int tmpMSearchInd = 0; //指向合并后搜索符集合的下标 
			for(auto it=DFANodes[i].itemSet.begin();it!=DFANodes[i].itemSet.end();++it){
				if(p.itemSet.find(*it)==p.itemSet.end()){//存在一个项目不相同，直接放弃该项目集合 
					flag=1;
					break;
				}
				else{//如果该项目找到相同的存在 
					if(op!=0){//只有LR1和LALR1才会进一步判断处理 
						auto findedItem = p.itemSet.find(*it);
						int flagg = 0;
						if((findedItem->search).size()==(it->search).size()){
							for(auto iter = (it->search).begin();iter!=(it->search).end();++iter){//比对搜索符集合是否相同 
								if((findedItem->search).find(*iter)==(findedItem->search).end()){
									flagg = 1;
									break;
								}
							}
						}
						else{
							flagg = 1;
						}
						if(op==1){ 
							if(flagg==1){//LR1中搜索符集合不同直接决定该项目集合不相等 
								flag = 1;
								break;
							}
						} 
						else if(op==2){//LALR1中虽然也要判断搜索符集合想不相同，但搜索符集合存在不相同不影响合并 
							if(flagg==1) LALRFlag = 1;
							tmpMergeSearch[tmpMSearchInd].insert((it->search).begin(),(it->search).end());
							tmpMergeSearch[tmpMSearchInd++].insert((findedItem->search).begin(),(findedItem->search).end());
						}
					}
				}
			}
			if(op==0||op==1){
				if(flag==0){//LR0、SLR1和LR1中flag=0表明下标为i的集合与目标项目集合相同 
					return i;
				}
			}
			else if(op==2){
				if(flag==0){//LALR1中flag=0表明项目集合中除去搜索符集合的部分相同 
					if(LALRFlag==0){//LALRFlag=0表明标为i的集合与目标项目集合相同 
						return i;
					}
					else{//LALR1项目集合相同但搜索符集合不同，就判断能否合并 
						set<string> sumStr;
						int sumSize = 0;
						int j = 0;
						for(auto it=DFANodes[i].itemSet.begin();it!=DFANodes[i].itemSet.end()&&j<tmpMSearchInd;++j,++it){
							if((it->col)>=pro_right[it->row].size()){
								sumStr.insert(tmpMergeSearch[j].begin(),tmpMergeSearch[j].end());
								sumSize += tmpMergeSearch[j].size();
							}
						}//判断合并后有没有归约-归约冲突 
						if(sumStr.size()==sumSize){//合并后没有归约-归约冲突 
							j = 0;
							for(auto it=DFANodes[i].itemSet.begin();it!=DFANodes[i].itemSet.end()&&j<tmpMSearchInd;++j,++it){
								const_cast<item*>(&(*it))->search = tmpMergeSearch[j];
							}//更新合并同心项后结点的搜索符
							for(int j=0;j<maxn;++j){
								amendVis[j] = 0;
							}
							amendVis[i] = 1;
							for(int j=0;j<totalCharacter.size();j++){//更新合并后同心项后面各结点的搜索符 
								if(DFAArc[i][j]!=0&&amendVis[DFAArc[i][j]]==0){
									amendDFS(i,DFAArc[i][j],totalCharacter[j]);
								}
							}
							return i;
						}
					}
				}
			}
		}
	}
	return 0;
} 
 

//计算某个项目集合对应的某个文法符号的转移函数
int calGo(int index, string symbol,int op)
{
	node p;
	p.haveR = false;
	for(auto it=DFANodes[index].itemSet.begin();it!=DFANodes[index].itemSet.end();++it){
		if(it->col<pro_right[it->row].size()&&pro_right[it->row][it->col]==symbol){
			item itemp;
			itemp.row = it->row;
			itemp.col = (it->col)+1;
			if(op!=0) itemp.search = it->search;
			p.itemSet.insert(itemp);
		}
	}
	calClosure(p,op);
	int equalInd = findEqual(p,op);
	int toCol = find(totalCharacter.begin(),totalCharacter.end(),symbol)-totalCharacter.begin();
	if(equalInd==0){
		DFANodes[itemSetInd] = p;
		DFAArc[index][toCol] = itemSetInd++;
	}
	else{
		DFAArc[index][toCol] = equalInd;
	}
	return equalInd;
}


//判断一个项目集合中是否全是归约项目
bool  judgeAllRe(const node &p)
{
	for(auto it=p.itemSet.begin();it!=p.itemSet.end();++it){
		if(it->col<pro_right[it->row].size()){
			return false;
		}
	}
	return true;
}


//从第零号项目集合开始扩散形成DFA
void DFS(int ind,int op)
{
	for(auto it=DFANodes[ind].itemSet.begin();it!=DFANodes[ind].itemSet.end();++it){
		if(it->col<pro_right[it->row].size()){
			int toCol = find(totalCharacter.begin(),totalCharacter.end(),pro_right[it->row][it->col])-totalCharacter.begin();
			if(DFAArc[ind][toCol]==0){
				int equalInd = calGo(ind, pro_right[it->row][it->col],op);
				if(equalInd==0&&!judgeAllRe(DFANodes[DFAArc[ind][toCol]])){
					DFS(DFAArc[ind][toCol],op);
				}
			}
		}
	}
} 


//生成DFA
void produceDFA(int op)
{
	//初始化数据结构
	itemSetInd = 0;
	for(int i=0;i<maxn;i++){
		DFANodes[i].itemSet.clear();
		DFANodes[i].haveR = false;
		for(int j=0;j<105;j++){
			DFAArc[i][j] = 0;
		}
	}
	//将终结符号和非终结符号合并到一起
	totalCharacter.clear(); 
	totalCharacter.insert(totalCharacter.end(),T.begin(),T.end());
	NT.erase("S'");
	totalCharacter.insert(totalCharacter.end(),NT.begin(),NT.end());
	NT.insert("S'");
	//初始化第零号项目集合的结点
	item itemp;
	itemp.row = 0;
	itemp.col = 0;
	if(op!=0) itemp.search.insert("$");
	DFANodes[itemSetInd].haveR = false;
	DFANodes[itemSetInd].itemSet.insert(itemp);
	calClosure(DFANodes[itemSetInd++],op);
	DFS(0,op);
}


//每轮求非终结符号的First集合 
bool getFirstPerTurn()
{
	bool change = false;
	for(map<string, vector<int> >::iterator it = NT_lines.begin();it!=NT_lines.end();++it){
		set<string> first;
		for(int i=0;i<(it->second).size();i++){
			int flag = 0;
			for(int j=0;j<pro_right[(it->second)[i]].size();j++){
				if(NT_first[pro_right[(it->second)[i]][j]].find("#")==NT_first[pro_right[(it->second)[i]][j]].end()){
					Union(first,NT_first[pro_right[(it->second)[i]][j]]);
					flag = 1;
					break;
				}
				else{
					Union(first,NT_first[pro_right[(it->second)[i]][j]],1);
				}
			}
			if(flag==0){
				first.insert("#");
			}
		}
		if(first.size()>NT_first[(it->first)].size()) change=true;
		NT_first[(it->first)] = first;
		first.clear();
	}
	return change;
} 


//求整个符号集的First集合 
void getFirst(){
	NT_first.clear();
	for(set<string>::iterator it = T.begin();it!=T.end();it++){
		set<string> first;
		first.insert((*it));
		NT_first[(*it)] = first;
		first.clear();
	}
	while(1){
		if(!getFirstPerTurn()) break;
	}
}


//初始化follow集合，将first的部分加入到follow中
void initFollow(){
	NT_follow[S].insert("$");
	for(int i=0;i<=pro_num;i++){
		for(int j=0;j<pro_right[i].size();++j){
			if(NT.find(pro_right[i][j])!=NT.end()){
				for(int k=j+1;k<pro_right[i].size();k++){
					if(NT_first[pro_right[i][k]].find("#")==NT_first[pro_right[i][k]].end()){
						Union(NT_follow[pro_right[i][j]],NT_first[pro_right[i][k]]);
						break;
					}
					else{
						Union(NT_follow[pro_right[i][j]],NT_first[pro_right[i][k]],1);
					}
				}
			}
		}
	}
} 


//每轮对所有非终结符号求Follow集合 
bool  getFollowPerTurn(){
	bool change = false;
	for(int i=0;i<=pro_num;i++){
		for(int j=0;j<pro_right[i].size();++j){
			if(NT.find(pro_right[i][j])!=NT.end()){
				int flag = 0;
				for(int k=j+1;k<pro_right[i].size();k++){
					if(NT_first[pro_right[i][k]].find("#")==NT_first[pro_right[i][k]].end()){
						flag = 1;
						break;
					}
				}
				if(flag==0){
					int pre_num = NT_follow[pro_right[i][j]].size();
					Union(NT_follow[pro_right[i][j]],NT_follow[pro_left[i]]);
					if(NT_follow[pro_right[i][j]].size()>pre_num) change = true;
				}
			}
		}
	}
	return change;
}


//求所有非终结符号的Follow集合 
void getFollow()
{
	//对产生式和终结符号进行修改
	int emptyStrFlag = 0;
	for(int i=0;i<=pro_num;++i){
		if(pro_right[i].size()==0){
			pro_right[i].push_back("#");
			emptyStrFlag = 1;
		}
	}
	if(emptyStrFlag) T.insert("#");
	//先求First集合
	getFirst();
	//初始化Follow集合 
	NT_follow.clear();
	initFollow();
	//开始循环求Follow集合，直至Follow集合不发生变化 
	while(1){
		if(!getFollowPerTurn()) break;
	}
	//将产生式和终结符号修改回去 
	for(int i=0;i<=pro_num;++i){
		if(pro_right[i].size()==1&&pro_right[i][0]=="#"){
			pro_right[i].pop_back();
		}
	}
	if(emptyStrFlag) T.erase("#");
}


//识别DFA的结构，判断用什么分析方法
int judgeLRMethod()
{
	//设置一下作为判断依据的标志位
	int SLR1 = 0, LR1 = 0; 
	//遍历项目集合的结点，寻找存在归约项目的项目集合并进行判断 
	for(int i=0;i<itemSetInd;++i){
		if(DFANodes[i].haveR&&DFANodes[i].itemSet.size()>1){
			SLR1 = 1;
			set<string> succeedStr;//将当前项目集合中基本项目的后继符号保存在一个集合中
			set<string> sumStr;//后继符号集合与所有归约项目的Follow集合的并集 
			int sumSize = 0;//各个归约项目左边非终结符号的Follow集合的大小之和 
			for(auto it=DFANodes[i].itemSet.begin();it!=DFANodes[i].itemSet.end();++it){
				if(it->col<pro_right[it->row].size()){
					succeedStr.insert(pro_right[it->row][it->col]);
				}
				else{
					sumSize += NT_follow[pro_left[it->row]].size();
					sumStr.insert(NT_follow[pro_left[it->row]].begin(),NT_follow[pro_left[it->row]].end()); 
				} 
			}
			sumSize += succeedStr.size();
			sumStr.insert(succeedStr.begin(),succeedStr.end());
			if(sumSize>sumStr.size()) LR1 = 1;
		}
	}
	//对判断标志位进行分析，返回结果：1用LR0，2用SLR1，3用LR1
	if(LR1==1){
		return 3;
	}
	else if(SLR1==1){
		return 2;
	}
	else{
		return 1;
	}
}


//判断LR1或者LALR1的分析方法能不能行
bool assessLR1_or_LALR1()
{
	for(int i=0;i<itemSetInd;++i){
		if(DFANodes[i].haveR&&DFANodes[i].itemSet.size()>1){
			set<string> succeedStr;//将当前项目集合中基本项目的后继符号保存在一个集合中
			set<string> sumStr;//后继符号集合与所有归约项目的搜索符集合的并集 
			int sumSize = 0;//各个归约项目搜索符集合的大小之和 
			for(auto it=DFANodes[i].itemSet.begin();it!=DFANodes[i].itemSet.end();++it){
				if(it->col<pro_right[it->row].size()){
					succeedStr.insert(pro_right[it->row][it->col]);
				}
				else{
					sumSize += (it->search).size();
					sumStr.insert((it->search).begin(),(it->search).end()); 
				} 
			}
			sumSize += succeedStr.size();
			sumStr.insert(succeedStr.begin(),succeedStr.end());
			if(sumSize>sumStr.size()){
				return false;
			}
		}
	}
	return true;
}


//生成LR0分析表
void produceLR0Table()
{
	//pos为索引找到的下标
	int pos; 
	//初始化action表和goto表的列索引 
	goToCol.assign(NT.begin(),NT.end());
	goToCol.erase(find(goToCol.begin(),goToCol.end(),"S'"));
	actionCol.assign(T.begin(),T.end());
	actionCol.push_back("$");
	//初始化action表和goto表
	for(int i=0;i<itemSetInd;++i){
		for(int j=0;j<1005;j++){
			action[i][j].op = '\0';
			action[i][j].to = 0;
			goTo[i][j] = 0;
		}
	} 
	//遍历邻接矩阵，生成LR0分析表
	for(int i=0;i<itemSetInd;++i){
		if(DFANodes[i].haveR){
			for(int j=0;j<actionCol.size();++j){
				action[i][j].op = 'r';
				action[i][j].to = (DFANodes[i].itemSet.begin())->row;
			}
		}
		else{
			for(int j=0;j<totalCharacter.size();j++){
				if(DFAArc[i][j]!=0){
					if(find(actionCol.begin(),actionCol.end(),totalCharacter[j])!=actionCol.end()){
						pos = find(actionCol.begin(),actionCol.end(),totalCharacter[j])-actionCol.begin();
						action[i][pos].op = 's';
						action[i][pos].to = DFAArc[i][j];
					}
					else{
						pos = find(goToCol.begin(),goToCol.end(),totalCharacter[j])-goToCol.begin();
						goTo[i][pos] = DFAArc[i][j];
					}
				}
			}
		}
	}
	//对出口状态进行修改
	for(int i=0;i<actionCol.size();i++){
		action[1][i].op = '\0';
		action[1][i].to = 0;
	}
	pos = find(actionCol.begin(),actionCol.end(),"$")-actionCol.begin();
	action[1][pos].op = 'a';
}


//生成SLR1分析表
void produceSLR1Table()
{
	//pos和poss为索引找到的下标
	int pos,poss; 
	//初始化action表和goto表的列索引 
	goToCol.assign(NT.begin(),NT.end());
	goToCol.erase(find(goToCol.begin(),goToCol.end(),"S'"));
	actionCol.assign(T.begin(),T.end());
	actionCol.push_back("$");
	//初始化action表和goto表
	for(int i=0;i<itemSetInd;++i){
		for(int j=0;j<1005;j++){
			action[i][j].op = '\0';
			action[i][j].to = 0;
			goTo[i][j] = 0;
		}
	} 
	//遍历邻接矩阵，生成SLR1分析表
	for(int i=0;i<itemSetInd;++i){
		for(auto it = DFANodes[i].itemSet.begin();it!=DFANodes[i].itemSet.end();++it){//遍历项目集合中所有项目 
			if(it->col>=pro_right[it->row].size()){//是归约项目 
				for(auto iter = NT_follow[pro_left[it->row]].begin();iter!=NT_follow[pro_left[it->row]].end();++iter){//遍历该归约项目的follow集合 
					pos = find(actionCol.begin(),actionCol.end(),(*iter))-actionCol.begin();
					action[i][pos].op = 'r';
					action[i][pos].to = it->row;
				}
			}
			else{//是基本项目 
				poss = find(totalCharacter.begin(),totalCharacter.end(),pro_right[it->row][it->col])-totalCharacter.begin();
				if(find(actionCol.begin(),actionCol.end(),pro_right[it->row][it->col])!=actionCol.end()){
					pos = find(actionCol.begin(),actionCol.end(),pro_right[it->row][it->col])-actionCol.begin();
					action[i][pos].op = 's';
					action[i][pos].to = DFAArc[i][poss];
				}
				else{
					pos = find(goToCol.begin(),goToCol.end(),pro_right[it->row][it->col])-goToCol.begin();
					goTo[i][pos] = DFAArc[i][poss];
				}
			}
		} 
	}
	//对出口状态进行修改
	pos = find(actionCol.begin(),actionCol.end(),"$")-actionCol.begin();
	action[1][pos].op = 'a';
}


//生成LR1分析表
void produceLR1_or_LALR1Table()
{
	//pos和poss为索引找到的下标
	int pos,poss; 
	//初始化action表和goto表的列索引 
	goToCol.assign(NT.begin(),NT.end());
	goToCol.erase(find(goToCol.begin(),goToCol.end(),"S'"));
	actionCol.assign(T.begin(),T.end());
	actionCol.push_back("$");
	//初始化action表和goto表
	for(int i=0;i<itemSetInd;++i){
		for(int j=0;j<1005;j++){
			action[i][j].op = '\0';
			action[i][j].to = 0;
			goTo[i][j] = 0;
		}
	}
	//遍历邻接矩阵，生成LR1或者LALR1分析表
	for(int i=0;i<itemSetInd;++i){
		for(auto it = DFANodes[i].itemSet.begin();it!=DFANodes[i].itemSet.end();++it){//遍历项目集合中所有项目 
			if(it->col>=pro_right[it->row].size()){//是归约项目 
				for(auto iter = (it->search).begin();iter!=(it->search).end();++iter){//遍历该归约项目的搜索符集合 
					pos = find(actionCol.begin(),actionCol.end(),(*iter))-actionCol.begin();
					action[i][pos].op = 'r';
					action[i][pos].to = it->row;
				}
			}
			else{//是基本项目 
				poss = find(totalCharacter.begin(),totalCharacter.end(),pro_right[it->row][it->col])-totalCharacter.begin();
				if(find(actionCol.begin(),actionCol.end(),pro_right[it->row][it->col])!=actionCol.end()){
					pos = find(actionCol.begin(),actionCol.end(),pro_right[it->row][it->col])-actionCol.begin();
					action[i][pos].op = 's';
					action[i][pos].to = DFAArc[i][poss];
				}
				else{
					pos = find(goToCol.begin(),goToCol.end(),pro_right[it->row][it->col])-goToCol.begin();
					goTo[i][pos] = DFAArc[i][poss];
				}
			}
		} 
	}
	//对出口状态进行修改
	pos = find(actionCol.begin(),actionCol.end(),"$")-actionCol.begin();
	action[1][pos].op = 'a';
}


int main()
{ 
	//打开文件 
	ifstream fin("grammar_rules.txt");
	ofstream fout("grammar_parse_table.txt");
	
	
	//输入上下文无关文法 
	readCFG(fin);
	
	
	//生成DFA 
	produceDFA(0);
	
	
	//求出Follow集合
	getFollow();
	
	
	//识别DFA的结构,判断用什么分析方法,并生成相应的分析表 
	int methodID = judgeLRMethod();
	if(methodID==1){
		//生成LR0的分析表
		produceLR0Table();
	}
	else if(methodID==2){
		//生成SLR1的分析表
		produceSLR1Table(); 
	}
	else{
		//生成符合LR1分析方法的DFA
		produceDFA(2);
		if(!assessLR1_or_LALR1()){
			printf("LALR1的分析方法无法分析该上下文无关文法！\n");
			produceDFA(1);
			if(!assessLR1_or_LALR1()){
				printf("LR1的分析方法无法分析该上下文无关文法！\n");
				return 0;
			}
			else{
				produceLR1_or_LALR1Table();
			}
		}
		else{
			produceLR1_or_LALR1Table(); 
		}
	}
	
	
	//输出分析表 
	printParseTable(fout);


	//关闭文件
	fin.close();
	fout.close();
	
	return 0;
}
