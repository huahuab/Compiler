#include<bits/stdc++.h>
using namespace std;
const int epsilon = 256;//空的定义
const int Accepting = 257;//结尾定义，即可接受状态的位置
const int bel = 258;//在minDFA过程中标记属于哪个集合

class DFA {
public:
	int df[205][260], dfaSize, typeSize;
	vector <string> Type;
	bool haserror=false;
	void inputType(istream &in) {
		in >> typeSize;
		int id;
		string s;
		for (int i = 0; i < typeSize; i++) {
			in >> id >> s;
			Type.push_back(s);
		}
	}
	void inputMap(istream &in) {
		memset(df, -1, sizeof df);
		in >> dfaSize;
		int a, nxt, c;
		while (in >> a && a != -1) {
			in >> nxt >> c;
			df[a][nxt] = c;
		}
	}
	void input(istream &in) {
		inputType(in);
		inputMap(in);
	}
	bool analysis(string s,ofstream &eout,int x) {
		int now = 0;
		bool f=1;
		vector <int> Errors;
		for (int i = 0; i < (int)s.size(); i++) {
			// cout<<s[i]<<' ';
			int x = (int)s[i];
			if (df[now][x] != -1) { //可以继续走
				cout << s[i];
				now = df[now][x];
			}
			else if(now==0&&s[i]==' ') continue;//根节点碰到空格，跳过
			else {
				if (df[now][Accepting] != -1) {//是可接受节点，输出类型
					cout << ' ' << Type[df[now][Accepting]] << endl;
					now = 0;
					// if (s[i] == ' '||s[i] == '	') {//也是为了跳过连续的空格和tab，字符串经过处理的话，可以不写这个
					// 	while (s[i] == ' '||s[i] == '	')
					// 		i++;
					// }
					if(s[i]!=' '||s[i]!='	') i--;
				}
				else {//否则提示错误
					f=0;
					haserror=true;
					// cout<<s[i]<<" error"<<endl;
					Errors.push_back(i);
					if(now!=0)
						i--,now=0; 
				}
			}
		}
		if(Errors.size()){
			for(auto y:Errors){
				eout<<"第"<<x<<"行，第"<<y<<"个字符出错，请检查程序! ";
				for(int i=0;i<s.size();i++){
					eout<<s[i];
					if(i==y)eout<<'^';
				}
				eout<<endl;
			}	
		}
		return f;
	}
};
bool delExSpace(string &str, string s)//去除程序中的连续空格和tab
{
	for (int i = 0; i < (int)s.size(); i++) {
		if(s[i]=='	')s[i]=' ';
		str += s[i];
		if (s[i] == ' '){
			while (s[i] == ' '||s[i] == '	') i++;
			i--;
		}
	} 
	return true;
}
int main()
{
    ifstream in("analysis-dfaMap-input.txt", ios::in);
    freopen("analysis-output.txt", "w", stdout);
    DFA dfa;
    dfa.input(in);
	in.close();
	in.clear();
    in.open("analysis-test.cpp", ios::in);
    ofstream eout("lexical_errors.txt", ios::out);
    string str, s;
	int i=1;
    while (getline(in, s)) {
        delExSpace(str, s);
        dfa.analysis(str + ' ',eout,i++);
        str.clear();
	}
	cout<<dfa.haserror<<" haserror"<<endl;
	// cout<<str<<endl;
	return 0;
}