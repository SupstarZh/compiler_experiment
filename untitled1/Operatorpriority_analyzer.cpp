#include "Operatorpriority_analyzer.h"
void Operatorpriority_analyzer::printLine(int n){
    for (int i = 0; i < n; ++i) {
        cout << '-';
    }
    cout << endl;
}
void Operatorpriority_analyzer::printTable(vector<char> symbolStack, vector<char> inputString, int step, char left, char priority, char right, string handle){
    string symbolStr = blank+" ", inputStr = blank+" ";
    for (int i = 0; i < symbolStack.size(); ++i) symbolStr += symbolStack[i];
    for (int i = 0; i < inputString.size(); ++i) inputStr += inputString[i];
    cout << "| " << setw(4) << step << '|' << setw(10) << symbolStr << '|' << setw(8)
         << (" " + blank + left + " " + priority + " " + right) << "|" << setw(12) << " "+handle << '|' << setw(10) << inputStr << "\n";
}
void Operatorpriority_analyzer::printV() { //打印集合和算符优先表
    ofstream out("priority_table.txt");
    out << "算符优先顺序表:\n";
    out << "     ";
    for (int i = 0; i < Vt.size(); ++i) {
        out << " " << Vt[i] << "  ";
    }
    out << " " << "#" << "  \n";
    for (int i = 0; i < Vt.size() + 1; ++i) { //输出table
        out << endl;
        out << "  ";
        if (i == Vt.size()) out << '#';
        else out << Vt[i];
        out << "  ";
        for (int j = 0; j < Vt.size() + 1; ++j) {
            out << " " <<table[i][j] << "  ";
        }
        out << endl;
    }
    out << endl;
    out.close();
}
int Operatorpriority_analyzer::get_VnNum(char c) {
    for (unsigned int i = 0; i < Vn.size(); ++i) {
        if (c == Vn[i])
            return i;
    }
    return -1;
}
int Operatorpriority_analyzer::get_VtNum(char c) {
    if (c == '#') //规约句柄时用于查找算符优先顺序表
        return Vt.size();

    for (unsigned int i = 0; i < Vt.size(); ++i) {
        if (c == Vt[i])
            return i;
    }
    return -1;
}
bool Operatorpriority_analyzer::search_List(char c, vector<char> List) {
    for (int i = 0; i < List.size(); ++i) {
        if (c == List[i])
            return true;
    }
    return false;
}
char Operatorpriority_analyzer::search_Grammar(string handle) { //归约句柄
    for (int i = 0; i < grammarList.size(); ++i) {
        if (grammarList[i].size() == 4 && grammarList[i][3] >= 'A' && grammarList[i][3] <= 'Z') continue; //忽略非终结符相互归约
        bool flag = false;
        if (handle.size() == grammarList[i].size() - 3) { //句柄长度和文法右端长度比较
            for (int j = 3; j < grammarList[i].size(); ++j) { //比较每一个字符
                //cout << grammarList[i] << endl;
                int k = j - 3;
                if (handle[k] >= 'A' && handle[k] <= 'Z' && grammarList[i][j] >= 'A'&& grammarList[i][j] <= 'Z') { //非终结符不重要
                    if (j == grammarList[i].size() - 1) {
                        //cout << grammarList[i] << endl;
                        flag = true;
                    }
                    continue;
                }
                if (handle[k] != grammarList[i][j]) { //有字符不匹配则跳出
                    flag = false;
                    break;
                }
                //cout << j << endl;
                if (j == grammarList[i].size() - 1) {
                    //cout << grammarList[i] << endl;
                    flag = true;
                }
            }
            //if (flag) break;
        }
        if (flag) { //如果匹配成功，返回该文法任意非终结符
            //cout << grammarList[i] << endl;
            return grammarList[0][0];
        }
    }
    //没有文法匹配，返回-1，说明输入串不属于该文法
    return -1;
}
bool Operatorpriority_analyzer::check_grammar(string grammar) { //检查输入的每条文法是不是算符优先文法
    if (grammar[0] < 'A' || grammar[0] > 'Z') { //检查最左非终结符是不是大写字母
        cout << "[ERROR]The leftmost non-terminal term of the grammar is not a capital letter!\n";
        return false;
    }
    for (int i = 3; i < grammar.size() - 1; ++i) { //检查右侧是否有连续的非终结符
        if (grammar[i] >= 'A' && grammar[i] <= 'Z'&&grammar[i + 1] >= 'A' && grammar[i + 1] <= 'Z') {
            cout << "[ERROR]Two non-terminal characters cannot appear consecutively on the right side of the grammar!\n";
            return false;
        }
    }
    return true;
}
void Operatorpriority_analyzer::putinVt() { //生成Vt集
    for (int i = 0; i < grammarList.size(); ++i) {
        for (int j = 3; j < grammarList[i].size(); ++j) {
            if (grammarList[i][j] >= 'A' && grammarList[i][j] <= 'Z') ;
            else {
                if (!search_List(grammarList[i][j],Vt)) {
                    Vt.push_back(grammarList[i][j]); //记录非终结符
                }
            }
        }
    }
}
void Operatorpriority_analyzer::INSERT(char U, char b) {
    int x = get_VnNum(U), y = get_VtNum(b);
    if (F[x][y] == 0) {
        F[x][y] = 1;
        STACK.push(pair<char, char>(U, b));
    }
}
void Operatorpriority_analyzer::get_firstVt() { //计算firstVT集
    for (int i = 0; i < Vn.size(); ++i) { //初始化
        for (int j = 0; j < Vt.size(); ++j) {
            F[i][j] = 0;
        }
    }
    for (int i = 0; i < grammarList.size(); ++i) { //赋初值
        char U = grammarList[i][0];
        for (int j = 3; j < grammarList[i].size(); ++j) {
            char b = grammarList[i][j];
            if (search_List(b, Vt)) { //是非终结符
                INSERT(U, b); //形如U->b...或U->Vb...
                break;
            }
        }
    }
    while (!STACK.empty()) {
        char V = STACK.top().first, b = STACK.top().second;
        STACK.pop();
        pair<char, char> tmp = pair<char, char>(V, b);
        for (int i = 0; i < grammarList.size(); ++i) {
            char U = grammarList[i][0];
            if (U != V && grammarList[i][3] == V) { //形如U->Vb...
                INSERT(U, b);
            }
        }
    }
    for (int i = 0; i < Vn.size(); ++i) {
        vector<char> tmp;
        firstVT.push_back(tmp);
    }
    for (int i = 0; i < Vn.size(); ++i) {
        for (int j = 0; j < Vt.size(); ++j) {
            if (F[i][j] == 1)
                firstVT[i].push_back(Vt[j]);
        }
    }
    return;
}
void Operatorpriority_analyzer::get_lastVt() { //计算lastVT集
    for (int i = 0; i < Vn.size(); ++i) { //初始化
        for (int j = 0; j < Vt.size(); ++j) {
            F[i][j] = 0;
        }
    }
    for (int i = 0; i < grammarList.size(); ++i) { //赋初值
        char U = grammarList[i][0];
        for (int j = grammarList[i].size() - 1; j > 2; --j) {
            char b = grammarList[i][j];
            if (search_List(b, Vt)) { //是非终结符
                INSERT(U, b); //形如U->...b或U->...bV
                break;
            }
        }
    }
    while (!STACK.empty()) {
        char V = STACK.top().first, b = STACK.top().second;
        STACK.pop();
        pair<char, char> tmp = pair<char, char>(V, b);
        for (int i = 0; i < grammarList.size(); ++i) {
            char U = grammarList[i][0];
            int tmp = grammarList[i].size() - 1;
            if (U != V && grammarList[i][tmp] == V) { //形如U->...bV
                INSERT(U, b);
            }
        }
    }
    for (int i = 0; i < Vn.size(); ++i) {
        vector<char> tmp;
        lastVT.push_back(tmp);
    }
    for (int i = 0; i < Vn.size(); ++i) {
        for (int j = 0; j < Vt.size(); ++j) {
            if (F[i][j] == 1)
                lastVT[i].push_back(Vt[j]);
        }
    }
    return;
}
void Operatorpriority_analyzer::get_table() { //计算算符优先顺序表
    for (int i = 0; i < Vt.size() + 1; ++i) { //table初始化
        for (int j = 0; j < Vt.size() + 1; ++j) {
            table[i][j] = '/';
        }
    }
    for (int i = 0; i < grammarList.size(); ++i) {
        for (int j = 3; j < grammarList[i].size() - 1; ++j) {
            char x0 = grammarList[i][j], x1 = grammarList[i][j + 1];
            if (search_List(x0, Vt) && search_List(x1, Vt)) {
                int x = get_VtNum(x0), y = get_VtNum(x1);
                table[x][y] = '=';
            }
            if (j < grammarList.size() - 2) {
                char x2 = grammarList[i][j + 2];
                if (search_List(x0, Vt) && search_List(x2, Vt) && search_List(x1,Vn)) {
                    int x = get_VtNum(x0), y = get_VtNum(x2);
                    table[x][y] = '=';
                }
            }
            if (search_List(x0, Vt) && search_List(x1, Vn)) {
                int tmp = get_VnNum(x1);
                for (int k = 0; k < firstVT[tmp].size(); ++k) {
                    char b = firstVT[tmp][k];
                    int x = get_VtNum(x0), y = get_VtNum(b);
                    table[x][y] = '<';
                }
            }
            if (search_List(x0, Vn) && search_List(x1, Vt)) {
                int tmp = get_VnNum(x0);
                for (int k = 0; k < lastVT[tmp].size(); ++k) {
                    char a = lastVT[tmp][k];
                    int x = get_VtNum(x1), y = get_VtNum(a);
                    table[y][x] = '>';
                }
            }
        }
    }
    for (int i = 0; i < firstVT.size(); ++i) { //处理'#'优先级
        for (int j = 0; j < firstVT[i].size(); ++j) {
            int y = get_VtNum(firstVT[i][j]);
            table[Vt.size()][y] = '<';
        }
    }
    for (int i = 0; i < lastVT.size(); ++i) {
        for (int j = 0; j < lastVT[i].size(); ++j) {
            int x = get_VtNum(lastVT[i][j]);
            table[x][Vt.size()] = '>';
        }
    }
    return;
}
void Operatorpriority_analyzer::print_firstlastVt(){
    ofstream out("firstlastVt.txt");
    out << "                    ";
    for (int i = 0; i < Vn.size(); ++i) {
        out << Vn[i] << "          ";
    }
    out<<endl;
    out<<"firstVT:";
    out<<"   ";
    for (int i = 0; i < firstVT.size() + 1; ++i) { //输出table
        for(auto j: firstVT[i]){
            out<<j<<",";
        }
        out<<"     ";
    }
    out<<endl;
    out<<"lastVT:";
    out<<"    ";
    for (int i = 0; i < lastVT.size() + 1; ++i) { //输出table
        for(auto j: lastVT[i]){
            out<<j<<",";
        }
        out<<"     ";
    }
    out.close();
}
void Operatorpriority_analyzer::Readwenfa()
{
    int size = 1000;
    int fd,i;
    wf = (char *)malloc(size);
    if(!wf)
    {
        cout<<"[error] could not malloc"<<endl;
        exit(-1);
    }
    if((fd = open("/home/zhuowenjie/CLionProjects/untitled1/wenfa2.txt", 0)) < 0)
    {
        cout<<"[error] could not open Grammer file"<<endl;
        exit(-1);
    }
    if((i = read(fd, wf, size-1)) <= 0)
    {
        cout<<"[error] return "<<i<<endl;
        exit(-1);
    }
}
void Operatorpriority_analyzer::Putinwf()
{
    while(*wf!='\0'){
        string tmp;
        while(*wf!='\n'){
            tmp += *wf;
            wf++;
        }
        if(check_grammar(tmp)){
            grammarList.push_back(tmp); //记录该文法
            if (!search_List(tmp[0], Vn)) //如果没出现过，记录该终结符
                Vn.push_back(tmp[0]);
        }
        putinVt();
        wf++;
    }
}
void Operatorpriority_analyzer::GEQ(string x){
    string b = SEM.top();SEM.pop();
    string a = SEM.top();SEM.pop();
    string c = "t";
    c += (i++ + '0');
    vector<string> temp;
    temp.push_back(x);
    temp.push_back(a);
    temp.push_back(b);
    temp.push_back(c);
    QT.push_back(temp);
    SEM.push(c);
}
void Operatorpriority_analyzer::printQT(){
    for(auto i:QT){
        cout<<"( ";
        for(auto j:i){
            cout<<j<<" ";
        }
        cout<<")"<<endl;
    }
}
void Operatorpriority_analyzer::Operator_analysisRun() {
    p = 0;
    ReadFile();
    cout<<"文件中的内容："<<endl;
    cout<<prog<<endl;
    p = 0;
    ch = prog[p++];
    scaner();
    Readwenfa();
    Putinwf();
    get_firstVt(); //求firstVT集
    get_lastVt(); //求lastVT集
    get_table(); //求算符优先顺序表
    print_firstlastVt();
    printV();
    symbolStack.push_back('#');
    string tem;
    cout <<"Operator priority analyzer:\n";
    //printLine(51);
    /*cout << '|' << setw(5) << left << "step" << '|' << setw(10)<< left <<"analysis stack" << '|' << setw(8) << left
        << "relation" << "|" << setw(12) << "operation" << '|' << setw(10) << "Remain" << "|\n"; */
    char left = '#', right = '\0', priority = '/'; //符号栈栈顶符号,读入符号,顺序
    int x, y,step = 0; //算符优先顺序表横纵坐标
    bool flag; //规约是否成功
    bool flag1 = 1;
    while (!inputString.empty() || symbolStack.size()>2) {
        for (int i = symbolStack.size() - 1; i >= 0; --i) {
            if (!search_List(symbolStack[i], Vn)) {
                left = symbolStack[i];
                break;
            }
        }
        if (right == '\0') {
            right = inputString.front();
            inputString.erase(inputString.begin());
            if(right == 'i'){
                tem = token;
            }
            if(right!='#'){
                scaner();
            }
        }
        x = get_VtNum(left);
        y = get_VtNum(right);
        priority = table[x][y];
        //printLine(51);
        if (priority == '/') { //归约失败
            flag1 = 0;
            //printTable(symbolStack, inputString, step, left, priority, right, "ERROR");
            //printLine(51);
            break;
        }
        else if (priority == '>') {
            flag = false;
            string handle;
            for (int i = symbolStack.size() - 1; i > 0; --i){ //寻找最左素短语
                handle = blank;
                for (int j = i; j < symbolStack.size(); ++j) { //句柄
                    handle += symbolStack[j];
                }
                int res = search_Grammar(handle);
                if (res != -1) {
                    flag = true;
                    if(left == '+'||left=='-'||left=='*'||left=='/'){
                        string ss;
                        ss+=left;
                        GEQ(ss);
                    }
                    //printTable(symbolStack, inputString, step, left, priority, right, ("Reduction" + handle));
                    for (int j = 0; j < handle.size(); ++j) { //句柄
                        symbolStack.erase(symbolStack.begin() + i);
                    }
                    symbolStack.push_back((char)res); //归约成功
                    break;
                }
            }
            if (flag) { //归约成功
                //printTable(symbolStack, inputString, step, left, priority, right, ("归约"+handle));
            }
            else { //归约失败
                flag1 = 0;
                //printTable(symbolStack, inputString, step, left, priority, right, "ERROR");
                //printLine(51);
                break;
            }
        }
        else { //移进
            //printTable(symbolStack, inputString, step, left, priority, right, "PUSHIN"+(blank+right));
            if(right == 'i'){
                SEM.push(tem);
                tem.clear();
            }
            symbolStack.push_back(right);
            right = '\0';
        }
        step++;
    }
    string tt;
    tt += symbolStack[0];
    tt += symbolStack[1];
    /*分析成功*/
    if(flag1){
        left = priority = right = ' ';
        cout<<"RIGHT!"<<endl;
        //printLine(51);
        //printTable(symbolStack, inputString, step, left, priority, right, "RIGHT!");
        //printLine(51);
        //return;
    }
    else{
        cout<<"ERROR!"<<endl;
    }
    printQT();
}