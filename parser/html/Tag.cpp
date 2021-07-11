#pragma once
#include<iostream>
#include<map>
#include<vector>
#include <boost/algorithm/string.hpp>

class Tag{
    
    public:
        std::string tagName;
        std::string originalTagName;
        std::string attributeString;
        std::string content;
        std::map<std::string, std::string> attributes;
        std::vector<Tag> children;
        Tag * parent;

        Tag(){

        }
        Tag(std::string tagName){
            this->tagName = boost::algorithm::to_lower_copy(tagName);
            this->originalTagName = tagName;
            this->attributeString = "";
            this->content = "";
            this->parent = nullptr;
        }


        Tag & operator=(const Tag & t2) {
            this->tagName = t2.tagName;
            this->originalTagName = t2.originalTagName;
            this->attributeString = t2.attributeString;
            this->attributes = t2.attributes;
            this->content = t2.content;
            this->children = t2.children;
            this->parent = t2.parent;
            return *this;
        }


        Tag(const Tag & t2){
            this->tagName = t2.tagName;
            this->originalTagName = t2.originalTagName;
            this->attributeString = t2.attributeString;
            this->attributes = t2.attributes;
            this->content = t2.content;
            this->children = t2.children;
            this->parent = t2.parent;
        }

        void appendChild(Tag t){
            children.push_back(t);
        }

        void setParent(Tag *t){
            this->parent = t;
        }

        void setAttribute(std::string key, std::string value){
            this->attributes[key] =  value;
        }
        
        bool hasAttribute(std::string key){
            std::map<std::string, std::string>::iterator it = attributes.find(key);
            if (it != attributes.end()) return true;
            return false;
        }

        std::string getAttribute(std::string key){
            if(!hasAttribute(key)) return "undefined";
            return attributes.at(key);
        }  

        void parseAttributes(){
            int i = 0;
            for(; i < attributeString.length(); i++){
                if(attributeString.at(i) != ' '){
                    //Begin building attribute name
                    std::string attributeName = "";
                    std::string attributeValue = "";
                    while(i < attributeString.length() && attributeString.at(i) != ' ' && attributeString.at(i) != '=' ){
                        attributeName += attributeString.at(i);
                        i++;
                    }

                    while(i < attributeString.length() && attributeString.at(i) == ' '){
                        i++;
                    }
                    if(i < attributeString.length() && attributeString.at(i) == '='){
                        //begin building attribute value.
                        i++;
                        while(i < attributeString.length() && attributeString.at(i) == ' '){
                            i++;
                        }

                        char quoteChar = ' ';
                        if(i < attributeString.length() && (attributeString.at(i) == '"' || attributeString.at(i) == '\'')){
                            //Handle a quoted attribute
                            quoteChar = attributeString.at(i);
                            i++;
                        }
                            
                        while(i < attributeString.length() && attributeString.at(i) != quoteChar){
                            attributeValue += attributeString.at(i);
                            i++;
                        }

                    }
                    else{
                        //Set up for loop for next attribute.
                        i--;
                    }

                    attributes[attributeName] = attributeValue;
                }
            }
        }


        Tag mergeTextTags(){
            std::string newTextContent = "";
            std::vector<Tag> newChildren;
            for(int i = 0; i < children.size(); i++){
                if(children[i].tagName == "kthulu-text"){
                    newTextContent = "";
                    for(; i < children.size() && children[i].tagName == "kthulu-text"; i++){
                        newTextContent += children[i].content + " ";
                    }
                    Tag newTag("kthulu-text");
                    //Trim whitespace
                    newTextContent.erase(std::remove(newTextContent.begin(), newTextContent.end(), '\t'), newTextContent.end());
                    newTextContent.erase(std::remove(newTextContent.begin(), newTextContent.end(), '\n'), newTextContent.end());
                    newTextContent = trimWhiteSpace(newTextContent);
                    newTag.content = newTextContent;
                    
                    if(newTag.content.find_first_not_of(' ') != std::string::npos ){
                        newChildren.push_back(newTag);
                    }
                    i--;
                }
                else{
                    newChildren.push_back(children[i].mergeTextTags());
                }
            }
            children = newChildren;
            return *this;
        }

        std::string prettyPrint(int indent){
            std::string indentStr = "";
            for(int i = 0; i < indent; i++){
                indentStr += "   ";
            }
            std::string output;
            output += indentStr + "Tag: " + tagName + "\n";
            
            if(attributes.size() != 0){
                output += indentStr + "attributes: " + "\n";
                for(auto const &x : attributes){
                    output += indentStr + "\t" + x.first + " : " + x.second + "\n";
                }

            }
            
            /*
            output += indentStr + "attributeStr: " + attributeString + "\n";
            */
            if(children.size() != 0) output + indentStr + "children: " + "\n";
            for(int i = 0 ; i < children.size(); i++){
                output += children[i].prettyPrint(indent + 1);
            }

            if(content != "") output += indentStr + "content: "  + content + "\n";
            return output + "\n";
        }


        std::string trimWhiteSpace(std::string input){
            std::string output;  
            unique_copy (input.begin(), input.end(), std::back_insert_iterator<std::string>(output),
                                     [](char a,char b){ return std::isspace(a) && std::isspace(b);});  
            return output;
        }
        
    
};