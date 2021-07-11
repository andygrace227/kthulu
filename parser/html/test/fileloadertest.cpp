#pragma ONCE
#include<iostream>
#include<fstream>
#include<streambuf>
#include"../Block.cpp"
#include"../HTMLParser.cpp"
#include"../Tag.cpp"

int main(int argc, char* argv[]){
    //load the test HTML into a string
    std::ifstream t("test.html");
    std::string str;

    t.seekg(0, std::ios::end);   
    str.reserve(t.tellg());
    t.seekg(0, std::ios::beg);

    str.assign((std::istreambuf_iterator<char>(t)),
            std::istreambuf_iterator<char>());


    //make ab parser and test it out.
    HTMLParser p(str);
    p.formBlocks();
    p.mergeBlocksInTags();
    p.createTags();
    p.mergeTextTags();
 
    std::cout << p.root.prettyPrint(0);

    
}