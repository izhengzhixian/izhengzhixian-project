#include <iostream>
#include <vector>
#include <map>
#include <stack>
#include <iomanip>
using namespace std;


/* 使用说明：先用类实例化一个对象
 * 然后使用load_lang来加载文法，所有包含或的语句需要分开
 * 如：B -> B+T|B-T分成B -> B+T 和 B -> B-T
 * 然后使用load_lang('B', "B+T")和load_lang('B', "B-T")加载
 * 注意，第一个是字符，第二个是字符串
 * 加载完成之后需要有一个特殊的文法表达式，假如当前入口是B
 * 则需要找一个没有用到的大写字母，比如Z，构造Z -> #B#
 * 然后使用load_lang加载
 * 接着使用set_start来设置入口，这里使用Z，即set_start('Z');
 * 然后执行over来关闭输入，并计算算符优先关系表
 * 最后用cal_formula来判断输入串，注意，这里不需要加结束标志#
 * 可用print_table来打印算符优先关系表
 * 还可用clear来释放资源，然后可以加载新的文法
 */


class Opg
{
    // 所有有返回值的方法，返回非0值表示出错
private:
    // 算符优先关系表的状态
    enum Status{None, Less, Same, Greater};
    // 存放上面状态对应的符号
    char oparray[4] = {' ', '<', '=', '>'};
    // 文法，每个非终结符可能有多个产生式，所以用vector来存储
    map<char, vector<string> > lang;
    // 终结符对应的索引，与算符优先关系表中的索引一致
    map<char, int> terminal;
    map<char, int> noterminal;
    // 算符优先关系表，大于，等于，小于，没有
    Status **table = nullptr;
    // 算符优先关系表的行数
    size_t table_size = 0;
    // firstvt矩阵，第一维是非终结符
    bool **firstvt = nullptr;
    size_t firstvt_size = 0;
    bool **lastvt = nullptr;
    size_t lastvt_size = 0;
    // 文法开始非终结符
    char start = 0;
    // 如果为true，则关闭load_lang，要重新加载，必须先clear
    bool close = false;
    int calc_firstvt();
    int calc_lastvt();
    // 求相等关系，直接在table中赋值
    int calc_same();
    // 扫描终结符存起来，与一个唯一的索引映射
    int scan_terminal();
    int calc_talbe();
public:
    // 释放所有资源，并初始化
    void clear();
    // 结束输入，开始计算
    int over();
    // 计算等式的规约过程
    int cal_formula(string str);
    // 加载文法
    int load_lang(char nt, string str);
    // 设置文法开始值
    int set_start(char c);
    // 打印算符优先关系表
    int print_table();
    Opg();
    virtual ~Opg();
};

Opg::Opg(){}
Opg::~Opg(){

    for(size_t i = 0; i < table_size; i++)
        delete []table[i];
    delete []table;
    table = nullptr;
    table_size = 0;

    for(size_t i = 0; i < firstvt_size; i++)
        delete []firstvt[i];
    delete []firstvt;
    firstvt = nullptr;
    firstvt_size = 0;

    for(size_t i = 0; i < lastvt_size; i++)
        delete []lastvt[i];
    delete []lastvt;
    lastvt = nullptr;
    lastvt_size = 0;
}
void Opg::clear(){
    start = 0;
    close = 0;
    lang.clear();
    terminal.clear();
    noterminal.clear();
    this->~Opg();
}
int Opg::over(){
    // 如果出错，返回1
    if(scan_terminal())
        return 1;
    if(calc_talbe())
        return 1;
    return 0;
}

