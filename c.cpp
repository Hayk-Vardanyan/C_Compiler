#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>
#include <string>
#include <map>
#include <utility>


class C{
    public:
    C();
    ~C();
    private:
    void start();
    bool check();
    void add_dis();
    void add_virtuals();
    void bind_virtuals();
    void delete_empty();
    void delete_prefix();
    void find_objects(int i);
    bool is_ref(std::string);
    bool check_member(int i);
    bool derivedacces(int i,int j);
    bool func_access(int i, int j); 
    bool func_access_not_derived(int i,int j);
    bool is_derived(std::string);
    void inherit(int i,int j);
    void read_text(std::string path);
    void generate();
    void sort_members(int i, int j);
    std::vector<std::string> make_vector(int i, int j);
    std::string check_add(int i,int j);
    std::string find_type(std::string str);
    std::string ref_to(std::string str);
    inline void error() { std::cout<<"ERROR" ; exit(0) ; }

    private:
    std::vector<std::vector<std::string>> text;
    std::map<std::string,std::vector<std::string>> mp;
    std::map<std::string,std::string> objects;
    std::map<std::pair<std::string,std::string>,std::string > vtable ;
    std::ifstream fin;
    std::ofstream fout;
    char flag = 'p';
    int countscop{};    

};



C::C()
{
start();
}

C::~C()
{
fout.close();
fin.close();
}

void C::start(){
    std::string path;
    std::cout<<"Enter File path -> ";
    std::cin>>path;
    read_text(path);
    if(check()){
        generate();
    }
}

void C::read_text(std::string path){
    fin.open(path);
    if(!fin.is_open()){
        std::cout<<"not";
    }
    while(!fin.eof()){
        std::string str;
        getline(fin,str);
        std::stringstream s(str);
        std::vector<std::string> tmp;
        while(!s.eof()){
            if(str == " "){
                continue;
            }
            s>>str;
            tmp.push_back(str);   
        }
        text.push_back(tmp);
    }
    
}

bool C::check(){
    int count{};
    add_virtuals();
    for(int i {};i<text.size();++i){
        for(int j{}; j<text[i].size();++j){
            if(text[i][j] == "class" ){
               countscop++;
               mp.emplace(text[i][j+1],make_vector(i,j));
            }
            if(text[i][j] == "class" && text[i][j+2] == ":"){
                inherit(i,j);
                if(derivedacces(i,j) == false){
                    error();
                }
            }
            if(text[i][j] == "::"){
                for (auto itr = mp.begin(); itr != mp.end(); ++itr) {
                    if(text[i][j] == "::" && text[i][j-1] == itr->first && text[i][j+2] == "("){
                        if(is_derived(itr->first)){
                            if(func_access(i,j) == false){
                                error();
                            }
                        }
                        if(!(is_derived(itr->first))){
                            if(func_access_not_derived(i,j)){
                                error();
                            }
                        }                   
                    }
                }
            }
            if(text[i][j] == "main"){
                find_objects(i);
                if(!(check_member(i))){
                    error();
                }
            }
        }
    }
    bind_virtuals();
    return true;
}

