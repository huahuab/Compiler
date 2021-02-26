#include<bits/stdc++.h>
using namespace std;

struct Semantic {
  public:
	struct node{
		string tokenName;//token名字
		int produceNum,val;//产生式编号，节点的值
		string dtype;//节点的数据类型
		vector<int> son;//子节点编号
	};
	vector<node> Tree;
	void input(ifstream &in){
		string s;
		int a;
		while (in>>s)
		{
			// cout<<"aa  "<<s<<endl;
			if(s=="")break;
			node now;
			now.tokenName=s;
			in>>now.produceNum;//输入产生式编号
			in>>now.val;//值字段
			getline(in,s);//清除换行
			getline(in,s);//数据类型
			now.dtype=s;
			getline(in,s);//输入儿子
			stringstream ss; ss << s;
			while(ss>>a){//把son添加
				now.son.push_back(a);
			}
			Tree.push_back(now);
		}
	}
	void deal(int num){
		node now=Tree[num];
		switch (now.produceNum)
		{
			case 1://
				deal(now.son[0]);
				deal(now.son[1]);
				deal(now.son[2]);
				deal(now.son[3]);
				break;
			case 6://statement = assign-stmt
				// cout<<6<<endl;
				deal(now.son[0]);//assign-stmt处理
				break; 
			case 10://if-stmt = if ( exp ) { stmt-sequence } 
				deal(now.son[1]);//exp
				deal(now.son[6]);//stmt-sequence
				break;
			case 13://assign-stmt -> Identifier = exp
				// cout<<13<<endl;
				deal(now.son[2]);//exp
				if(Tree[now.son[0]].dtype!=Tree[now.son[2]].dtype)//类型不同,报警告
					cout<<"Type mismatch"<<endl;
				Tree[now.son[0]].val=Tree[now.son[2]].val;
				break;
			case 29://exp = rep 
				// cout<<29<<endl;
				deal(now.son[0]);//rep
				now.val=Tree[now.son[0]].val;
				now.dtype=Tree[now.son[0]].dtype;//获得类型
				break;
			case 34://rep = simple-exp
				// cout<<34<<endl;
				deal(now.son[0]);//simple-exp
				now.val=Tree[now.son[0]].val;
				now.dtype=Tree[now.son[0]].dtype;//获得类型
				break;
			case 39://simple-exp = simple-exp addop term 
				// cout<<39<<endl;
				deal(now.son[0]);//simple-exp,主要是获得值
				deal(now.son[2]);//term获得term的值
				if(Tree[now.son[1]].val==0)//+
					now.val=Tree[now.son[0]].val+Tree[now.son[2]].val;//获得值
				else//-
					now.val=Tree[now.son[0]].val+Tree[now.son[2]].val;
				now.dtype=Tree[now.son[0]].dtype;//获得类型
				break;
			case 40://simple-exp = term
				// cout<<40<<endl;
				deal(now.son[0]);//term
				now.val=Tree[now.son[0]].val;//获得值
				now.dtype=Tree[now.son[0]].dtype;//获得类型
				break; 
			case 44://term = factor
				// cout<<44<<endl;
				now.val=Tree[now.son[0]].val;//获得值
				now.dtype=Tree[now.son[0]].dtype;//获得类型
				break;
			default:
				break;
        }
		Tree[num]=now;
	}
	void output(ofstream &out){
		for(int i=0;i<Tree.size();i++){
			node x=Tree[i];
			out<<"节点号:"<<i<<endl;
			out<<x.tokenName<<endl;
			out<<x.produceNum<<endl;
			out<<"val: "<<x.val<<endl;
			out<<x.dtype<<endl;
			for(auto y:x.son){
				out<<y<<' ';
			}
			if(x.son.size()==0)out<<"noson";
			out<<endl;
		}
	}
};
int main()
{
	ifstream in("semantic-input.txt", ios::in);
	ofstream out("semantic-output.txt", ios::out);
	Semantic S;
	S.input(in);
	S.deal(0);
	S.output(out);
	in.close();
	out.close();
	return 0;
}