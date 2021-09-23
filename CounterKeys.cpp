#include<iostream> 
#include <string>
#include <sstream>
#include <vector>
#include <fstream>
#include <unordered_map>
#include <stack> 
using namespace std;

const string keywords[] = { "auto","break","case","char","const","continue","default"
,"do","double","else","enum","extern","float","for","goto","if"
,"int","long","register","return","short","signed","sizeof","static"
,"struct","switch","typedef","union","unsigned","void","volatile","while" };

struct UserInputArg{
	string filepath;
	int level; 
};

struct OutputResult{
	int keywords_num;
	int switch_num;
	vector<int> case_arr;
	int if_else_num;
	int if_elseif_else_num;
	int level;
};

/**********************用户操作界面***************************/ 

class UserInterface{
public:
	static void input(UserInputArg* in); //用户输入
	static void output(OutputResult* out);  //结果输出 
};

void UserInterface::input(UserInputArg* in){
	cout << "Please enter the full path of the target file:";
	cin >> in->filepath;
	cout << "Please enter the target level:";
	cin >> in->level;
}

void UserInterface::output(OutputResult* out){
	if (out->level >= 1) {
		cout << "total num: " << out->keywords_num << endl;
	}
	if (out->level >= 2) {
		cout << "switch num: " << out->switch_num << endl;
		cout << "case num: ";
		if (out->case_arr.empty()) {
			cout << 0 <<endl;
		}
		else
		{
			for (int i = 0; i < out->case_arr.size(); i++) {
				cout << out->case_arr[i] << " ";
			}
			cout << endl;
		}
	}
	if (out->level >= 3) {
		cout << "if-else num: " << out->if_else_num << endl;
	}
	if (out->level == 4) {
		cout << "if-elseif-else num: " << out->if_elseif_else_num << endl;
	}

}

/**********************读取目标文件***************************/ 

class FileRead{
public:
	ifstream file; 
	stringstream file_content; // 读取文件内容 
	bool findSucess(); // 判断是否找到目标文件 
	void openFile(UserInputArg* in); // 重新输入直到打开目标文件 
	string readFile(); // 读取目标文件内容 
	void closeFile(); // 关闭目标文件 
	
}; 

bool FileRead::findSucess(){
	if(file.fail()){
		return 0;
	}
	else{
		return 1;
	}
} 

void FileRead::openFile(UserInputArg* in){
	file.open(in->filepath);
	while(!findSucess()){
		cout << "The file is unsucessfully found."
			<< "Please re-enter the correct filepath." << endl;
		UserInterface::input(in);
		file.open(in->filepath);	
	}
	
}

string FileRead::readFile(){
	file_content << file.rdbuf(); // rdbuf()把file流对象中的流重定向到file_content上
	return file_content.str();
}

void FileRead::closeFile(){
	file.close();
}

/**********************关键词计数***************************/

class Count{
public:
	OutputResult out;
	unordered_map<string, int> keywords_map; // 构建关键词哈希表，便于后续查找 
	stack<string> ifelse_stack; // 创建ifelse栈容器，逐一判断栈顶元素 
	void countKeywords(string s);
	void countSwitchCase(string s, int* case_arr_index);
	bool addTopStack(string s, char* c);
	void addBracket(char* c);
	void countIfElse(string s, char* c);
};

void Count::countKeywords(string s){
	if(keywords_map.find(s) != keywords_map.end()){ //搜索map值，找不到对应值返回0，找到对应值返回1 
		out.keywords_num++;
	}
}

void Count::countSwitchCase(string s, int* case_arr_index) {
	if (s == "switch") {
		out.switch_num++;
		out.case_arr.push_back(0); // 在数组后添加一项 （创建空间） 
		(*case_arr_index)++;
	}
	else if (s == "case") {
		out.case_arr[*case_arr_index]++;
	}
}

bool Count::addTopStack(string s, char* c){
	if (s == "if"){
		ifelse_stack.push("if");
	}
	return 1; 
}

void Count::addBracket(char* c){
	string s(1, *c); // 一位一位判断字符串 
	if (*c == '{') {
		
		ifelse_stack.push(s); // 添加“{” 
	}
	if (*c == '}') {
		if (ifelse_stack.top() == "{") {
			ifelse_stack.pop(); // 删去“{” 
		}
		else {
			ifelse_stack.push(s); // 添加“}” 
		}
	}
}

void Count::countIfElse(string s, char* c){
	if(!addTopStack(s, c)){ // 遇到else 
		bool else_if = 0; 
		while(ifelse_stack.top() != "if"){
			if(ifelse_stack.top() == "}"){
				do {
					ifelse_stack.pop();
				} while (ifelse_stack.top() != "{");
				ifelse_stack.pop(); // 删除到“{” 
			}
			if(ifelse_stack.top() == "elseif"){
				else_if = 1;
				ifelse_stack.pop();
				
			} 
		}
		ifelse_stack.pop(); // 删去栈顶if
		if (else_if) {
			out.if_elseif_else_num++;
		}
		else {
			out.if_else_num++;
		}
	}
}




