std::vector<std::string> C::make_vector(int i, int j){
    std::vector<std::string> tmpvec;
    std::string tmp = text[i][j+1] ;
     //p: private //  r: protected  // b: public
    for(int i2 = i; i2 < text.size(); ++i2){
        for(int j2 = j; j2 < text[i2].size(); ++j2){
            if(text[i2][j2] == "{" && i2 != i ){
                countscop++;
            }
            else if(text[i2][j2] == "}"){
                countscop--;
            }
            if(countscop == 0 ||( text[i2][j2] == "class" && i2 != i)){
                return tmpvec;
            }

            if(text[i2][j2] == "private:"){
                text[i2].erase(text[i2].begin()+j2);
                flag = 'p';
            }
            else if (text[i2][j2] == "protected:"){
                text[i2].erase(text[i2].begin()+j2);
                flag = 'r';
            }
            else if(text[i2][j2] == "public:"){ 
                text[i2].erase(text[i2].begin()+j2);
                flag = 'b';
            }

            if( (text[i2][j2] == "int" && (text[i2][j2+2] == ";" || text[i2][j2+2] == "="))
                ||  (text[i2][j2] == "float" && (text[i2][j2+2] == ";" || text[i2][j2+2] == "="))
                ||  (text[i2][j2] == "double" && (text[i2][j2+2] == ";" || text[i2][j2+2] == "="))
                ||  (text[i2][j2] == "short" && (text[i2][j2+2] == ";" || text[i2][j2+2] == "="))
                ||  (text[i2][j2] == "long" && (text[i2][j2+2] == ";" || text[i2][j2+2] == "=")))
            {
                check_add(i2,j2+1);
                tmpvec.push_back(text[i2][j2+1]);
            } 
            else if((text[i2][j2] == "const" && (text[i2][j2+3] == ";" || text[i2][j2+3] == "="))
                ||  (text[i2][j2] == "firend" && (text[i2][j2+3] == ";" || text[i2][j2+3] == "="))
                ||  (text[i2][j2] == "static" && (text[i2][j2+3] == ";" || text[i2][j2+3] == "="))
                )
            {
                check_add(i2,j2+2);
                tmpvec.push_back(text[i2][j2+2]);
               j2+=2;
            }
            else if( (text[i2][j2] == "int" && text[i2][j2+2] == "(" )
                ||  (text[i2][j2] == "float" && text[i2][j2+2] == "("  )
                ||  (text[i2][j2] == "double" && text[i2][j2+2] == "(" )
                ||  (text[i2][j2] == "short" && text[i2][j2+2] == "(" )
                ||  (text[i2][j2] == "long" && text[i2][j2+2] == "(" )
                ||  (text[i2][j2] == "void" && text[i2][j2+2] == "(" ))
            {
                check_add(i2,j2+1);
                tmpvec.push_back(text[i2][j2+1]);
            }
            else if((text[i2][j2] == "const" && text[i2][j2+3] == "(" )
                ||  (text[i2][j2] == "firend" && text[i2][j2+3] == "(" )
                ||  (text[i2][j2] == "static" && text[i2][j2+3] == "(" ))
            {
                check_add(i2,j2+2);
                tmpvec.push_back(text[i2][j2+2]);
                j2+=2;
            }
            else if (text[i2][j2] == "virtual" && text[i2][j2+3] == "(" ) {
                check_add(i2,j2+2);
                text[i2][j2+2] = "vt" + text[i2][j2+2] + tmp;
                tmpvec.push_back(text[i2][j2+2]);
                j2+=2;
            }

            
            else {
                //error();
            }
        }
    }

  return tmpvec;
}

