#include<bits/stdc++.h>
using namespace std;

struct Grammar {
public:
	int num, seqnum,snum,snumLR1,snumLALR1;
	map <string, int> hash; //符号hash成数字方便处理
	vector <string> name;//为了可以用数字快速查找到符号名字
	typedef vector<int> rightPart;//右部
	typedef pair<int, rightPart> Production;
	vector <Production> productionSeq; //产生式，左部和右部
	vector <vector<int>> seqSet;//存放每个非终极符号推导式的序号
	set <int> terminal, non_terminal;//终结符号集合/非终结符号集合
	//LR(0)和SLR自动机
	struct node{//状态的节点——项目
		int num,now;//第num个式子，走到第now个符号，num==-1时，now表示以第now个符号(name[now])开始的所有式子
		bool operator < (node a)const{
			if(num==a.num)
				return now<a.now;
			return num<a.num;
		}
	};
	typedef vector <node> status;//自动机节点——项目集
	vector <status> statusSet;//自动机节点的集合
	vector<vector <int>> G;//自动机的转移函数
	map <status,int> index;//状态的哈希，获得状态的序号
	//LR(1)和LALR(1)的自动机
	struct nodeLR1{//状态的节点
		int num,now;//第num个式子，走到第now个符号，num==-1时，now表示以第now个符号(name[now])开始的所有式子
		set<int> nxt;//向前搜索的集合
		nodeLR1(){}
		nodeLR1(int _num,int _now,int _nxt){
			num=_num,now=_now,nxt.insert(_nxt);
		}
		nodeLR1(int _num,int _now,set<int> _nxt){
			num=_num,now=_now;
			for(auto x:_nxt)
				nxt.insert(x);
		}
		bool operator < (nodeLR1 a)const{
			if(num==a.num){
				if(now==a.now)
					return nxt<a.nxt;
				return now<a.now;
			}		
			return num<a.num;
		}
	};
	typedef vector <nodeLR1> statusLR1;//自动机节点——项目集
	vector <statusLR1> statusSetLR1;//自动机节点的集合
	map <statusLR1,int> indexLR1;//状态的哈希，获得状态的序号
	vector<vector <int>> GLR1;//LR1自动机的转移函数
	//LALR1相关变量
	vector<status> coreSetLALR1;//记录每个项目的核心项
	map<status,int> indexLALR1;//hash
	vector<statusLR1> statusSetLALR1;//记录每个项目的核心项
	vector<vector<int>> GLALR1;//LALR1自动机的转移函数
	vector<vector <int>> LRmap;//LR分析表
	vector <set<int>> firstSet, followSet;//fisrt/follow集合
	Grammar() {
		num = seqnum = snum =snumLR1 = snumLALR1= 0;
		hash["epsilon"] = num++;//空 0
		hash["$"] = num++;//界符 1
		hash["start"] = num++;//唯一开始符号 2
		name.push_back("epsilon");
		name.push_back("$");
		name.push_back("start");
		vector<int> a{3};//读入的第一个符号对应的数字一定是3，即使后面不修改也是对的
		productionSeq.push_back(make_pair(2,a));//为了拓广文法，先插入一个式子，作为开始的式子，一会修改右部为输入第一个式子的左部
		seqnum++;
	}
	int find(string s) { //返回符号s对应的数字
		if (!hash.count(s)) {
			name.push_back(s);
			hash[s] = num++;
		}
		return hash[s];
	}
	bool appand(string s) { //添加产生式，把同一个符号开始的产生式分割，| 隔开算多个式子
		stringstream ss; ss << s;
		string l, r; //l是代表左部，r代表右部
		Production p;//定义一个产生式节点
		ss >> l;
		p.first = find(l); //赋值给左边
		ss >> l; //吃掉"->"
		if (l != "->") return false;
		while (ss >> r) {
			if ("|" == r) {
				if (p.second.size() == 0) return false;
				productionSeq.push_back(p);
				seqnum++;
				p.second.clear();
			}
			else {
				p.second.push_back(find(r));
			}
		}
		productionSeq.push_back(p);
		seqnum++;
		return true;
	}
	void extension(){//拓广文法，读入的第一个符号对应的数字一定是3，即使不运行也是对的
		productionSeq[0].second[0]=productionSeq[1].first;
	}
	void divideSeq(){//分割式子序列
		seqSet.resize(num);
		int num=0;
		for(auto x:productionSeq){
			seqSet[x.first].push_back(num++);
		}//所有左部相同的式子的序号加入到一个集合里
	}
	void judgeTerminal() {
		for (auto x : productionSeq)//左边出现的一定是非终结符
			non_terminal.insert(x.first);
		for (int i = 0; i < num; i++) {//其余的就是终结符
			if (!non_terminal.count(i))
				terminal.insert(i);
		}
	}
	void projectClosure(status &item){
		set <int> st;
		for(int i=0;i<(int)item.size();i++){//对每个项目求闭包，新的项目也会算在里面
			node x=item[i];
			if(x.num!=-1){//这项不是闭包项
				if(x.now<(int)productionSeq[x.num].second.size()){//点不在最后一个字符，now==size表明这个式子已经进行完了
					int y=productionSeq[x.num].second[x.now];//num个式子的now字符是什么
					if(non_terminal.count(y)&&!st.count(y)){//字符是非终结符，并且还未加入这个闭包项，所有他开始的符号都要加入item
						item.push_back({-1,y});//x.num==-1,是闭包项的标志
						st.insert(y);
					}
				}
			}
			else{//这项是闭包项
				for(auto p:seqSet[x.now]){
					int y=productionSeq[p].second[0];
					if(non_terminal.count(y)&&!st.count(y)){//字符是非终结符，并且还未加入这个闭包项，所有他开始的符号都要加入item
						item.push_back({-1,y});//x.num==-1,是闭包项的标志
						st.insert(y);
					}
				}
			}
		}
	}
	status getNxtStatus(status now,int c){
		status nxt;
		for(auto x:now){
			if(x.num==-1){//闭包项，把所有产生式都加入集合
					for(auto p:seqSet[x.now]){//取出每一个式子,p是式子编号
						int y=productionSeq[p].second[0];
						if(y==c)//添加式子 
							nxt.push_back({p,1});
					}
				}
				else{//非闭包项
					if(x.now<(int)productionSeq[x.num].second.size()){//不是结束
						int y=productionSeq[x.num].second[x.now];//num个式子的now字符是什么
						if(y==c)//添加式子
							nxt.push_back({x.num,x.now+1}); 
					}
				}
		}
		projectClosure(nxt);
		sort(nxt.begin(),nxt.end());
		return nxt;
	}
	void createAutomaton(){//构造活前缀自动机
        status zero{{0,0}};//0号式子开始，符号0;
		projectClosure(zero);
		sort(zero.begin(),zero.end());
		statusSet.push_back(zero);
		index[zero]=snum++;
		for(int i=0;i<(int)statusSet.size();i++){//对每个状态进行扩展
			G.push_back(vector <int>(num,-1));
			status now=statusSet[i];//取出状态，进行扩展
			set <int> st;
			for(auto x:now){
				if(x.num==-1){//闭包项
					for(auto p:seqSet[x.now]){//取出每一个式子,p是式子编号
						int y=productionSeq[p].second[0];
						if(y!=0) st.insert(y);//epsilon不用加
					}
				}
				else{//非闭包项
					if(x.now<(int)productionSeq[x.num].second.size()){//不是结束
						int y=productionSeq[x.num].second[x.now];//num个式子的now字符是什么
						if(y!=0) st.insert(y);//epsilon不用加
					}
				}
			}
			for(auto x:st){
				status nxt=getNxtStatus(now,x);
				if(!index.count(nxt))//状态没出现过
					index[nxt]=snum++,statusSet.push_back(nxt);
				G[i][x]=index[nxt];//状态转移
			}
		}
    }
    void getFirst() {
        firstSet.resize(num);
        for (auto x : terminal)
            firstSet[x].insert(x);
        while (1) {
            bool f = 1;
			vector <rightPart> vec;
			for(auto now:non_terminal){//每个非终极符号求first
				for (auto p : productionSeq)//求出这个非终极符号的所有产生式右部
					if(p.first==now)
						vec.push_back(p.second);
                for (auto rp : vec) { //从右侧多个式子的集合取出一个式子
                    bool hasepilon = true;
                    for (auto x : rp) { //取出该式子的每一个元素
                        if (!hasepilon) 
                            break; //上一个符号的first没有epsilon可以直接跳出
                        else
                            hasepilon = false;
                        for (auto y : firstSet[x]) { //集合中每个数都要判断,
                            if (y == 0) {            // epsilon的数字
                                hasepilon = true;
                            } else if (!firstSet[now].count(y)) { //插入新的符号
                                f = 0, firstSet[now].insert(y);
                            }
                        }
                    }
                    if (hasepilon) //每个都含有epsilon
                        firstSet[now].insert(0);	
				}
			}
            if (f)
                break;
        }
    }
    void Toepsilon(vector<bool> &flag) {
        set<int> residual;
        for (int i = 0; i < num; i++) {
            if (*(firstSet[i].begin()) == 0) //含有epsilon,则定义为true
                flag[i] = true;
            else
                residual.insert(i);
        }
        bool f = 1;
        while (f) {
            f = 0;
            for (auto it = residual.begin(); it != residual.end();) {
                int k = *it;
                bool ff = 0;
                for (auto x : firstSet[k]) {
                    if (flag[x] == true) { // k推出来的符号中能推出epsilon的
                        ff = 1;
                        break;
                    }
                }
                if (ff) {
                    flag[k] = 1;
                    f = 1;
                    residual.erase(it);
                    it++;
                } else
                    it++;
            }
        }
    }
    void getFollow() {
        vector<bool> flag(num + 5, 0);
        Toepsilon(flag);
        followSet.resize(num);
        followSet[productionSeq[0].first].insert(1); //开始字符加入"$",'$'提前定义好是1了
        while (1) {
            bool f = 1;
            for (auto p : productionSeq) {
				rightPart rp = p.second; //一条只有一个式子
				int lenrp = rp.size();
				for (int i = 0; i < lenrp; i++) { //取出该式子的每一个元素
					if (terminal.count(rp[i]))
						continue; //终止符号没有follow
					if (i < lenrp - 1) {
						for (auto x : firstSet[rp[i +1]]) { //后一个的first集合除了epsilon加入左边
							if (x != 0 &&
								!followSet[rp[i]].count(x)) { // follow里没有epsilon，其余符号没有就插入
								f = 0;
								followSet[rp[i]].insert(x);
							}
						}
					}
					if (i == lenrp - 1 ||flag[rp[i + 1]] ==1) { //是最后一个或者后面的符号可以推出来epsilon就和推导式左边的符号的follow有关
						for (auto x :followSet[p.first]) { //遍历左侧的follow集合
							if (x != 0 &&
								!followSet[rp[i]].count(
									x)) { // follow里没有epsilon，其余符号没有就插入
								f = 0;
								followSet[rp[i]].insert(x);
							}
						}
					}
				}
            }
            if (f)
                break;
        }
    }
	bool judgeSLR(status now){
		set <int> st;
		for(auto x:now){
			if(x.num!=-1){//非闭包项，闭包项不会规约，不需要考虑
				if(x.now<(int)productionSeq[x.num].second.size()){//不是结束，即不是规约项
					int y = productionSeq[x.num].second[x.now];
					if(st.count(y))
						return false;
					else st.insert(y);	
				}
				else{//出现规约项,式子左侧的follow集合
					for(auto y:followSet[x.num]){
						if(st.count(y))
							return false;
						else st.insert(y);	
					}
				}
			}
			else{//闭包项
				for(auto p:seqSet[x.now]){//取出每一个式子,p是式子编号
					int y=productionSeq[p].second[0];
					if(y!=0&&st.count(y))
						return false;
					else st.insert(y);
				}

			}
		}
		return true;
	}
    int judgeLRtype(){
		getFirst();
		getFollow();
		int type=1;
		for(int i=0;i<snum;i++){
			if(statusSet[i].size()==1)continue;//只有一个式子，不会存在冲突
			status now=statusSet[i];
			for(auto x:now){
				if(x.num!=-1){//非闭包项，闭包项不会规约，不需要考虑
					if(x.now<(int)productionSeq[x.num].second.size()){//不是结束，即不是规约项
						continue;
					}
					else{//出现规约项
						type=2;
						if(!judgeSLR(now)){
							// cout<<i<<endl;
							return 3;
						}
						break;
					}
				}
			}
		}
		return type;
	}
	void getLR0map() {//获取LR(0)分析表
		LRmap.clear();
        for (int i = 0; i < snum; i++) {
            LRmap.push_back(vector<int>(num, -1));
            bool f = 1;
            for (int j = 0; j < num; j++) {//填入移入S
                if (G[i][j] != -1) {
                    LRmap[i][j] = G[i][j];
                    f = 0;
                }
            }
            if (f) { //没有移入，可能是规约(LR0 一定是规约,且只有一个项)
                node x = statusSet[i][0];
                if (x.now != (int)productionSeq[x.num].second.size()) {//没有移入还不是规约，说明出现了错误，不是LR(0)文法
                    cout << "error" << endl;
                    return;
                }
                if (productionSeq[x.num].first == 2) { //规约完是开始符号
                    LRmap[i][1] = -2; //碰到界符是可接受的，-2表示可接收状态
                } 
				else {
                    for (auto k : terminal) {
                        LRmap[i][k] = x.num + 100000; //超过100000就是规约行为
                    }
                }
            }
        }
    }
    void getSLR1map() {//获取SLR(1)分析表
		LRmap.clear();
        for (int i = 0; i < snum; i++) {
            LRmap.push_back(vector<int>(num, -1));
            for (int j = 0; j < num; j++) {//填入移入
                if (G[i][j] != -1)
                    LRmap[i][j] = G[i][j];
            }
            for (auto x : statusSet[i]){//填入规约项目
				if (x.now != (int)productionSeq[x.num].second.size())//不是规约项目
                	continue;
				int l=productionSeq[x.num].first;//取出左部
				if ( l == 2) { //规约完是开始符号
					LRmap[i][1] = -2;//碰到界符是可接受的，-2表示可接收状态
				} 
				else {
					for (auto k : followSet[l]) {
						LRmap[i][k] = x.num + 100000; //超过100000就是规约行为
					}
				}
			}
		}
    }
    void projectClosure2(statusLR1 &item) {
        set<int> st;
        for (int i = 0; i < (int)item.size();i++) { //对每个项目求闭包，新的项目也会算在里面
            nodeLR1 x = item[i];
			if(x.num!=-1){//这项不是闭包项
				int siz=productionSeq[x.num].second.size();
				if(x.now<siz){//点不在最后一个字符，now==size表明这个式子已经进行完了
					//字符是非终结符，并且还未加入这个闭包项，所有他开始的符号都要加入item，分别计算first(βa)
					//β是y的下一个字符，a是向前搜索符号的集合，β的first含有epsilon，就加上a，否则就只有first(β)，y是最后一个字符(now==size-1)相当于β==epsilon，直接传递a
					int y=productionSeq[x.num].second[x.now];//num个式子的now字符是什么
					if(non_terminal.count(y)&&!st.count(y)){//字符是非终结符，并且还未加入这个闭包项，所有他开始的符号都要加入item,要计算新的向前搜索符号
						if(x.now == siz-1)//y是最后一个字符(now==size-1)相当于β==epsilon，直接传递a
							item.push_back({-1,y,x.nxt});//x.num==-1,是闭包项的标志
						else{
							int b=productionSeq[x.num].second[x.now+1];
							set <int> nnxt=firstSet[b];
							if(nnxt.count(0))//first(β)包含epsilon，把x.nxt(a)加进去
								for(auto c:x.nxt)
									nnxt.insert(c);
							item.push_back({-1,y,nnxt});//x.num==-1,是闭包项的标志
						}
						st.insert(y);
					}
				}
			}
			else{//这项是闭包项
				for(auto p:seqSet[x.now]){//以x.now开始的每个式子，如果是第一项是非终极符号，就加入
					int y=productionSeq[p].second[0];
					int siz=productionSeq[p].second.size();
					if(non_terminal.count(y)&&!st.count(y)){//字符是非终结符，并且还未加入这个闭包项，所有他开始的符号都要加入item，既加入一个闭包项
						if(0 == siz-1)//y是最后一个字符(0==size-1)相当于β==epsilon，直接传递a
							item.push_back({-1,y,x.nxt});//x.num==-1,是闭包项的标志
						else{
							int b=productionSeq[p].second[1];
							set <int> nnxt=firstSet[b];
							if(nnxt.count(0))//first(β)包含epsilon，把x.nxt(a)加进去
								for(auto c:x.nxt)
									nnxt.insert(c);
							item.push_back({-1,y,nnxt});//x.num==-1,是闭包项的标志
						}
						//x.num==-1,是闭包项的标志
						st.insert(y);
					}
				}
			}
		}
    }
    statusLR1 getNxtStatus2(statusLR1 now, int c) {
        statusLR1 nxt;
        for (auto x : now) {
            if (x.num == -1) { //闭包项，把所有产生式都加入集合
                for (auto p : seqSet[x.now]) { //取出每一个式子,p是式子编号
                    int y = productionSeq[p].second[0];
                    if (y == c) //添加式子
                        nxt.push_back({p, 1,x.nxt});
                }
            } else {//非闭包项
                if (x.now < (int)productionSeq[x.num].second.size()) { //不是结束
                    int y = productionSeq[x.num].second[x.now]; // num个式子的now字符是什么
                    if (y == c)//添加式子
                        nxt.push_back({x.num, x.now + 1,x.nxt});
                }
            }
        }
        projectClosure2(nxt);
        sort(nxt.begin(), nxt.end());
        return nxt;
    }
	void createAutomatonLR1() { //构造LR(1)活前缀自动机,带向前搜索符号
        statusLR1 zero{{0, 0, 1}}; // 0号式子开始，符号0,开始搜索符号为$  1;
        projectClosure2(zero);//求取闭包
        sort(zero.begin(), zero.end());//排序，方便hash
        statusSetLR1.push_back(zero);
        indexLR1[zero] = snumLR1++;
        for (int i = 0; i < (int)statusSetLR1.size(); i++) { //对每个状态进行扩展
            GLR1.push_back(vector<int>(num, -1));
            statusLR1 now = statusSetLR1[i]; //取出状态，进行扩展
            set<int> st;//用来存放可以扩展的符号，即 ·后面的的符号
            for (auto x : now) {
                if (x.num == -1) {                 //闭包项
                    for (auto p : seqSet[x.now]) { //取出每一个式子,p是式子编号
                        int y = productionSeq[p].second[0];
                        if (y != 0) st.insert(y); // epsilon不用加
                    }
                } 
				else { //非闭包项
                    if (x.now < (int)productionSeq[x.num].second.size()) { //不是结束
                        int y = productionSeq[x.num].second[x.now]; // num个式子的now字符是什么
                        if (y != 0) st.insert(y); // epsilon不用加
                    }
                }
            }
            for (auto x : st) {
                statusLR1 nxt = getNxtStatus2(now, x);
                if (!indexLR1.count(nxt)) //状态没出现过
                    indexLR1[nxt] = snumLR1++, statusSetLR1.push_back(nxt);
                GLR1[i][x] = indexLR1[nxt]; //状态转移
            }
        }
    }
	//创建LR1自动机的时候，de一个沙雕错误(排序的定义，以为没有影响)写的输出，写完了，发现bug已经修正了，吐血.jpg
	// void coutstatus2(statusLR1 zero){
	// 	cout<<"sbegin:"<<endl;
	// 	for(int j=0;j<(int)zero.size();j++){
	// 			if(zero[j].num==-1){
	// 				for(auto p:seqSet[zero[j].now]){
	// 					coutputProduction(p,0);
	// 					coutputNxtSet(zero[j].nxt);
	// 				}
	// 			}
	// 			else {
	// 				coutputProduction(zero[j].num,zero[j].now);
	// 				coutputNxtSet(zero[j].nxt);
	// 			}
	// 		}
	// 	cout<<"send"<<endl<<endl;
	// }
	// void coutputProduction(int x,int k){
	// 	Production p=productionSeq[x];
	// 	int now = p.first;
	// 	cout << name[now] << " -> ";
	// 	for (int i=0;i<(int)p.second.size();i++) { //右侧式子的每一个元素
	// 		if(i==k)cout<<".";
	// 		cout << name[p.second[i]] << ' ';
	// 	}
	// 	if((int)p.second.size()==k)cout<<".";
	// 	cout << endl;
    // }
    // void coutputNxtSet(set<int> a) {
    //     cout << "pre: ";
    //     for (auto x : a) {
    //        cout << name[x] << ' ';
    //     }
    //     cout << endl;
    // }
    void getLR1map() { //获取SLR(1)分析表
        LRmap.clear();
        for (int i = 0; i < snumLR1; i++) {
            LRmap.push_back(vector<int>(num, -1));
            for (int j = 0; j < num; j++) { //填入移入
                if (GLR1[i][j] != -1)
                    LRmap[i][j] = GLR1[i][j];
            }
            for (auto x : statusSetLR1[i]) { //填入规约项目
                if (x.now != (int)productionSeq[x.num].second.size()) //不是规约项目
                    continue;
                int l = productionSeq[x.num].first; //取出左部
                if (l == 2) {                       //规约完是开始符号
                    LRmap[i][1] = -2; //碰到界符是可接受的，-2表示可接收状态
                } else {
                    for (auto k : x.nxt) {
                        LRmap[i][k] = x.num + 100000; //超过100000就是规约行为
                    }
                }
            }
        }
    }
	bool getLALR1(){
		vector<int> belong(snumLR1,0),oneofLALR1;//属于哪个新节点,每个项目族第一个节点(用来比较转移)
		for(int i=0;i<snumLR1;i++){//遍历每个节点，把核心项目一样的映射到相同的新节点
			statusLR1 item = statusSetLR1[i];
			status core;
			for(auto x:item)//取出核心项
				core.push_back({x.num,x.now});
			if (!indexLALR1.count(core)) //核心项没出现过
                indexLALR1[core] = snumLALR1++,oneofLALR1.push_back(i);
			coreSetLALR1.push_back(core);
			belong[i]=indexLALR1[core];//这个核心项属于哪个项目族
		}
		//计算同一集合所有核心项的转移是不是相同的。
		bool f=0;
		for(int i=0;i<snumLR1;i++){
			if(oneofLALR1[belong[i]]==i){//当前的核心项属于的项目族编号的主元素是自己,转移肯定一样，直接跳过
				statusSetLALR1.push_back(statusSetLR1[i]);//把当前项目加入新的集合，一会把别的状态的向前搜索符号并上来,编号一定是按顺序出现的，不会出错
				GLALR1.push_back(vector<int>(num,-1));//把这个转移写进表里，别的项目要和表里的一致
				int bel=belong[i];
				for(int j=0;j<num;j++){//一共num个转移符号个数
					if(GLR1[i][j]!=-1)
						GLALR1[bel][j]=belong[GLR1[i][j]];
				}
				continue;
			}
			int man=oneofLALR1[belong[i]];//man是集合主元素编号，判断i和man的转移是否都一样
			for(int j=0;j<num;j++){//一共num个转移符号个数
			// cout<<GLALR1[man][j]<<' '<<belong[GLR1[i][j]]<<endl;
				if(GLR1[i][j]!=-1&&GLALR1[man][j]!=belong[GLR1[i][j]])//转移不同，不能构造LALR1
					{f=1;break;}
			}
			if(f) break;
			//全都相同就把向前搜索符号并上去
			int bel=belong[i];
			statusLR1 lr1=statusSetLR1[i];
			for(int j=0;j<(int)lr1.size();j++){
				for(auto y:lr1[j].nxt)
					statusSetLALR1[bel][j].nxt.insert(y);
			}
		}
		if(f) return false;
		return true;
	}
	void getLALR1map() { //获取SLR(1)分析表
        LRmap.clear();
        for (int i = 0; i < snumLALR1; i++) {
            LRmap.push_back(vector<int>(num, -1));
            for (int j = 0; j < num; j++) { //填入移入
                if (GLR1[i][j] != -1)
                    LRmap[i][j] = GLALR1[i][j];
            }
            for (auto x : statusSetLALR1[i]) { //填入规约项目
                if (x.now != (int)productionSeq[x.num].second.size()) //不是规约项目
                    continue;
                int l = productionSeq[x.num].first; //取出左部
                if (l == 2) {                       //规约完是开始符号
                    LRmap[i][1] = -2; //碰到界符是可接受的，-2表示可接收状态
                } else {
                    for (auto k : x.nxt) {
                        LRmap[i][k] = x.num + 100000; //超过100000就是规约行为
                    }
                }
            }
        }
    }
    void getLRmap(ofstream &out) {//自动判断文法类型，构造分析表
        createAutomaton();         //创建自动机
        int typ = judgeLRtype(); //判断类型(1:LR(0),2:SLR(1),3:LR(1))
		cout<<typ<<endl;
        if (typ==1)//LR0
       		getLR0map();
		else if(typ==2){//SLR1
			getSLR1map();
		}
		else{
			createAutomatonLR1();
			if(getLALR1())
				getLALR1map();
			else
				getLR1map();
		}
		output(out);
		outputStatus(out);
        outputProductionSeq(out);
        outputLRmap(out);
    }
	struct treeNode{//分析树节点
		string tokenName;//token名字
		int produceNum,val;//产生式编号，节点的值
		string dtype;//节点的数据类型
		vector<int> son;//子节点编号
		treeNode(string _tokenName,int _produceNum=-1,string _dtype="",int _val=0) {
			tokenName=_tokenName;
			produceNum=_produceNum;
			dtype=_dtype;
			val=_val;
		}
    };
	vector<treeNode> Tree;
	int treeNum;
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
		stack <int> stk,tstk;
		int k = 0;
		stk.push(0);//0状态入栈
		treeNum=0;
		Tree.clear();
		Tree.push_back(treeNode("start",0));
		tstk.push(treeNum++);//开始节点对应树编号入栈
		while(1){
			int s=stk.top();
			int a=tokenSeq[k];
			// cout<<s<<' '<<a<<' '<<name[a]<<endl;
			// cout<<LRmap[s][a]<<endl;
			if(LRmap[s][a]<100000&&LRmap[s][a]>-1){//移入项目
				stk.push(a);
				stk.push(LRmap[s][a]);
				Tree.push_back(treeNode(name[a]));//创建新的分析树节点
				tstk.push(treeNum++);//节点入栈
				k++;
			}
			else if(LRmap[s][a]>=100000){//规约项目
				int x=LRmap[s][a]-100000;
				int n=productionSeq[x].second.size();
				int A=productionSeq[x].first;
				n*=2;
				outputProduction(x,-1,out);
				Tree.push_back(treeNode(name[A],x));//创建新的分析树节点，弹出的节点是它的子节点
				while(n--){
					stk.pop();
					if(n&1){
						Tree[treeNum].son.push_back(tstk.top());
						tstk.pop();
					}
				}
				reverse(Tree[treeNum].son.begin(),Tree[treeNum].son.end());
				s=stk.top();
				// cout<<"s'="<<s<<endl;
				// cout<<"GOTO"<<LRmap[s][A]<<endl;
				stk.push(s);
				stk.push(LRmap[s][A]);
				tstk.push(treeNum++);//节点入栈
			}
			else if(LRmap[s][a]==-2){
				out<<"Legal token sequence"<<endl;
				Tree[0].son.push_back(tstk.top());
				outputTree(out);
				return true;
			}
			else{
				out<<"Error:Illegal token sequence"<<endl;
				return false;
			}
		}
	}
	void outputTree(ofstream &out){
		out<<"Analysis tree:"<<endl;
		out<<"Number of nodes:"<<treeNum<<endl;
		for(auto x:Tree){
			out<<x.tokenName<<endl;
			out<<x.produceNum<<endl;
			out<<"son: ";
			for(auto y:x.son)
				out<<y<<' ';
			out<<endl;
		}
	}
	void output(ofstream &out) {
		out << "Number of symbols:";
		out << name.size() << endl;
		for (auto x : name)
			out << x << ' ';
		out << endl;
	}
	void outputProductionSeq(ofstream &out) {
		out <<"Number of Grammars:"<< seqnum << endl;
		out<<"Extension Grammar:"<<endl;
		for (auto p : productionSeq) {
			int now = p.first;
			out << name[now] << " -> ";
			for (auto x : p.second) { //右侧式子的每一个元素
				out << name[x] << ' ';
			}
			out << endl;
		}
	}
	void outputStatus(ofstream &out){
		out<<"\nnumber of status:"<<snum<<endl;
		for(int i=0;i<(int)statusSet.size();i++){
			out<<i<<endl;
			status zero=statusSet[i];
			for(int j=0;j<(int)zero.size();j++){
				if(zero[j].num==-1){
					for(auto p:seqSet[zero[j].now])
						outputProduction(p,0,out);
				}
				else outputProduction(zero[j].num,zero[j].now,out);
			}
			out<<endl;
		}
	}
	void outputProduction(int x,int k,ofstream &out){
		Production p=productionSeq[x];
		int now = p.first;
		out << name[now] << " -> ";
		for (int i=0;i<(int)p.second.size();i++) { //右侧式子的每一个元素
			if(i==k)out<<".";
			out << name[p.second[i]] << ' ';
		}
		if((int)p.second.size()==k)out<<".";
		out << endl;
    }
	void outputStatusLR1(ofstream &out){
		out<<"\nnumber of status:"<<snumLR1<<endl;
		out<<statusSetLR1.size()<<endl;
		for(int i=0;i<(int)statusSetLR1.size();i++){
			out<<i<<endl;
			statusLR1 zero=statusSetLR1[i];
			for(int j=0;j<(int)zero.size();j++){
				if(zero[j].num==-1){
					for(auto p:seqSet[zero[j].now]){
						outputProduction(p,0,out);
						outputNxtSet(zero[j].nxt,out);
					}
				}
				else {
					outputProduction(zero[j].num,zero[j].now,out);
					outputNxtSet(zero[j].nxt,out);
				}
			}
			out<<endl;
		}
	}
	void outputStatusLALR1(ofstream &out){
		out<<"\nnumber of status:"<<snumLALR1<<endl;
		out<<statusSetLALR1.size()<<endl;
		for(int i=0;i<(int)statusSetLALR1.size();i++){
			out<<i<<endl;
			statusLR1 zero=statusSetLALR1[i];
			for(int j=0;j<(int)zero.size();j++){
				if(zero[j].num==-1){
					for(auto p:seqSet[zero[j].now]){
						outputProduction(p,0,out);
						outputNxtSet(zero[j].nxt,out);
					}
				}
				else {
					outputProduction(zero[j].num,zero[j].now,out);
					outputNxtSet(zero[j].nxt,out);
				}
			}
			out<<endl;
		}
	}
	void outputNxtSet(set <int> a,ofstream &out){
		out<<"pre: ";
		for(auto x:a){
			out<<name[x]<<' ';
		}
		out<<endl;
	}
	void outputG(ofstream &out){
		out<<"Transfer function"<<endl;
		for(int i=0;i<snum;i++){
			for(int j=0;j<num;j++){
				if(G[i][j]!=-1){
					out<<i<<' '<<name[j]<<' '<<G[i][j]<<endl;
				}
			}
		}
	}
	void outputLRmap(ofstream &out){
		out<<"LRmap:"<<endl;
		int w=6;
		for(auto k:terminal)
			if(k!=0)
				out<<setw(w)<<name[k];
		for(auto k:non_terminal)
			if(k!=2)
				out<<setw(w)<<name[k];
		out<<endl;
		for(int i=0;i<(int)LRmap.size();i++){
			for(auto k:terminal){
				if(k==0) continue;
				string s;
				if(LRmap[i][k]==-2)
					s+="acc";
				else if(LRmap[i][k]<100000&&LRmap[i][k]>-1)
					s="S"+to_string(LRmap[i][k]);
				else if(LRmap[i][k]>=100000)
					s="R"+to_string(LRmap[i][k]-100000);
				out<<setw(w)<<s;
			}	
			for(auto k:non_terminal){
				if(k==2) continue;
				if(LRmap[i][k]!=-1)
					out<<setw(w)<<LRmap[i][k];
				else out<<setw(w)<<" ";
			}
			out<<endl;
		}
	}
	void outputFirst(ofstream &out) {
		out << "First:" << endl;
		for (int i = 0; i < num; i++) {
			if (terminal.count(i))
				continue; //不输出终结符
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
			if (followSet[i].size() == 0)
				continue;//不输出终结符
			out << name[i] << "  ";
			for (auto x : followSet[i])
				out << name[x] << ' ';
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
	g.extension();
	g.divideSeq();
	g.judgeTerminal();
	g.getLRmap(out);
	// g.createAutomaton();
	// g.judgeLRtype();
	// g.output(out);
	// g.outputProductionSeq(out);
	// g.getLR0map();
	// g.getSLR1map();
	// g.createAutomatonLR1();
	// if(g.getLALR1()){
	// 	out<<"Compliant with LALR1 grammar!";
	// }
	// else{
	// 	out<<"Not compatible with LALR1 grammar!";
	// 	return 0;
	// }
	// g.getLR1map();
	// g.getLALR1map();
	// g.outputFirst(out);
	// g.outputFollow(out);
    // g.outputStatus(out);
	// g.outputStatusLR1(out);
	// g.outputStatusLALR1(out);
	// g.outputG(out);
	// g.outputLRmap(out);
	g.analysis(out);
	out.close();
	return 0;
}