int Opg::load_lang(char nt, string str){
    if(close || nt <'A' || nt > 'Z' || str.empty()){
        // 报错
        return 1;
    }
    if(lang.count(nt) == 1){
        lang[nt].push_back(str);
    }else{
        size_t cur_size = noterminal.size();
        noterminal[nt] = (int)cur_size;
        vector<string> v({str});
        lang[nt] = v;
    }
    return 0;
}
int Opg::scan_terminal(){
    terminal.clear();
    for (map<char, vector<string> >::iterator i = lang.begin(); i != lang.end(); ++i) {
        for (vector<string>::iterator j = i->second.begin();
             j != i->second.end(); ++j) {
            for(size_t k = 0; k < j->size(); k++){
                char c = (*j)[k];
                if(c <'A' || c > 'Z'){
                    if(terminal.count(c) == 0) {
                        size_t cur_size = terminal.size();
                        terminal[c] = (int)cur_size;
                    }
                }
            }
        }
    }
    return 0;
}
int Opg::calc_firstvt(){
    // 报错
    if(firstvt_size != 0)
        return 1;
    size_t tsize = terminal.size();
    size_t ntsize = noterminal.size();
    firstvt = new bool*[ntsize];
    for (size_t i = 0; i < ntsize; ++i) {
        firstvt[i] = new bool[tsize];
        for(size_t j = 0; j < tsize; j++){
            firstvt[i][j] = false;
        }
    }
    firstvt_size = ntsize;
    stack<pair<char, char> > firstvt_stack;
    // 用来存储已经入过栈的序列，如果一个序列入过栈，则不再压栈
    // 第二个参数是字符串，是所有入过栈的字符的集合
    map<char, string> save_stack;
    // 扫描所有式子，压栈
    for (map<char, vector<string> >::iterator i = lang.begin(); i != lang.end(); ++i) {
        for (vector<string>::iterator j = i->second.begin();
             j != i->second.end(); ++j) {
            for(size_t k = 0; k < j->size(); k++){
                char c = (*j)[k];
                if(c <'A' || c > 'Z'){
                    // 这里是如果当前序列没有如果栈
                    if(save_stack.count(i->first) == 0){
                        firstvt_stack.push(make_pair(i->first, c));
                        save_stack[i->first] = "";
                        save_stack[i->first].append(1, c);
                    }else if(save_stack[i->first].find(c) == string::npos ){
                        firstvt_stack.push(make_pair(i->first, c));
                        save_stack[i->first].append(1, c);
                    }
                    break;
                }
            }
        }
    }

    // 判断当前栈顶的式子
    while(!firstvt_stack.empty()){
        pair<char, char> top = firstvt_stack.top();
        firstvt[noterminal[top.first]][terminal[top.second]] = true;
        firstvt_stack.pop();
        for (map<char, vector<string> >::iterator i = lang.begin(); i != lang.end(); ++i) {
            for (vector<string>::iterator j = i->second.begin();
                 j != i->second.end(); ++j) {
                if( (*j)[0] == top.first && top.first != i->first){
                    if(save_stack.count(i->first) == 0){
                        firstvt_stack.push(make_pair(i->first, top.second));
                        save_stack[i->first] = "";
                        save_stack[i->first].append(1, top.second);
                    }else if(save_stack[i->first].find(top.second) == string::npos ){
                        firstvt_stack.push(make_pair(i->first, top.second));
                        save_stack[i->first].append(1, top.second);
                    }
                }
            }
        }
    }

    return 0;
}
int Opg::calc_same(){
    if(table_size == 0)
        return 1;
    for (map<char, vector<string> >::iterator i = lang.begin(); i != lang.end(); ++i) {
        for (vector<string>::iterator j = i->second.begin();
             j != i->second.end(); ++j) {
            char pre = 0;
            char next = 0;
            for(size_t k = 0; k < j->size(); k++){
                char c = (*j)[k];
                if(c <'A' || c > 'Z'){
                    next = c;
                    if(pre != 0){
                        table[terminal[pre]][terminal[next]] = Same;
                    }
                    pre = next;
                }
            }
        }
    }
    return 0;
}
int Opg::calc_lastvt(){
    // 报错
    if(lastvt_size != 0)
        return 1;
    size_t tsize = terminal.size();
    size_t ntsize = noterminal.size();
    lastvt = new bool*[ntsize];
    for (size_t i = 0; i < ntsize; ++i) {
        lastvt[i] = new bool[tsize];
        for(size_t j = 0; j < tsize; j++){
            lastvt[i][j] = false;
        }
    }
    lastvt_size = ntsize;

    stack<pair<char, char> > lastvt_stack;
    map<char, string> save_stack;
    for (map<char, vector<string> >::iterator i = lang.begin(); i != lang.end(); ++i) {
        for (vector<string>::iterator j = i->second.begin();
             j != i->second.end(); ++j) {
            for(size_t k = j->size(); k > 0; k--){
                char c = (*j)[k-1];
                if(c <'A' || c > 'Z'){
                    if(save_stack.count(i->first) == 0){
                        lastvt_stack.push(make_pair(i->first, c));
                        save_stack[i->first] = "";
                        save_stack[i->first].append(1, c);
                    }else if(save_stack[i->first].find(c) == string::npos ){
                        lastvt_stack.push(make_pair(i->first, c));
                        save_stack[i->first].append(1, c);
                    }
                    break;
                }
            }
        }
    }

    while(!lastvt_stack.empty()){
        pair<char, char> top = lastvt_stack.top();
        lastvt[noterminal[top.first]][terminal[top.second]] = true;
        lastvt_stack.pop();
        for (map<char, vector<string> >::iterator i = lang.begin(); i != lang.end(); ++i) {
            for (vector<string>::iterator j = i->second.begin();
                 j != i->second.end(); ++j) {
                if( (*j)[j->size()-1] == top.first && top.first != i->first){
                    if(save_stack.count(i->first) == 0){
                        lastvt_stack.push(make_pair(i->first, top.second));
                        save_stack[i->first] = "";
                        save_stack[i->first].append(1, top.second);
                    }else if(save_stack[i->first].find(top.second) == string::npos ){
                        lastvt_stack.push(make_pair(i->first, top.second));
                        save_stack[i->first].append(1, top.second);
                    }
                }
            }
        }
    }

    return 0;
}
int Opg::set_start(char c){
    // 报错
    if(lang.count(c) == 0)
        return 1;
    start = c;
    return 0;
}
int Opg::calc_talbe(){
    // 报错
    if(start == 0 || table_size != 0)
        return 1;
    close = true;
    size_t tsize = terminal.size();
    table = new Status*[tsize];
    for (size_t i = 0; i < tsize; ++i) {
        table[i] = new Status[tsize];
        for(size_t j = 0; j < tsize; j++){
            table[i][j] = None;
        }
    }
    table_size = tsize;
    calc_firstvt();
    calc_lastvt();
    calc_same();
    char pre = 0;
    char next = 0;
    for (map<char, vector<string> >::iterator i = lang.begin(); i != lang.end(); ++i) {
        for (vector<string>::iterator j = i->second.begin();
             j != i->second.end(); ++j) {
            pre = 0;
            next = 0;
            for(size_t k = 0; k < j->size(); k++){
                next = (*j)[k];
                if(pre == 0){
                    pre = next;
                    continue;
                }
                if(next <'A' || next > 'Z'){
                    for (size_t x = 0; x < terminal.size(); ++x) {
                        if(lastvt[noterminal[pre]][x]){
                            Status relation = table[x][terminal[next]];
                            if(relation != None && relation != Greater){
                                cout << "不是一个算符优先文法" << "\n";
                                exit(1);
                            }else{
                                table[x][terminal[next]] = Greater;
                            }
                        }
                    }
                }else{
                    for (size_t x = 0; x < terminal.size(); ++x) {
                        if(firstvt[noterminal[next]][x]){
                            Status relation = table[terminal[pre]][x];
                            if(relation != None && relation != Less){
                                cout << "不是一个算符优先文法" << "\n";
                                exit(1);
                            }else{
                                table[terminal[pre]][x] = Less;
                            }
                        }
                    }
                }
                pre = next;
            }
        }
    }
    return 0;
}