void C::generate()
{
    std::string tmpnm;
    fout.open("C++_To_C.cpp");
    if(fout.is_open()){
        for(int i {};i<text.size();++i){
            for(int j{}; j<text[i].size();++j){
                if(text[i][j] == "class"){
                    tmpnm = text[i][j+1];
                    if(text[i][j+2] == ":"){
                        text[i].erase(text[i].begin()+j+2,text[i].begin()+text[i].size()-1);
                    }
                    sort_members(i,j);
                    text[i][j] = "struct";
                }
                if( (text[i][j] == "int" && text[i][j+2] == "(" && text[i][j+1] != "main")
                ||  (text[i][j] == "float" && text[i][j+2] == "("  )
                ||  (text[i][j] == "double" && text[i][j+2] == "(" )
                ||  (text[i][j] == "short" && text[i][j+2] == "(" )
                ||  (text[i][j] == "long" && text[i][j+2] == "(" )
                ||  (text[i][j] == "void" && text[i][j+2] == "(" ))
                 {
                    if(text[i][text[i].size()-1] == ";"){
                        text.erase(text.begin()+i,text.begin()+i+1);
                    }
                    else if(text[i][text[i].size()-1] != ";"){
                        if(text[i][j+3] == ")"){
                        if(text[i][text[i].size()-2] == "const"){
                            text[i].erase(text[i].begin() + text[i].size()-2);
                            text[i].insert(text[i].begin() + j+3,"const " +tmpnm + "* const dis ");
                        }
                        else
                        text[i].insert(text[i].begin()+ j+3,tmpnm + "* const dis ");
                        }
                        else if(text[i][j+3] != ")"){
                        if(text[i][text[i].size()-2] == "const"){
                            text[i].erase(text[i].begin() + text[i].size()-2);
                            text[i].insert(text[i].begin() + j+3,"const " +tmpnm + "* const dis ");
                        }
                        text[i].insert(text[i].begin()+ j+3,tmpnm + "* const dis ,");
                        }
                    }    
                }
                 else if((text[i][j] == "const" && text[i][j+3] == "(" )
                ||  (text[i][j] == "firend" && text[i][j+3] == "(" )
                ||  (text[i][j] == "static" && text[i][j+3] == "(" )
                ||  (text[i][j] == "virtual" && text[i][j+3] == "(" ))
                {
                    if(text[i][text[i].size()-1] == ";"){
                      text.erase(text.begin()+i,text.begin()+i+1);
                    } 
                    else if(text[i][text[i].size()-1] != ";" && text[i][j] != "static" && 
                    text[i][j] != "firend" ){
                        if(text[i][j+4] == ")"){
                        if(text[i][text[i].size()-2] == "const"){
                            text[i].erase(text[i].begin() + text[i].size()-2);
                            text[i].insert(text[i].begin() + j+4,"const " + tmpnm + "* const dis ");
                        }
                        else
                            text[i].insert(text[i].begin()+ j+4,tmpnm + "* const dis ");
                        }
                        else if(text[i][j+3] != ")"){
                        if(text[i][text[i].size()-2] == "const"){
                            text[i].erase(text[i].begin() + text[i].size()-2);
                            text[i].insert(text[i].begin() + j+4,"const " + tmpnm + "* const dis ");
                        }
                        else
                            text[i].insert(text[i].begin()+ j+4,tmpnm + "* const dis ,");
                        }
                    }
                    i++;

                }
                if(text[i][j]=="::"){
                    if(text[i][j+3] == ")"){
                        if(text[i][text[i].size()-2] == "const"){
                            text[i].erase(text[i].begin() + text[i].size()-2);
                            text[i].insert(text[i].begin() + j+3,"const " + text[i][j-1] + "* const dis ");
                        }
                        else
                        text[i].insert(text[i].begin() + j+3,text[i][j-1] + "* const dis ");
                    }
                    else if(text[i][j+3] != ")"){
                        if(text[i][text[i].size()-2] == "const"){
                            text[i].erase(text[i].begin() + text[i].size()-2);
                            text[i].insert(text[i].begin() + j+3,"const " + text[i][j-1] + "* const dis ");
                        }
                        else
                        text[i].insert(text[i].begin() + j+3,text[i][j-1] + "* const dis ,");
                    }
                    text[i].erase(text[i].begin()+j-1,text[i].begin()+j+1);
                }
                if (text[i][j] == "." && text[i][j+2] == "(" ){
                    if(text[i][j+3] == ")"){
                        text[i].insert(text[i].begin()+j+3,"&"+text[i][j-1]);
                        text[i].erase(text[i].begin()+j-1,text[i].begin()+j+1);
                    }
                    else if(text[i][j+3] != ")"){
                        text[i].insert(text[i].begin()+j+3,"&"+text[i][j-1]+",");
                        text[i].erase(text[i].begin()+j-1,text[i].begin()+j+1);
                    }
                }
                else if(text[i][j] == "->" && text[i][j+2] == "(" ){
                    if(text[i][j+3] == ")"){
                        text[i].insert(text[i].begin()+j+3,ref_to(text[i][j-1]));
                        text[i].erase(text[i].begin()+j-1,text[i].begin()+j+1);
                    }
                    else if(text[i][j+3] != ")"){
                        text[i].insert(text[i].begin()+j+3,ref_to(text[i][j-1])+",");
                        text[i].erase(text[i].begin()+j-1,text[i].begin()+j+1);
                    }
                }
            }
        }
        add_dis();
        delete_empty();
        delete_prefix();
        for(int i {};i<text.size();++i){
            for(int j{}; j<text[i].size();++j){
                fout<<text[i][j]<<" ";
            }
        fout<<std::endl;
        }
    }

    else{
        std::cout<<"not open";
    }

}

