#include<bits/stdc++.h>
using namespace std;

struct Grammar {
public:
	int num, seqnum;
	map <string, int> hash; //符号hash成数字方便处理
	vector <string> name;//为了可以用数字快速查找到符号名字
	map <int, int> leftPos; //放一个某左部对应的产生式的位置
	typedef vector<int> rightPart;//一个右部，同一个非终极符号可能推出不同的右部，用|连接，所以每个右部需要单独定义
	typedef pair<int, vector<rightPart>> Production; //产生式，左部和多个右部组成的pair
	vector<Production> productionSeq;//产生式序列
	set <int> terminal, non_terminal;//终结符号集合/非终结符号集合
	vector<set <int>> firstSet, followSet;//fisrt/follow集合
	typedef set<int> selectPart;//一个select的结构
	vector<vector<selectPart>> selectSet;//select集合
	vector<vector<int>> LLmap;
	Grammar() {
		num = seqnum = 0;
		hash["epsilon"] = num++;
		hash["$"] = num++;//Follow要用到
		name.push_back("epsilon");
		name.push_back("$");
	}
	int find(string s) { //返回符号s对应的数字
		if (!hash.count(s)) {
			name.push_back(s);
			hash[s] = num++;
		}
		return hash[s];
	}
	bool appand_old(string s) { //备份添加产生式，这个函数按照输入确定产生式个数
		seqnum++;
		stringstream ss; ss << s;
		string l, r; //l是代表左部，r代表右部
		Production p;//定义一个产生式节点
		ss >> l;
		p.first = find(l); //赋值给左边
		ss >> l; //吃掉"->"
		if (l != "->") return false;
		rightPart rp;
		while (ss >> r) {
			if ("|" == r) {
				if (rp.size() == 0) return false;
				p.second.push_back(rp);
				rp.clear();
			}
			else {
				rp.push_back(find(r));
				// cout << r << endl;
			}
		}
		p.second.push_back(rp);//最后要push进去
		productionSeq.push_back(p);
		return true;
	}
	bool appand(string s) {//更新的添加产生式函数，会把相同的左部直接放在一起
		stringstream ss; ss << s;
		string l, r; //l是代表左部，r代表右部
		ss >> l;
		int L = find(l);
		if (!leftPos.count(L)) {
			leftPos[L] = seqnum++;
			Production p;//定义一个产生式节点
			p.first = L;
			productionSeq.push_back(p);
		}
		int lp = leftPos[L];
		ss >> l; //吃掉"->"
		if (l != "->") return false;
		rightPart rp;
		while (ss >> r) {
			if ("|" == r) {
				if (rp.size() == 0) return false;
				productionSeq[lp].second.push_back(rp);
				rp.clear();
			}
			else {
				rp.push_back(find(r));
				// cout << r << endl;
			}
		}
		productionSeq[lp].second.push_back(rp);//最后要push进去
		return true;
	}
	void judgeTerminal() {
		for (auto x : productionSeq)//左边出现的一定是非终结符
			non_terminal.insert(x.first);
		non_terminal.insert(1);//'$'算非终结符号
		for (int i = 0; i < num; i++) {//其余的就是终结符
			if (!non_terminal.count(i))
				terminal.insert(i);
		}
	}
	bool removeLeftRecursion() {
		vector <bool> recur(num, 0); //定义长为左边个数的数组，用来标志是否含有左递归，用来替换
		for (int i = 0; i < seqnum; i++) {
			Production p = productionSeq[i]; //取出当前产生式
			Production np, nxtp; //temp放替换完的式子，np放新的i位置的式子(原来的p)，nxtp放新式子
			vector <rightPart> temp = p.second, tmp;
			while (1) { // 替换掉p右式包含前面的被替换过的字符
				bool f = 1;
				for (auto rp : temp) {//从多个式子的集合取出一个式子
					int lenrp = rp.size(), j = 0;
					rightPart nrp;//new rightPart
					for (; j < lenrp; j++) { //取出该式子的每一个元素
						if (recur[rp[j]]) { //当前元素被去除过左递归
							f = 0; //替换（会出现一个式子变多个式子）
							int lp = leftPos[rp[j]];
							Production c = productionSeq[lp]; //取出对应的表达式，进行替换
							for (auto AA : c.second) {
								rightPart xx = nrp;
								for (auto aa : AA) //添加对应的分解式
									xx.push_back(aa);
								for (int k = j + 1; k < lenrp; k++) //继续添加后面的符号
									xx.push_back(rp[k]);
								tmp.push_back(xx);//push_back到temp后面
							}
							break;
						}
						nrp.push_back(rp[j]);
						if (j == lenrp - 1) //直到最后一个符号都不需要替换
							tmp.push_back(nrp);
					}
				}
				temp = tmp;
				tmp.clear();
				if (f) break; //不再变化就跳出
			}
			// cout<<name[p.first]<<' ';
			// cout<<temp.size()<<endl;
			for (int j = 0; j < (int)temp.size(); j++) { //从右侧多个式子的集合取出一个式子
				if (temp[j][0] == p.first) { //含有左递归,标记,第j个需要改写
					rightPart nrp;
					for (int k = 1; k < (int)temp[j].size(); k++) {//后面的所有字符
						nrp.push_back(temp[j][k]);
					}
					nrp.push_back(num);
					nxtp.second.push_back(nrp);
				}
				else { //不含递归的式子直接加上新的字符(β)，放进np里
					temp[j].push_back(num);
					np.second.push_back(temp[j]);
				}
			}
			if (nxtp.second.size() > 0 && np.second.size() > 0) { //np和nxtp都有右式，说明可以消除
				if (np.second.size() == 0) { //一个β都没有，文法有误，无法继续消除
					cout << "Contains irreversible recursion" << endl;
					return false;
				}
				string factors = name[p.first] + "\'";//加入新字符
				name.push_back(factors);
				hash[factors] = num;
				nxtp.first = num;
				nxtp.second.push_back(vector<int> (1, 0)); //加入epsilon
				productionSeq.push_back(nxtp);//加入新的式子
				np.first = p.first;
				productionSeq[i] = np; //替换当前式子
				num++, seqnum++;
				recur.push_back(0);//标记新加入的不存在左递归，主要是防止访问越界
				recur[p.first] = 1; //标记这个式子被替换过
			}
		}
		return true;
	}
	void leftFactoring() {
		bool f = 1;
		while (f) {
			f = 0;
			for (int i = 0; i < seqnum; i++) {
				Production p = productionSeq[i]; //取出当前产生式
				map <int, int> count;
				int mx = -1, mxcnt = 1;
				set <int> st;//用来放相同的符号的下标，遍历这个，如果不同了，就退出，即提取策略是公共部分式子越多越好
				for (auto rp : p.second) { //从右侧多个式子的集合取出一个式子
					count[rp[0]]++;//每个式子的第一个符号进行计数
					if (count[rp[0]] > mxcnt) //大于1才更新，相等可以不更新
						mxcnt = count[rp[0]], mx = rp[0];
				}
				if (mx == -1)continue; //没有公因子，直接下一个
				f = 1; //有公因子需要再跑所有式子
				for (int j = 0; j < (int)p.second.size(); j++) { //从右侧多个式子的集合取出一个式子
					if (mx == p.second[j][0]) st.insert(j); //将对应的下标加入st
				}
				// cout << name[mx] << endl;//debug
				// cout << st.size() << endl;
				int len = 1, y = *(st.begin());//公共部分长度
				while (1) {
					bool ff = 0;
					for (auto x : st) {
						if ((int)p.second[x].size() <= len) { //当前x的式子长度小于等于len,直接break;
							ff = 1; break;
						}
						else if (p.second[x][len] != p.second[y][len]) { //和第一个不相同
							ff = 1; break;
						}
					}
					if (ff) break;
					len++;
				}
				Production np, nxtp;//np替换当前的p,产生新的nxtp,
				np.first = p.first;
				//构建一个新的符号
				string factors = "factor" + to_string(num);
				name.push_back(factors);
				hash[factors] = num;
				nxtp.first = num;
				bool haseps = 1;
				for (int j = 0; j < (int)p.second.size(); j++) { //
					if (!st.count(j)) { //非公共部分直接加入np
						np.second.push_back(p.second[j]);
					}
					else { //公共因子后面的加入nxtp
						rightPart nrp;
						if (len == (int)p.second[j].size() && haseps) { //长度相等，需要加入epsilon
							haseps = 0; //避免重复加入
							nrp.push_back(0);
							nxtp.second.push_back(nrp);
						}
						else {
							for (int k = len; k < (int)p.second[j].size(); k++) {
								nrp.push_back(p.second[j][k]);
							}
							nxtp.second.push_back(nrp);
						}
					}
				}
				rightPart newr;//np加入公共因子和新符号的式子
				for (int j = 0; j < len; j++) {
					newr.push_back(p.second[y][j]);
				}
				newr.push_back(num);
				np.second.push_back(newr);
				// int now = np.first;//debug
				// cout << name[now] << " -> ";
				// for (auto rp : np.second) { //从右侧多个式子的集合取出一个式子
				// 	for (auto x : rp) { //取出该式子的每一个元素
				// 		cout << name[x] << ' ';
				// 	}
				// 	if (rp != *(np.second.end() - 1)) cout << "| ";
				// }
				// cout << endl;
				productionSeq[i] = np; //替换当前式子
				productionSeq.push_back(nxtp);//加入新的式子
				num++, seqnum++;
			}
		}
	}
	void getFirst() {
		firstSet.resize(num);
		for (auto x : terminal)
			firstSet[x].insert(x);
		while (1) {
			bool f = 1;
			for (auto p : productionSeq) {
				int now = p.first;
				// cout<<"now="<<now<<endl;
				for (auto rp : p.second) { //从右侧多个式子的集合取出一个式子
					bool hasepilon = true;
					for (auto x : rp) { //取出该式子的每一个元素
						if (!hasepilon) break; //上一个符号的first没有epsilon可以直接跳出
						else hasepilon = false;
						// if(terminal.count(x)&&!firstSet.count(x)){//当前是终结符，并且没被加入集合就加入
						// 	f=0,firstSet[now].insert(x);
						// 	break;
						// }
						// else if(non_terminal.count(x)){//当前是非终结符，将该符号的first集合中now没有的加入,同时判断是否为epsilon，是的话要继续判断下一个数
						// 	for(auto y:firstSet[x]){//集合中每个数都要判断
						// 		if(y==0){//epsilon的数字
						// 			hasepilon=true;
						// 		}
						// 		else if(!firstSet[now].count(y)){//插入新的符号
						// 			f=0,firstSet[now].insert(y);
						// 		}
						// 	}
						// }
						//其实不用区分是否为终结符号。
						for (auto y : firstSet[x]) { //集合中每个数都要判断,
							if (y == 0) { //epsilon的数字
								hasepilon = true;
							}
							else if (!firstSet[now].count(y)) { //插入新的符号
								f = 0, firstSet[now].insert(y);
							}
						}
					}
					if (hasepilon) //每个都含有epsilon
						firstSet[now].insert(0);
				}
			}
			if (f) break;
		}
	}
	void Toepsilon(vector <bool> &flag)
	{
		set <int> residual;
		for (int i = 0; i < num; i++) {
			if (*(firstSet[i].begin()) == 0) //含有epsilon,则定义为true
				flag[i] = true;
			else residual.insert(i);
		}
		bool f = 1;
		while (f) {
			f = 0;
			for (auto it = residual.begin(); it != residual.end();) {
				int k = *it;
				bool ff = 0;
				for (auto x : firstSet[k]) {
					if (flag[x] == true) { //k推出来的符号中能推出epsilon的
						ff = 1; break;
					}
				}
				if (ff) {
					flag[k] = 1;
					f = 1;
					residual.erase(it);
					it++;
				}
				else it++;
			}
		}
	}
	void getFollow() {
		vector <bool> flag(num + 5, 0);
		Toepsilon(flag);
		followSet.resize(num);
		followSet[productionSeq[0].first].insert(1);//开始字符加入"$",'$'提前定义好是1了
		while (1) {
			bool f = 1;
			for (auto p : productionSeq) {
				for (auto rp : p.second) { //从右侧多个式子的集合取出一个式子
					int lenrp = rp.size();
					for (int i = 0; i < lenrp; i++) { //取出该式子的每一个元素
						if (terminal.count(rp[i])) continue; //终止符号没有follow
						if (i < lenrp - 1) {
							for (auto x : firstSet[rp[i + 1]]) { //后一个的first集合除了epsilon加入左边
								if (x != 0 && !followSet[rp[i]].count(x)) { //follow里没有epsilon，其余符号没有就插入
									f = 0; followSet[rp[i]].insert(x);
								}
							}
						}
						if (i == lenrp - 1 || flag[rp[i + 1]] == 1) { //是最后一个或者后面的符号可以推出来epsilon就和推导式左边的符号的follow有关
							for (auto x : followSet[p.first]) {//遍历左侧的follow集合
								if (x != 0 && !followSet[rp[i]].count(x)) {//follow里没有epsilon，其余符号没有就插入
									f = 0; followSet[rp[i]].insert(x);
								}
							}
						}
					}
				}
			}
			if (f) break;
		}
	}
	void getSelect() {
		selectSet.resize(num);
		for (auto p : productionSeq) {
			for (auto rp : p.second) { //从右侧多个式子的集合取出一个式子
				selectPart sp;
				int rp0 = rp[0]; //取右边第一个元素
				for (auto x : firstSet[rp0]) { //该元素的first集合
					sp.insert(x);
					if (x == 0) { //集合里包含epsilon把式子左边的follow放进去
						for (auto y : followSet[p.first]) {
							sp.insert(y);
						}
					}
				}
				selectSet[p.first].push_back(sp);
			}
		}
	}
	bool judgeLegalSet() { //判断获得的集合是不是合法的
		for (auto spseq : selectSet) {
			if (spseq.size() <= 1) //集合为空,这个符号为终极符号，或者只有一个产生式，都不需要判定交集
				continue;
			vector <int> pre, c;
			for (auto x : spseq[0])
				pre.push_back(x);
			for (int i = 1; i < (int)spseq.size(); i++) {
				c.clear();
				set_intersection(spseq[i].begin(), spseq[i].end(), pre.begin(), pre.end(), back_inserter(c));
				pre = c;
			}
			if (c.size() > 0) return false; //出现非空集合，返回假
		}
		return true;
	}
	bool judgeLegal() {
		firstSet.clear(), followSet.clear(), selectSet.clear();
		getFirst();
		getFollow();
		getSelect();
		return judgeLegalSet();
	}
	void getLLmap() {
		LLmap.resize(num + 5);
		for (int i = 0; i < num + 5; i++)
			LLmap[i].resize(num + 5);
		for (int i = 0; i < (int)productionSeq.size(); i++) {
			Production p = productionSeq[i];
			int index = (i + 1) * 10000; //一个数字存储两个维度
			int l = p.first;
			for (int j = 0; j < (int)p.second.size(); j++) { //从右侧多个式子的集合取出一个式子
				int x = p.second[j][0]; //式子第一个数
				for (auto r : firstSet[x]) {
					if (r == 0) { //含有epsilon
						for (auto rr : followSet[l]) {
							LLmap[l][rr] = index + j;
						}
					}
					else LLmap[l][r] = index + j;
				}
			}
		}
	}
	bool analysis(ofstream &out) {
		ifstream in("tokens.in", ios::in);
		vector <int> tokenSeq;
		string s;
		while (in >> s) { //读入token序列
			if (hash.count(s)) tokenSeq.push_back(hash[s]);
			else {
				out << "Error:Illegal token entered\n";
				in.close();
				return false;
			}
		}
		in.close();
		tokenSeq.push_back(hash["$"]);
		stack <int> stk;
		int k = 0;
		stk.push(1);
		stk.push(productionSeq[0].first);//开始符号入栈
		while (stk.top() != 1 || tokenSeq[k] != 1) { //栈顶和下一个符号都不是$
			// cout<<name[stk.top()]<<endl;
			int now = stk.top();
			if (terminal.count(now) && now == tokenSeq[k]) { //栈顶终结符并且和输入相等，元素出栈，token序列前进
				stk.pop();
				k++;
			}
			else if (non_terminal.count(now) && LLmap[now][tokenSeq[k]] != 0) { //栈顶非终结符号，并且有转移的式子，把对应式子反向压栈
				stk.pop();
				int num = LLmap[now][tokenSeq[k]] / 10000 - 1, num1 = LLmap[now][tokenSeq[k]] % 10000; //第num个式子的第num1个递推式
				rightPart rp = productionSeq[num].second[num1];
				for (int i = rp.size() - 1; i >= 0; i--) { //反向压栈
					if(rp[i]!=0)//不压epsilon
						stk.push(rp[i]);
				}
			}
			else {
				out << "Error:Illegal token sequence" << endl;
				return false;
			}
		}
		if (stk.top() == 1 && tokenSeq[k] == 1) {
			out << "Legal token sequence" << endl;
		}
		else {
			out << "Error:Illegal token sequence" << endl;
			return false;
		}
		return true;
	}
	void output(ofstream &out) {
		out << seqnum << endl;
		out << endl;
		out << "Number of symbols:";
		out << name.size() << endl;
		for (auto x : name)
			out << x << ' ';
		out << endl;
		out << "Number of non_terminals:";
		out << non_terminal.size() << endl;
		for (auto x : non_terminal)
			out << name[x] << ' ';
		out << endl;
		out << "Number of terminals:";
		out << terminal.size() << endl;
		for (auto x : terminal)
			out << name[x] << ' ';
		out << endl;
	}
	void outputFirst(ofstream &out) {
		out << "First:" << endl;
		for (int i = 0; i < num; i++) {
			if (terminal.count(i)) continue; //不输出终结符
			out << name[i] << "  ";
			for (auto x : firstSet[i]) {
				// if(x!=0)
				out << name[x] << ' ';
			}
			out << endl;
		}
	}
	void outputFollow(ofstream &out) {
		out << "Follow:" << endl;
		for (int i = 0; i < num; i++) {
			if (followSet[i].size() == 0)continue;
			out << name[i] << "  ";
			for (auto x : followSet[i])
				out << name[x] << ' ';
			out << endl;
		}
	}
	void outputProductionSeq(ofstream &out) {
		for (auto p : productionSeq) {
			int now = p.first;
			out << name[now] << " -> ";
			for (auto rp : p.second) { //从右侧多个式子的集合取出一个式子
				for (auto x : rp) { //取出该式子的每一个元素
					out << name[x] << ' ';
				}
				if (rp != *(p.second.end() - 1)) out << "| ";
			}
			out << endl;
		}
	}
	void outputLLmap(ofstream &out) {
		int w = 20;
		out << "LL(1) map:" << endl;
		vector <int> ter;
		out << setw(w) << " ";
		for (auto x : terminal) {
			if (x != 0) {
				out  << setw(w) << name[x];
				ter.push_back(x);
			}
		}
		out << endl;
		for (auto l : non_terminal) {
			out << setw(w) << name[l];
			for (auto r : ter) {
				if (LLmap[l][r] != 0) {
					int k = LLmap[l][r] / 10000 - 1;
					string an = name[productionSeq[k].first] + " -> ";
					for (auto x : productionSeq[k].second[LLmap[l][r] % 10000]) {
						an += (name[x] + ' ');
					}
					out << setw(w) << an;
				}
				else out << setw(w) << " ";
			}
			out << endl;
		}
	}
};
int main()
{
	ifstream in("grammar-input.txt", ios::in);
	ofstream out("grammar-output.txt", ios::out);
	string s;
	Grammar g;
	while (getline(in, s)) {
		if (!g.appand(s)) {
			out << "Wrong grammar writing\n" << endl;
			break;
		}
	}
	in.close();
	// out << "原始式子：" << endl;
	// g.outputProductionSeq(out);
	g.removeLeftRecursion();
	g.leftFactoring();
	g.judgeTerminal();
	if (g.judgeLegal()) out << "Comply with LL (1) grammar\n";
	else out << "Does not conform to LL (1) grammar\n";
	g.getLLmap();
	g.analysis(out);
	// g.outputLLmap(out);
	// g.outputFirst(out);
	// g.outputFollow(out);
	// g.outputProductionSeq(out);
	// g.output(out);
	out.close();
	return 0;
}