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
	int if_else_num;
	int if_elseif_else_num;
	int level;
	vector<int> case_arr;
};

struct IgnoreList{
	bool double_slash; //   //
	bool slash_star; //     /* */
	bool quotes; //        ""
	bool macro;  //        #
	bool ignore_symbol_before; //��ǰ����ӵķ���֮ǰ��������ӵķ���
	int quote_times; // ���ų��ֵĴ���
};

/**********************�û���������***************************/ 

class UserInterface{
public:
	static void input(UserInputArg* in); //�û�����
	static void output(OutputResult* out);  //������ 
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

/**********************��ȡĿ���ļ�***************************/ 

class FileRead{
public:
	ifstream file; 
	stringstream file_content; // ��ȡ�ļ����� 
	bool findSucess(); // �ж��Ƿ��ҵ�Ŀ���ļ� 
	void openFile(UserInputArg* in); // ��������ֱ����Ŀ���ļ� 
	string readFile(); // ��ȡĿ���ļ����� 
	void closeFile(); // �ر�Ŀ���ļ� 
	
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
	file_content << file.rdbuf(); // rdbuf()��file�������е����ض���file_content��
	return file_content.str();
}

void FileRead::closeFile(){
	file.close();
}

/**********************�ؼ��ʼ���***************************/

class Count{
public:
	OutputResult out;
	IgnoreList situation;
	unordered_map<string, int> keywords_map; // �����ؼ��ʹ�ϣ�����ں������� 
	stack<string> ifelse_stack; // ����ifelseջ��������һ�ж�ջ��Ԫ�� 
	
	void GetReady(const string arr[], int size, int level);
	bool isIgnoreSymbol(const string& s, int i, IgnoreList* situation);
	void IgnoreSlash(const string& s, int i, IgnoreList* situation);
	void IgnoreMacro(const string& s, int i, IgnoreList* situation);
	void IgnoreQuote(const string& s, int i, IgnoreList* situation) ;
	void countKeywords(string s);
	void countSwitchCase(string s, int* case_arr_index);
	bool addTopStack(string s, char* c);
	void addBracket(char* c);
	void countIfElse(string s, char* c);
	void countWork(string content, int level);
};

void Count::GetReady(const string arr[], int size, int level){
	for(int i = 0; i < size; i++){
		keywords_map.emplace(arr[i],0); // 
	}
	situation = { false,false,false,false,false,0 }; // ��ʼ�� ��δ������Ҫ���Եķ��� 
	out = { level, 0,0,0,0 }; // ��ʼ�����йؼ�������Ϊ�� 

}

bool Count::isIgnoreSymbol(const string& s, int i, IgnoreList* situation){
	IgnoreMacro(s, i, situation);
	IgnoreSlash(s, i, situation);
	IgnoreQuote(s, i, situation);
	return (situation->double_slash || situation->slash_star || situation->macro || situation->quotes);
	
}

void Count::IgnoreSlash(const string& s, int i, IgnoreList* situation){
	if (s[i - 1] == '/' && s[i] == '/' && !situation->ignore_symbol_before) {  //�״�������//�� 
		situation->double_slash = true;
		situation->ignore_symbol_before = true;
	}
	if (s[i - 1] == '\n' && situation->double_slash) { // ��//�����������н��������� 
		situation->double_slash = false;
		situation->ignore_symbol_before = false;
	}
	if (s[i - 1] == '/' && s[i] == '*' && !situation->ignore_symbol_before) {  // �״�������/*�� 
		situation->slash_star = true;
		situation->ignore_symbol_before = true;
	}
	if (s[i - 1] == '*' && s[i] == '/' && situation->ignore_symbol_before) {  // ������*/������������ 
		situation->slash_star = false;
		situation->ignore_symbol_before = false;
	}	
}

void Count::IgnoreMacro(const string& s, int i, IgnoreList* situation){
	if (s[i - 1] == '#' && !situation->ignore_symbol_before) {  // �״�������#��
		situation->macro = true;
		situation->ignore_symbol_before = true;
	}
	if (s[i - 1] == '\n' && situation->macro) { // ��#�����������н��������� 
		situation->macro = false;
		situation->ignore_symbol_before = false;
	}
} 

void Count::IgnoreQuote(const string& s, int i, IgnoreList* situation) {
	//ǰ��û�������ú��Ե��ַ� �� ǰ���Ѿ���һ����ƥ�������
	if (s[i - 1] == '"' && (!situation->ignore_symbol_before || situation->quote_times == 1)){
		situation->quotes = !situation->quotes; //�������������һ���ģ�����ֻ��ȡ��
		
		situation->quote_times = (situation->quote_times + 1) % 2; //�����ν��Ϊ1��ż���ν��Ϊ0
		if (situation->quote_times == 0) {
			situation->ignore_symbol_before = false; // �ɶ����� �������� 
		}
		else
		{
			situation->ignore_symbol_before = true; //  �������� ����ing 
		}
	}
}

void Count::countKeywords(string s){
	if(keywords_map.find(s) != keywords_map.end()){ //����mapֵ���Ҳ�����Ӧֵ����0���ҵ���Ӧֵ����1 
		out.keywords_num++;
	}
}

void Count::countSwitchCase(string s, int* case_arr_index) {
	if (s == "switch") {
		out.switch_num++;
		out.case_arr.push_back(0); // ����������һ�� �������ռ俪ʼ���case������ 
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
	if (s == "else"){
		
	}
	return 1; 
}

void Count::addBracket(char* c){
	string s(1, *c); // һλһλ�ж��ַ��� 
	if (*c == '{') {
		
		ifelse_stack.push(s); // ��ӡ�{�� 
	}
	if (*c == '}') {
		if (ifelse_stack.top() == "{") {
			ifelse_stack.pop(); // ɾȥ��{�� 
		}
		else {
			ifelse_stack.push(s); // ��ӡ�}�� 
		}
	}
}

void Count::countIfElse(string s, char* c){
	if(!addTopStack(s, c)){ // ����else 
		bool else_if = 0; 
		while(ifelse_stack.top() != "if"){
			if(ifelse_stack.top() == "}"){
				do {
					ifelse_stack.pop();
				} while (ifelse_stack.top() != "{");
				ifelse_stack.pop(); // ɾ������{�� 
			}
			if(ifelse_stack.top() == "elseif"){
				else_if = 1;
				ifelse_stack.pop();
				
			} 
		}
		ifelse_stack.pop(); // ɾȥջ��if
		if (else_if) {
			out.if_elseif_else_num++;
		}
		else {
			out.if_else_num++;
		}
	}
}

void Count::countWork(string content, int level){
	int index = 0;
	int switch_num_temp = 0;
	int case_arr_index = -1;
	string word;
	
	for (int i = 1; i < content.size(); i++){
		if(isIgnoreSymbol(content, i, &situation)){
			continue; // ����������Ҫ���Ե���� 
		}		
		if (!isalpha(content[i - 1]) && isalpha(content[i])) {
			index = i;
		}
		if (isalpha(content[i - 1]) && !isalpha(content[i])) {   
			word = content.substr(index, i - index); // �����ж��Ƿ�ΪӢ����ĸ��ȡ���� E.g. /nif(){} ��ȡ��if�� 
			
			countKeywords(word);
			countSwitchCase(word, &case_arr_index);
			countIfElse(word, &content[i]);
		}
		addBracket(&content[i - 1]);
	}
}






