std::string C::check_add(int i,int j){
    if(flag == 'p'){
        text[i][j] = "pri" + text[i][j];
    }
    else if (flag == 'r'){
        text[i][j] = "pro" + text[i][j];
    }
    else if(flag == 'b'){
        text[i][j] = "pub" + text[i][j];
    }
    return text[i][j];
}

void C::inherit(int i,int j){
    if(text[i][j+3] == "private"){
        std::vector<std::string> tmpvec ;
        tmpvec.push_back(text[i][j+4]);
        tmpvec.push_back("priob");
        tmpvec.push_back(";");
        text.insert(text.begin()+i+1,tmpvec);
        for (auto itr = mp.begin(); itr != mp.end(); ++itr) {
            if(itr->first == text[i][j+1]){
                itr->second.push_back(tmpvec[0]+tmpvec[1]+tmpvec[2]);
            }
        }
    }
    else if(text[i][j+3] == "public"){
        std::vector<std::string> tmpvec ;
        tmpvec.push_back(text[i][j+4]);
        tmpvec.push_back("pubob");
        tmpvec.push_back(";");
        text.insert(text.begin()+i+1,tmpvec);
        for (auto itr = mp.begin(); itr != mp.end(); ++itr) {
            if(itr->first == text[i][j+1]){
                itr->second.push_back(tmpvec[0]+tmpvec[1]+tmpvec[2]);
            }
        }
    }
    else if(text[i][j+3] == "protected"){
        std::vector<std::string> tmpvec ;
        tmpvec.push_back(text[i][j+4]);
        tmpvec.push_back("proob");
        tmpvec.push_back(";");
        text.insert(text.begin()+i+1,tmpvec);
        for (auto itr = mp.begin(); itr != mp.end(); ++itr) {
            if(itr->first == text[i][j+1]){
                itr->second.push_back(tmpvec[0]+tmpvec[1]+tmpvec[2]);
            }
        }
    }
    else {
        std::vector<std::string> tmpvec ;
        tmpvec.push_back(text[i][j+3]);
        tmpvec.push_back("priob");
        tmpvec.push_back(";");
        text.insert(text.begin()+i+1,tmpvec);
        for (auto itr = mp.begin(); itr != mp.end(); ++itr) {
            if(itr->first == text[i][j+1]){
                itr->second.push_back(tmpvec[0]+tmpvec[1]+tmpvec[2]);
            }
        }
    }

}

bool C::derivedacces(int i,int j){
     std::string tmp;
     std::string tmpdr = text[i][j+1];
    if(text[i][j+3] == "public" || text[i][j+3] == "private" || text[i][j+3] == "protected" ){
        tmp = text[i][j+4];
    }
    else if(!(text[i][j+3] == "public" || text[i][j+3] == "private" || text[i][j+3] == "protected" )){
        tmp = text[i][j+3];
    }
    countscop++;
    i+=2;
    for(int i2 = i; i2<text.size();++i2){
        for(int j2 = j ; j2 < text[i2].size();++j2){
            for (auto itr = mp.begin(); itr != mp.end(); ++itr) {
                if(text[i2][j2] == "}"){
                    countscop--;
                }
                else if(text[i2][j2] == "{"){
                    countscop++;
                }
                if(countscop == 0 ){
                    return true;
                }
                for (auto itr = mp.begin(); itr != mp.end(); ++itr) {
                    if(itr->first == tmpdr){
                        for(int jj = 0; jj<itr->second.size();++jj){
                            if(("pri" + text[i2][j2] == itr->second[jj])
                            || ("pro" + text[i2][j2] == itr->second[jj])
                            || ("pub" + text[i2][j2] == itr->second[jj]) ){
                                return true;
                            }
                        }
                    }
                }
                if(tmp == itr->first ){
                    for(int itr2 = 0; itr2<itr->second.size();++itr2){
                        if(text[i2][j2] == itr->second[itr2].substr(3)){
                            if(itr->second[itr2].substr(0,3) == "pri"){
                                return false;
                            }
                        }
                    }
                }
            }
        }
    }
    return true;

}