int Opg::print_table(){
    if(table_size == 0)
        return 1;
    //每个索引对应的终结符
    char * tarray = new char[terminal.size()];
    for (map<char, int>::iterator i = terminal.begin(); i != terminal.end(); ++i) {
        tarray[i->second] = i->first;
    }
    cout << "  ";
    for (size_t i = 0; i < terminal.size(); ++i) {
        cout << tarray[i] << " ";
    }
    cout << endl;

    for (size_t i = 0; i < table_size; ++i) {
        cout << tarray[i] << " ";
        for (size_t j = 0; j < terminal.size(); ++j) {
            cout << oparray[table[i][j]] << " ";
        }
        cout << endl;
    }
    delete [] tarray;
    return 0;
}

int Opg::cal_formula(string str){
    if(!close)
        return 1;
    vector<char> readed;
    readed.push_back(lang[start][0][0]);
    str.append(1, lang[start][0][lang[start][0].size()-1]);
    Status relation;
    size_t distance_setw = str.size() + 4;
    for (size_t i = 0; i < str.size(); ++i) {
        string readed_str(readed.begin(), readed.end());
        char now = str[i];
        char readed_top = readed.back();
        if(readed_top >= 'A' && readed_top <= 'Z'){
            readed.pop_back();
            relation = table[terminal[readed.back()]][terminal[now]];
            readed.push_back(readed_top);
        }else{
            relation = table[terminal[readed.back()]][terminal[now]];
        }
        // 表示刚从栈里弹出的一个非终结符
        char first_char = 0;
        // 表示在栈顶将要被弹出的非终结符
        char second_char = 0;
        switch(relation){
        case Greater:
            cout << left << setw(distance_setw) << readed_str;
            cout << left << setw(6) << now;
            cout << "归约" << endl;
            while(true){
                readed_top = readed.back();
                if(readed_top >= 'A' && readed_top <= 'Z'){
                    readed.pop_back();
                }else{
                    second_char = readed_top;
                    if(first_char != 0){
                        if(table[terminal[second_char]][terminal[first_char]] == Less){
                            readed.push_back('N');
                            break;
                        }
                    }
                    first_char = second_char;
                    readed.pop_back();
                }
            }
            // 下次依然读取当前的now字符
            --i;
            break;
        case Same:
            if(now == lang[start][0][lang[start][0].size()-1]){
                cout << left << setw(distance_setw) << readed_str;
                cout << left << setw(6) << now;
                cout <<  "接受" << endl;
                return 0;
            }
        case Less:
            cout << left << setw(distance_setw) << readed_str;
            cout << left << setw(6) << now;
            cout <<  "压入" << endl;
            readed.push_back(now);
            break;
        case None:
            cout << "出现算符优先关系表不存在的关系" << "\n";
            return 1;
        default:
            cout << "算符优先关系表构建错误" << "\n";
            exit(1);
        }

    }

    return 0;
}

