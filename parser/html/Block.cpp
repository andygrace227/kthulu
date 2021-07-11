#pragma once

#include<iostream>

class Block{
    public:
        char beginningChar;
        char endChar;
        std::string content;

        Block(std::string content){
            this->content = content;
            this->beginningChar = content.at(0);
            this->endChar = content.at(content.length() - 1);
        }

};