bool C::is_derived(std::string str){
    for (auto itr = mp.begin(); itr != mp.end(); ++itr) {
        if(str == itr->first){
            for(int j = 0; j<itr->second.size();++j){
                if(itr->second[j].substr(4) == "ob;"){
                    return true;
                }
            }
        }
    }
    return false;
}
  
bool C::func_access(int i,int j){
    std::string tmp;
    std::string tmpdr = text[i][j-1];
    countscop++;
    i++;
    for (auto itr = mp.begin(); itr != mp.end(); ++itr) {
        if(itr->first == tmpdr){
            for(int jj = 0; jj<itr->second.size();++jj){
                if(itr->second[jj][itr->second[jj].size()-1] == ';'){
                    tmp = itr->second[jj][0];
                }
            }
        }
    }
    for(int i2 = i; i2<text.size();++i2){
        for(int j2 = j ; j2 < text[i2].size();++j2){
            for (auto itr = mp.begin(); itr != mp.end(); ++itr) {
                if(text[i2][j2] == "}"){
                    countscop--;
                }
                else if(text[i2][j2] == "{"){
                    countscop++;
                }
                if(countscop == 0 ){
                    return true;
                }

                for (auto itr = mp.begin(); itr != mp.end(); ++itr) {
                    if(itr->first == tmpdr){
                        for(int jj = 0; jj<itr->second.size();++jj){
                            if(("pri" + text[i2][j2] == itr->second[jj])
                            || ("pro" + text[i2][j2] == itr->second[jj])
                            || ("pub" + text[i2][j2] == itr->second[jj]) ){
                                return true;
                            }
                        }
                    }
                }

                if(tmp == itr->first ){
                    for(int itr2 = 0; itr2<itr->second.size();++itr2){
                        if(text[i2][j2] == itr->second[itr2].substr(3)){
                            if(itr->second[itr2].substr(0,3) == "pri"){
                                return false;
                            }
                        }
                    }
                }
            }
        }
    }
    return true;
}

bool C::func_access_not_derived(int i,int j){
    std::string tmpdr = text[i][j-1];
    countscop++;
    i++;
    j=0;
    for(int i2 = i; i2<text.size();++i2){
        for(int j2 = j ; j2 < text[i2].size();++j2){
            for (auto itr = mp.begin(); itr != mp.end(); ++itr) {
                if(text[i2][j2] == "}"){
                    countscop--;
                }
                else if(text[i2][j2] == "{"){
                    countscop++;
                }
                if(countscop == 0 ){
                    return false;
                }

                for (auto itr = mp.begin(); itr != mp.end(); ++itr) {
                    if(itr->first == tmpdr){
                        for(int jj = 0; jj<itr->second.size();++jj){
                            if(("pri" + text[i2][j2] == itr->second[jj])
                            || ("pro" + text[i2][j2] == itr->second[jj])
                            || ("pub" + text[i2][j2] == itr->second[jj]) ){
                                return false;
                            }
                        }
                    }
                }
            }
        }
    }
    return true;
}

void C::find_objects(int i){
    i++;
    countscop++;
    for(int i2 = i; i2<text.size();++i2){
        for(int j2 = 0 ; j2 < text[i2].size();++j2){
            if(text[i2][j2] == "{"){
                countscop++;
            }
            else if(text[i2][j2] == "}"){
                countscop--;
            }
            if(countscop == 0){
                return;
            }

            for (auto itr = mp.begin(); itr != mp.end(); ++itr) {
                if(text[i2][0] == itr->first){
                    objects.emplace(itr->first , text[i2][1] );
                }
                else if(text[i2][1] == itr->first){
                    objects.emplace(itr->first , text[i2][2] );
                }
            } 
        }
    }
}

