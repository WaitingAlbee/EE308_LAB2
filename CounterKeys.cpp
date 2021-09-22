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




























