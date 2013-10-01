#include <iostream>
#include <string>
#include <vector>
#include <stack>
#include <cstdlib>
#include <cmath>

using namespace std;

namespace op{
	enum opIndex{lparen, rparen, u_plus, u_minus, plus, minus, divide, times, mod, eos, operand};
	enum Associativity{left_asso, right_asso};
}
//In-stack precedence
const size_t isp[]={0, 19, 16, 16, 13, 13, 14, 14, 14, 0};
//Incomming precedence
const size_t icp[]={20, 19, 16, 16, 13, 13, 14, 14, 14, 0};
const op::Associativity opasso[]={left_asso};

string getToken(const string &str, size_t *p);
double stod(const string &str, size_t *sz);
op::opIndex tokenType(const string &str);
string trim(const string &str);
vector<string> Infix2Postfix(const string &str);
double calcPostfixExpr(const vector<string> &str);

int main(int argc, char *argv[]){
	size_t sz=0;
	string expr;
	cin>>expr;
	vector<string> result=Infix2Postfix(expr);
	for(auto &s: result){
		cout<<s;
	}
	cout<<"="<<calcPostfixExpr(result);
	return 0;
}

vector<string> Infix2Postfix(const string &str){
	vector<string> result;
	stack<string> opstack;

	size_t p=0;
	string token=getToken(str, &p);

	//p refer to "next reading position", so when p==str.size(), token=last token before eos.
	while(p<=str.size()){
		const op::opIndex opi=tokenType(token);
		//cout<<"??????"<<opi;
		if(opi==op::operand){
			result.push_back(token);
		}else if(opi==op::rparen){
			//unstack until lparen
			for(;!opstack.empty()&&opstack.top()!="(";opstack.pop()){
				result.push_back(opstack.top());
			}
			//unstack '('
			opstack.pop();
		}else{
			for(;!opstack.empty()&&isp[tokenType(opstack.top())]>=icp[opi];opstack.pop())
				result.push_back(opstack.top());
			opstack.push(token);
		}
		token=getToken(str, &p);
	}

	//Unstack remaining operator...
	for(;!opstack.empty(); opstack.pop())
		result.push_back(opstack.top());
	return result;
}

double calcPostfixExpr(const vector<string> &pExprToken){
	stack<double> pstack;
	size_t garbage;
	for(auto &token: pExprToken){
		op::opIndex opi=tokenType(token);
		if(opi==op::operand)
			pstack.push(stod(token, &garbage));
		else{
			double op2=pstack.top();
			pstack.pop();
			double op1;
			switch(opi){
				case op::plus:
					op1=pstack.top();
					pstack.pop();
					pstack.push(op1+op2); break;
				case op::u_plus:
					pstack.push(op2); break;
				case op::minus:
					op1=pstack.top();
					pstack.pop();
					pstack.push(op1-op2); break;
				case op::u_minus:
					pstack.push(-op2); break;
				case op::times:
					op1=pstack.top();
					pstack.pop();
					pstack.push(op1*op2); break;
				case op::divide:
					op1=pstack.top();
					pstack.pop();
					pstack.push(op1/op2); break;
				case op::mod:
					op1=pstack.top();
					pstack.pop();
					pstack.push(fmod(op1,op2)); break;
			}
		}
	}
	return pstack.top();
}

string getToken(const string &str, size_t *p){
	string token;
	size_t sz;

	stod(str.substr(*p), &sz);
	if(str[*p]=='+'||str[*p]=='-'){
		if(*p==0||str[*p-1]=='(')
			return string(1, str[(*p)++])+"u";
		else
			return string(1, str[(*p)++]);
	}
	if(sz>0){
		token=str.substr(*p, sz);
		*p+=sz;
	}else{
		token=str[(*p)++];
	}

	return trim(token);
}

double stod(const string &str, size_t *sz){
	char *sz2;
	double result=strtod(str.c_str(), &sz2);
	*sz=sz2-str.c_str();
	return result;
}

op::opIndex tokenType(const string &str){
	switch(str[0]){
		case '+':
			return (str.size()==1)?(op::plus):(op::u_plus);
		case '-':
			return (str.size()==1)?(op::minus):(op::u_minus);
		case '*':
			return op::times;
		case '/':
			return op::divide;
		case '(':
			return op::lparen;
		case ')':
			return op::rparen;
		case '%':
			return op::mod;
		default:
			return op::operand;
	}
	return op::operand;
}

string trim(const string &str){
	const size_t start=str.find_first_not_of(' ');
	const size_t count=str.find_last_not_of(' ')-start+1;
	//count =end-start+1
	return str.substr(start, count);
}