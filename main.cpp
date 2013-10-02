#include <iostream>
#include <string>
#include <vector>
#include <stack>
#include <map>
#include <cstdlib>
#include <cmath>
#include <stdexcept>

using namespace std;

namespace op{
	enum opIndex{lparen, rparen, assign, u_plus, u_minus, plus, minus, divide, times, mod, eos, operand};
}
enum Associativity{left2right, right2left};
enum operandType{identifier, pure_number, syntax_error};

//In-stack precedence
const size_t isp[]={0, 19, 3, 16, 16, 13, 13, 14, 14, 14, 0};
//Incomming precedence
const size_t icp[]={20, 19, 3, 16, 16, 13, 13, 14, 14, 14, 0};
const Associativity asso[]={left2right, left2right, right2left, right2left, left2right, left2right, left2right, left2right, left2right, left2right, left2right};
map<string, double> variable;

string getToken(const string &str, size_t *p);
op::opIndex tokenType(const string &str);
string trim(const string &str);
vector<string> Infix2Postfix(const string &str);
double calcPostfixExpr(const vector<string> &str);
operandType checkOpType(const string &);
double getOpValue(const string &);

int main(int argc, char *argv[]){
	size_t sz=0;
	string expr;
	while(1){
		cout<<">";
		cin>>expr;
		vector<string> result=Infix2Postfix(expr);
		/*for(auto &s: result){
			cout<<s;
		}*/
		cout<<"="<<calcPostfixExpr(result)<<endl;
	}
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
			for(;!opstack.empty()&&(isp[tokenType(opstack.top())]>icp[opi]||(isp[tokenType(opstack.top())]==icp[opi]&&asso[tokenType(opstack.top())]==left2right));opstack.pop())
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
	stack<string> pstack;
	for(auto &token: pExprToken){
		op::opIndex opi=tokenType(token);
		if(opi==op::operand)
			pstack.push(token);
		else{
			double op2_val, op1_val;
			string op2=pstack.top();
			op2_val=getOpValue(op2);
			pstack.pop();
			if(opi==op::u_plus||opi==op::u_minus){
				op2_val=getOpValue(op2);
				pstack.push(to_string(opi==op::u_plus?op2_val:-op2_val));
				continue;
			}

			string op1=pstack.top();
			pstack.pop();
			op1_val=getOpValue(op1);
			switch(opi){
				case op::assign:
					variable[op1]=op2_val;
					pstack.push(to_string(op2_val));
					break;
				case op::plus:
					pstack.push(to_string(op1_val+op2_val)); break;
				case op::minus:
					pstack.push(to_string(op1_val-op2_val)); break;
				case op::times:
					pstack.push(to_string(op1_val*op2_val)); break;
				case op::divide:
					pstack.push(to_string(op1_val/op2_val)); break;
				case op::mod:
					pstack.push(to_string(fmod(op1_val,op2_val))); break;
			}
		}
	}
	if(checkOpType(pstack.top())==pure_number)
		return stod(pstack.top());
	else
		return variable[pstack.top()];
}

string getToken(const string &str, size_t *p){
	string token;
	size_t sz=0;

	if(*p>=str.size())
		return (*p)++,"";
	
	try{
		stod(str.substr(*p), &sz);
	}
	catch(...){
		sz=0;
	}
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
		case '=':
			return op::assign;
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

operandType checkOpType(const string &token){
	size_t sz;
	try{
		stod(token, &sz);
	}catch(std::invalid_argument &e){
		sz=0;
	}
	if(sz==0)
		return identifier;
	if(sz>0&&sz!=token.size())
		return syntax_error;
	else
		return pure_number;
}

double getOpValue(const string &token){
	//cout<<"GetOpValue="<<token<<endl;
	switch(checkOpType(token)){
		case pure_number:
			return stod(token);
		case identifier:
			{
				auto it=variable.find(token);
				if(it==variable.cend()){
					variable[token]=0.0;
					return 0.0;
				}else
					return it->second;
			}
		case syntax_error:;
	}
}