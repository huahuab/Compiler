#include<bits/stdc++.h>
using namespace std;

double Cal(double a, char op, double b) {
	if (op == '+') return a + b;
	if (op == '-') return a - b;
	if (op == '*') return a * b;
	if (op == '/') return a / b;
	cout<<"程序出错"<<endl;
	return -1;
}
int main()
{
	freopen("input.txt", "r", stdin);
	freopen("output.txt", "w", stdout);
	map <char, int> ops;
	ops['+'] = 0, ops['-'] = 0, ops['*'] = 1, ops['/'] = 1, ops['#'] = -1;
	string s;
	bool f=0;
	while (cin >> s) {
		s+="#";
		stack <double> num;
		stack <char> op;
		op.push('#');
		int i = 0;
		char c = s[i++];
		char x = op.top();
		while (x!='#'||c!='#') {
			if (ops.count(c)) {
				if (ops[x] < ops[c]) {
					op.push(c);
					c = s[i++];
				}
				else {
					x = op.top();
					op.pop();
					double b = num.top();
					num.pop();
					double a = num.top();
					num.pop();
					num.push(Cal(a, x, b));
				}
			}
			else if (c >= '0' && c <= '9') {
				int d = 0;
				while (c >= '0' && c <= '9')
				{
					d = d * 10 + c - '0';
					c = s[i++];
				}
				num.push((double)d);
			}
			else
			{
				cout << "出现非法字符" << endl;
				f=1;
				break;
			}
			x = op.top();
		}
		if(f) continue;
		if (num.empty()) {
			cout << "表达式不正确" << endl;
			continue;
		}
		cout << num.top() << endl;
	}
	return 0;
}