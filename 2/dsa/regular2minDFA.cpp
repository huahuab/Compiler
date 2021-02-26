#include <bits/stdc++.h>
using namespace std;
const int epsilon = 256;//空的定义
const int Accepting = 257;//结尾定义，即可接受状态的位置
const int bel = 258;//在minDFA过程中标记属于哪个集合
class NFA {
public:
    vector<int> nf[305][260];
    int NfaId;
    NFA() {
        memset(nf, -1, sizeof nf);
        NfaId = 2;
    }
    void link(int s, int ch, int t, int id) {//起始状态是s，当前字符为ch，下一个状态为t，id是为了标记改状态是哪个可接受状态
        if(ch>=300) ch-=300;//将字符还原回ascill
        nf[s][ch].push_back(t);
        if (ch == epsilon && t == 1)
            nf[s][Accepting].push_back(id);
    }
    void strToNFA(string s, int id)
    {
        stack<int> s_st;
        stack<int> s_ed;
        stack<int> s_str;
        s_str.push('$');
        s_st.push(0);
        s_ed.push(1);
        for (int i = 0; i < (int)s.size(); ++i)
        {
            int ch = s[i];
            // cout << i << ' ' << ch << endl;
            if (ch == '\\') {
                ch = s[++i]; //转义字符
                // cout<<ch<<endl;
                s_str.push(ch+300);//为了转义字符
                s_st.push(NfaId++);
            }
            else if (ch == '(') {//左括号，创建新的id并入栈
                s_ed.push(NfaId++);
                s_str.push('(');
            }
            else if (ch == ')') {//右括号，取出到左括号之间的所有字符和两个栈顶进行连接
                int ed = s_ed.top();
                int st = s_st.top();
                link(st, epsilon, ed, id);
                ch = s_str.top();
                while (ch != '(')
                {
                    int nxt = s_st.top();
                    s_st.pop();
                    int pre = s_st.top();
                    if (ch != '#')
                        link(pre, ch, nxt, id);
                    // nf[pre][(int)ch]=nxt;
                    s_str.pop();
                    ch = s_str.top();
                }
                s_str.pop();
                s_str.push('#');//为了解决括号嵌套，‘#’相当于push了括号的内容
                s_st.push(s_ed.top());
                s_ed.pop();
            }
            else if (ch == '|') {//或将前面所有的连接连起来，连接在外层的st和ed上
                int ed = s_ed.top();
                int st = s_st.top();
                // nf[st][epsilon]=ed;
                link(st, epsilon, ed, id);
                ch = s_str.top();
                while (ch != '(' && ch != '$')
                {
                    int nxt = s_st.top();
                    s_st.pop();
                    int pre = s_st.top();
                    if (ch != '#')
                        link(pre, ch, nxt, id);
                    // nf[pre][(int)ch]=nxt;
                    s_str.pop();
                    ch = s_str.top();
                }
            }
            else if (ch == '*') { //闭包运算，把外层（st和ed栈顶）的起点和终点用epsilon连接起来。
                int nxt = s_st.top();
                s_st.pop();
                int pre = s_st.top();
                link(pre, epsilon, nxt, id);
                link(nxt, epsilon, pre, id);
                // nf[pre][epsilon]=nxt;
                // nf[nxt][epsilon]=pre;
                s_st.push(nxt);
            }
            else {
                s_str.push(ch);
                s_st.push(NfaId++);
            }
        }
        int ch = s_str.top();
        if (ch != '$') link(s_st.top(), epsilon, 1, id);
        while (ch != '$')//清空栈，防止出现没有比连接更低级优先级的符号出现。
        {
            int nxt = s_st.top();
            s_st.pop();
            int pre = s_st.top();
            if (ch != '#')
                link(pre, ch, nxt, id);
            // nf[pre][(int)ch] = nxt;
            s_str.pop();
            ch = s_str.top();
        }
    }
    void outputNFA() {
        cout << NfaId << endl;
        for (int i = 0; i < NfaId; i++) {
            for (int j = 0; j < 256; j++) {
                if (nf[i][j].size()) {
                    for (auto x : nf[i][j])
                        cout << i << ' ' << char(j) << ' ' << x << endl;
                }
            }
            if (nf[i][256].size()) {
                for (auto x : nf[i][epsilon])
                    cout << i << ' ' << "epsilon" << ' ' << x << endl;
            }
            if (nf[i][257].size()) {
                for (auto x : nf[i][Accepting])
                    cout << i << " Accepting " << x << endl;
            }
        }
    }
    //DFA
    int df[305][260], DfaId;
    vector<int> epsilonClosure(vector<int> pre, int a) {//求一个集合经过a的epsilon闭包
        vector <int> nxt;
        for (auto x : pre) { //把pre里的每个元素经过a的集合求出来
            if (nf[x][a].size())
                for (auto y : nf[x][a])
                    nxt.push_back(y);
        }
        sort(nxt.begin(), nxt.end()), nxt.erase(unique(nxt.begin(), nxt.end()), nxt.end());
        //求nxt数组每个元素的epsilon闭包
        set <int> st;
        queue <int> que;
        for (auto x : nxt) {
            st.insert(x);
            que.push(x);
        }
        while (!que.empty()) {
            int now = que.front();
            que.pop();
            for (auto x : nf[now][epsilon])
                if (!st.count(x)) {
                    st.insert(x);
                    que.push(x);
                }
        }
        nxt.clear();
        for (auto it = st.begin(); it != st.end(); it++)
            nxt.push_back(*it);
        return nxt;
    }
    void toDFA() {
        DfaId = 0;
        memset(df, -1, sizeof df);
        map <vector<int>, int> dfmp; //保存出现过的状态集
        queue <vector<int>> q;//队列保证集合按出现顺序加入DFA中
        vector <int> start{0};//构造开始状态集
        for (auto x : nf[0][epsilon])
            start.push_back(x);
        dfmp[start] = DfaId++; //加入开始状态集
        q.push(start);
        while (!q.empty()) {
            vector <int> now = q.front(); 
            q.pop();
            int nowID = dfmp[now], AC = 0x3f3f3f3f;
            for (auto x : now) {
                if (nf[x][Accepting].size()) {
                    AC = min(AC, nf[x][Accepting][0]); //为了保证保留字优先,取较小的Accpting(先输入的保留字，保留字id比较小，别的不会冲突)
                }
            }
            if (AC != 0x3f3f3f3f) df[nowID][Accepting] = AC;//更新过是有可接受状态的
            for (int i = 0; i < 256; i++) {
                vector <int> tmp = epsilonClosure(now, i);
                if (tmp.size()) {
                    // cout<<"i="<<char(i)<<endl;
                    // for(auto x:tmp)
                    //     cout<<x<<' ';
                    // cout<<endl;
                    if (!dfmp.count(tmp)) { //未存在过的节点
                        dfmp[tmp] = DfaId++;
                        q.push(tmp);
                    }
                    df[nowID][i] = dfmp[tmp];
                }
            }
        }
    }
    void outputDFA() {
        // cout<<"DFAMap"<<endl;
        cout << DfaId << endl;
        for (int i = 0; i < DfaId; i++) {
            for (int j = 0; j < 256; j++) {
                if (df[i][j] != -1) {
                    cout << i << ' ' << char(j) << ' ' << df[i][j] << endl;
                }
            }
            if (df[i][256] != -1) {
                cout << i << ' ' << "epsilon" << ' ' << df[i][256] << endl;
            }
            if (df[i][257] != -1) {
                cout << i << ' ' << "Accepting" << ' ' << df[i][257] << endl;
            }
        }
    }
    //minDFA
    int mdf[305][260], mDfaId;
    bool equalBel(int x, int y) {
        bool f = 1;
        for (int i = 0; i <= 256; i++) {
            if (df[x][i] != df[y][i]) {
                if (df[x][i] == -1 || df[y][i] == -1) f = 0;
                if (df[df[x][i]][bel] != df[df[y][i]][bel]) f = 0;
            }
        }
        if (df[x][Accepting] != df[y][Accepting]) f=0;
        return f;
    }
    void minDFA() {
        mDfaId = 1;
        memset(mdf, -1, sizeof mdf);
        for (int i = 0; i < DfaId; i++) {
            df[i][bel] = df[i][Accepting] + 1;
            if(mDfaId<=df[i][bel]) mDfaId=df[i][bel]+1;
        }
        bool f = true;
        while (f) { //f==fasle时，说明上次没有集合被分开
            f = false;
            vector <int> vec[305];
            for (int i = 0; i < DfaId; i++) { //将所有节点分类
                vec[df[i][bel]].push_back(i);
            }
            for (int i = 0; i < mDfaId; i++) { //对每个节点集合尝试拆分
                if (vec[i].size() == 1) //该集合只有一个节点，不再尝试拆分
                    continue;
                int newId = -1;
                for (int j = 1; j < (int)vec[i].size(); j++) {
                    if (!equalBel(vec[i][0], vec[i][j])) { //不相等则分类
                        if (newId == -1) newId = mDfaId++;
                        df[vec[i][j]][bel] = newId;
                        vec[newId].push_back(vec[i][j]);
                    }
                }
                if (newId != -1) f = true;
            }
        }
        vector <int> vec[305];
        for (int i = 0; i < DfaId; i++) //将所有节点分类
            vec[df[i][bel]].push_back(i);
        for (int i = 0; i < mDfaId; i++) {
            for (int j = 0; j < 257 ; j++){
                int x=df[vec[i][0]][j];
                if(x!=-1)
                    mdf[i][j]=df[x][bel];
            }
            mdf[i][Accepting]=df[vec[i][0]][Accepting];
        }
    }
    void outputMinDFA() {
        cout << mDfaId << endl;
        for (int i = 0; i < mDfaId; i++) {
            for (int j = 0; j <= 257; j++) {
                if (mdf[i][j] != -1) {
                    cout << i << ' ' << j << ' ' << mdf[i][j] << endl;
                }
            }
        }
        cout<<-1<<endl;
    }
    int analysis(string s){
        int now=0;
        for(int i=0;i<(int)s.size();i++){
            if(mdf[now][(int)s[i]]!=-1)
                now=mdf[now][(int)s[i]];
            else return -1;
        }
        return mdf[now][Accepting];
    }
};
class TokenType {
public:
    vector<string> Type;
    void addstr(string &s, int &id) {
        string tmp, new_s;
        int i = 0;
        while (s[i] != '=')tmp += s[i++];
        Type.push_back(tmp);
        id = Type.size() - 1;
        for (i++; i < (int)s.size(); i++) {
            if (s[i] == '\\') new_s += s[i++], new_s += s[i];
            else if (s[i] == '[') {
                for (int j = s[i + 1]; j < s[i + 3]; j++)
                    new_s += j, new_s += '|';
                new_s += s[i + 3];
                i += 4;
            }
            else new_s += s[i];
        }
        s = new_s;
    }
    void output() {
        cout << Type.size() << endl;
        for (int i = 0; i < (int)Type.size(); ++i) {
            cout << i << ' ' << Type[i] << endl;
        }
    }
};


int main()
{
    freopen("regular2minDFA-input.txt", "r", stdin);
    freopen("analysis-dfaMap-input.txt", "w", stdout);
    string str;
    NFA nfa;
    TokenType token;
    while (cin >> str) {
    // int n;
    // cin>>n;
    // while(n--){
    //     cin>>str;
        int id;
        token.addstr(str, id);
        nfa.strToNFA(str, id);
    }
    token.output();
    // nfa.outputNFA();
    nfa.toDFA();
    // cout<<"DFAOk"<<endl;
    // nfa.outputDFA();
    nfa.minDFA();
    // cout<<"mDFAOk"<<endl;
    nfa.outputMinDFA();
    return 0;
}