bool C::check_member(int i){
    i++;
    countscop++;
    for(int i2 = i; i2<text.size();++i2){
        for(int j2 = 0 ; j2 < text[i2].size();++j2){
            if(text[i2][j2] == "{"){
                countscop++;
            }
            else if(text[i2][j2] == "}"){
                countscop--;
            }
            if(countscop == 0){
                return true;
            }
        
            if(text[i2][j2] == "."){
                for(auto it = objects.begin(); it != objects.end();++it){
                    if(text[i2][j2-1] == it->second){
                        for(auto it2 = mp.begin();it2 != mp.end();++it2){
                            if(it->first == it2->first){
                                for(int j=0; j<it2->second.size();++j){
                                    if(("pri"+text[i2][j2+1] == it2->second[j2])
                                    ||("pro"+text[i2][j2+1] == it2->second[j2])){
                                        return false;
                                    }
                                }
                            }
                        }
                    }
                }
            }
        }
    }
    return true;
}

void C::sort_members(int i, int j){
    countscop = 1;
    i++;
    bool flag = false;
    for(int i2 = i; i2 < text.size(); ++i2){
        for(int j2 = j; j2 < text[i2].size(); ++j2){
            if(text[i2][j2] == "{"){
                countscop++;
            }
            else if(text[i2][j2] == "}"){
                countscop--;
            }
            if(countscop == 0 ){
                flag =true;
                text.erase(text.begin()+i2,text.begin()+i2+1);
                break;
            }
            if( (text[i2][j2] == "int" && (text[i2][j2+2] == ";" || text[i2][j2+2] == "="))
                ||  (text[i2][j2] == "float" && (text[i2][j2+2] == ";" || text[i2][j2+2] == "="))
                ||  (text[i2][j2] == "double" && (text[i2][j2+2] == ";" || text[i2][j2+2] == "="))
                ||  (text[i2][j2] == "short" && (text[i2][j2+2] == ";" || text[i2][j2+2] == "="))
                ||  (text[i2][j2] == "long" && (text[i2][j2+2] == ";" || text[i2][j2+2] == "=")))
            {
               text.insert(text.begin()+i+1,text[i2]);
               text.erase(text.begin()+i2+1,text.begin()+i2+2);
            } 
            else if((text[i2][j2] == "const" && (text[i2][j2+3] == ";" || text[i2][j2+3] == "="))
                ||  (text[i2][j2] == "firend" && (text[i2][j2+3] == ";" || text[i2][j2+3] == "="))
                ||  (text[i2][j2] == "static" && (text[i2][j2+3] == ";" || text[i2][j2+3] == "="))
                )
            {
               text.insert(text.begin()+i+1,text[i2]);
               text.erase(text.begin()+i2+1 ,text.begin()+i2+2);
            }
        }
        if(flag){
            break;
        }
    }
int tmp;
flag = false;
    for(int i2 = i; i2 < text.size(); ++i2){
        for(int j2 = j; j2 < text[i2].size(); ++j2){
            if( (text[i2][j2] == "int" && text[i2][j2+2] == "(" )
                ||  (text[i2][j2] == "float" && text[i2][j2+2] == "("  )
                ||  (text[i2][j2] == "double" && text[i2][j2+2] == "(" )
                ||  (text[i2][j2] == "short" && text[i2][j2+2] == "(" )
                ||  (text[i2][j2] == "long" && text[i2][j2+2] == "(" )
                ||  (text[i2][j2] == "void" && text[i2][j2+2] == "(" ))
            {
                tmp = i2;
                flag = true;
                break;
            }
            else if((text[i2][j2] == "const" && text[i2][j2+3] == "(" )
                ||  (text[i2][j2] == "firend" && text[i2][j2+3] == "(" )
                ||  (text[i2][j2] == "static" && text[i2][j2+3] == "(" )
                ||  (text[i2][j2] == "virtual" && text[i2][j2+3] == "(" ))
            {
                tmp = i2;
                flag = true;
                break;
            }
        }
        if(flag){
            break;
        }
    }
    std::vector<std::string> tmpvec;
    tmpvec.push_back("}");
    tmpvec.push_back(";");
    text.insert(text.begin()+tmp-1,tmpvec);



}