int main(int argc, char *argv[])
{
    Opg opg;
    // 这里是书上的一个例子，用来测试
    /*
    cout << "课本上的例子，用来测试" << "\n";
    opg.load_lang('Z', "#E#");
    opg.load_lang('E', "E+T");
    opg.load_lang('E', "T");
    opg.load_lang('T', "T*F");
    opg.load_lang('T', "F");
    opg.load_lang('F', "P^F");
    opg.load_lang('F', "P");
    opg.load_lang('P', "(E)");
    opg.load_lang('P', "i");
    opg.set_start('Z');
    opg.over();
    opg.print_table();
    opg.cal_formula("i*i+i");

    // 清空数据
    opg.clear();

    cout << endl << endl;
    */

    //以下是本次实验的数据
    cout << "本次实验，输出结果" << "\n";
    opg.load_lang('B', "BoT");
    opg.load_lang('B', "T");
    opg.load_lang('T', "TaF");
    opg.load_lang('T', "F");
    opg.load_lang('F', "nF");
    opg.load_lang('F', "(B)");
    opg.load_lang('F', "t");
    opg.load_lang('F', "f");
    opg.load_lang('Z', "#B#");
    opg.set_start('Z');
    opg.over();
    opg.print_table();
    opg.cal_formula("ntofat");

    return 0;
}
