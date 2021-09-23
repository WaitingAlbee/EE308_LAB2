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
int i = 1;
struct UserInputArg{
    string filepath;
    int level;
};

struct OutputResult{
    int level;
    int keywords_num;
    int switch_num;
    int if_else_num;
    int if_elseif_else_num;
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

/**********************�ؼ��ʼ���***************************/

class Count{
public:
    OutputResult out;
    IgnoreList situation;
    unordered_map<string, int> keywords_map; // �����ؼ��ʹ�ϣ�����ں�������
    stack<string> ifelse_stack; // ����ifelseջ��������һ�ж�ջ��Ԫ��

    void GetReady(const string arr[], int size, int level);
    void countKeywords(string s);
    void countSwitchCase(string s, int* case_arr_index);
    void countIfElse(string s, char* c);
    bool isIgnoreSymbol(const string& s, int i, IgnoreList* situation);
    void IgnoreSlash(const string& s, int i, IgnoreList* situation);
    void IgnoreMacro(const string& s, int i, IgnoreList* situation);
    void IgnoreQuote(const string& s, int i, IgnoreList* situation) ;
    void countWork(string content, int level);
    OutputResult* getOutput();
};

void Count::GetReady(const string arr[], int size, int level){
    for(int i = 0; i < size; i++){
        keywords_map.emplace(arr[i],0); //
    }
    situation = { false,false,false,false,false,0 }; // ��ʼ�� ��δ������Ҫ���Եķ���
    out = { level, 0,0,0,0 }; // ��ʼ�����йؼ�������Ϊ��

}

void Count::countKeywords(string s){
    if(keywords_map.find(s) != keywords_map.end()){  //ʹ��unordered_map�������Ҳ�����Ӧֵ����0���ҵ���Ӧֵ����1
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


void Count::countIfElse(string s, char* c){
    if (s == "if"){
        ifelse_stack.push(s); //����ifֱ�ӽ�ջ
    }
    else if(s == "else"){
        char* p_temp = c;
        while (*p_temp == ' ' ) {
            p_temp++;
        }
        if (*p_temp == 'i' && *(p_temp + 1) == 'f') {   //����else if��ӽ�ջ
            i+=3;
            out.keywords_num++; // �ؼ��ʼ���ʱ����һ��if������ 
            ifelse_stack.push("elseif");
        }
        else { //����else
            int flag = 0;
            while (ifelse_stack.top() != "if") // ���ջ����elseif,��ɾ������if��(if_elseif_else_num +1)��ɾȥif 
            {
                flag = 1;
                ifelse_stack.pop();
            }
            if (!flag){ //���ջ����if,(if_else_num +1)��ɾ��if 
                out.if_else_num++;
            }
            else out.if_elseif_else_num++;
            ifelse_stack.pop();

        }
    }
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
    if (s[i - 1] == '"' && (!situation->ignore_symbol_before || situation->quote_times == 1)){ 	//ǰ��û�������ú��Ե��ַ� �� ǰ���Ѿ���һ����ƥ�������
        situation->quotes = !situation->quotes; 

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

void Count::countWork(string content, int level){
    int index = 0;
    int switch_num_temp = 0;
    int case_arr_index = -1;
    string word;

    for (; i < content.size(); i++){
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
    }
}

OutputResult* Count::getOutput(){
    OutputResult* p_out = &out;
    return p_out;
}

int main(){
    int arr_size = sizeof(keywords)/sizeof (keywords[0]);
    UserInputArg in;
    UserInterface::input(&in);

    ifstream f;
    f.open(in.filepath);

    ostringstream buf;
    char c;
    while(buf && f.get(c))
    {
        buf.put(c);
    }
    string s = buf.str();

    Count count;
    count.GetReady(keywords, arr_size, in.level);
    count.countWork(s, in.level);
    UserInterface::output(count.getOutput());
    return 0;
}