void C::delete_empty(){
    for(int i {};i<text.size();++i){
        for(int j{}; j<text[i].size();++j){
            if( (text[i].size() == 1 && 
            (text[i][j][0] == '\0' ||
             text[i][j][0] == '\n' ||
             text[i][j][0] == ' ')||
             text[i].size() == 0)){
                 text.erase(text.begin()+i);
            }
        }
    }
}

void C::delete_prefix(){
    for(int i {};i<text.size();++i){
        for(int j{}; j<text[i].size();++j){
            if(text[i][j].substr(0,3) == "pri" || 
            text[i][j].substr(0,3) == "pro" || 
            text[i][j].substr(0,3) == "pub"){
                text[i][j].erase(0,3);
            }
            else if(text[i][j].substr(0,2) == "vt"){
                text[i][j].erase(0,5);
            }
            else if(text[i][j] == "virtual"){
                text[i].erase(text[i].begin()+j);
            }
        }
    }
}

void C::add_dis(){
    for(int i {};i<text.size();++i){
        for(int j{}; j<text[i].size();++j){
            for(auto it = mp.begin();it!=mp.end();++it){
                for(int jj = 0 ; jj < it->second.size();++jj){
                    if(text[i][j] == it->second[jj].substr(3,it->second[jj].size()) && 
                    text[i][j+1] != "(")
                    {
                        text[i][j] = "dis-> " + text[i][j];
                    }
                    if(text[i][j] == "main"){
                        return;
                    }
                }
            }
        }
    }
}

void C::add_virtuals(){
int tmpi;
    for(int i {};i<text.size();++i){
        for(int j{}; j<text[i].size();++j){
            if(text[i][j] == "main"){
                tmpi = i;
                break;
            }
        }
    }
    for(int i = tmpi;i<text.size();++i){
        for(int j = 0; j<text[i].size();++j){
            if(text[i][j][text[i][j].size()-1] == '*' || 
                text[i][j][text[i][j].size()-1] == '&'){
                    if(text[i][j][text[i][j].size()-1] == '&'){
                        text[i][j+3]="&"+text[i][j+3];
                    }
                    text[i][j][text[i][j].size()-1] = '*';
                    vtable.emplace(std::make_pair(text[i][j].substr(0,text[i][j].size()-1),text[i][j+1]),text[i][j+3]);
                    text[i].insert(text[i].begin()+j+3,"("+text[i][0]+")");

            }
        }
    }
}

void C::bind_virtuals(){
    int tmpi;
    std::string tmp;
    for(int i {};i<text.size();++i){
        for(int j{}; j<text[i].size();++j){
            if(text[i][j] == "main"){
                tmpi = i;
                break;
            }
        }
    }
    for(int i = tmpi;i<text.size();++i){
        for(int j = 0; j<text[i].size();++j){
            if(text[i][j] == "." && is_ref(text[i][j-1])){
                text[i][j] = "->"; 
            }
            if(text[i][j] == "->" && is_ref(text[i][j-1])){
                for(auto it : vtable){
                    if(text[i][j-1] == it.first.second){
                        tmp = find_type(it.second);
                        text[i][j+1] += tmp;
                    }
                }
            }

        }
    }
}

bool C::is_ref(std::string str){
    for(auto it : vtable){
        if(it.first.second == str){
            return true;
        }
    }
return false;
}

std::string C::find_type(std::string str){
    for(auto it : objects){
        if(it.second == str){
            return it.first;
        }
        else if(str[0] == '&'){
            if(it.second==str.substr(1,str.size())){
                return it.first;
            }
        }
    }
    return "";
}

std::string C::ref_to(std::string str){
    for(auto it : vtable){
        if(str == it.first.second){
            return it.second;
        }
    }
    return "";
}

int main()
{
	C obj;
}
