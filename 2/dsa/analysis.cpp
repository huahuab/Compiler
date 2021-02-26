#include<bits/stdc++.h>
using namespace std;
const int epsilon = 256;//空的定义
const int Accepting = 257;//结尾定义，即可接受状态的位置
const int bel = 258;//在minDFA过程中标记属于哪个集合

class DFA {
public:
	int df[105][260], dfaSize, typeSize;
	vector <string> Type;
	void inputType() {
		cin >> typeSize;
		int id;
		string s;
		for (int i = 0; i < typeSize; i++) {
			cin >> id >> s;
			Type.push_back(s);
		}
	}
	void inputMap() {
		memset(df, -1, sizeof df);
		cin >> dfaSize;
		int a, nxt, c;
		while (cin >> a && a != -1) {
			cin >> nxt >> c;
			df[a][nxt] = c;
		}
	}
	void input() {
		inputType();
		inputMap();
	}
	bool analysis(string s) {
		int now = 0;
		bool f=1;
		for (int i = 0; i < (int)s.size(); i++) {
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
					cout<<" error"<<endl;
					i--;
				}
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
	freopen("analysis-dfaMap-input.txt", "r", stdin);
	freopen("analysis-output.txt","w",stdout);
	DFA dfa;
	dfa.input();
	freopen("analysis-test.cpp", "r", stdin);
	string str, s;
	while (getline(cin, s)) {
		delExSpace(str, s);
	}
	dfa.analysis(str+' ');
	return 0;
}