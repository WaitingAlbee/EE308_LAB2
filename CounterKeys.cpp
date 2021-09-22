#include<iostream> 
#include <string>
#include <sstream>
#include <vector>
#include <fstream>
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
	vector<int> case_arr;
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




























