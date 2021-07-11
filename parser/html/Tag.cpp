#pragma ONCE
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

        Tag(tagName){
            this->tagName = boost::algorithm::to_lower_copy(tagName);
            this->originalTagName = tagName;
            this->attributeString = "";
            this->content = "";
            this->parent = nullptr;
        }


        Tag(const Tag $t2){
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
        
        void hasAttribute(std::string key){
            std::map::iterator it = attributes.find(key);
            if (it != attributes.end()) return true;
            return false;
        }

        void getAttribute(std::string key){
            if(!hasAttribute(key)) return "undefined";
            return attributes.at(key);
        }  

        void parseAttributes(){
            int i = 0;
            for(; i < attributeString.length; i++){
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
            std::vector newChildren;
            for(int i = 0; i < children.length(); i++){
                if(children[i].tagName == "kthulu-text"){
                    newTextContent = children[i].content + " ";
                    for(; i < children.length() && children[i].tagName == "kthulu-text"; i++){
                        newTextContent += children[i].content + " ";
                    }
                    Tag newTag("kthulu-text");
                    newTag.content = newTextContent;
                    newChildren.push_back(newTag);
                    i--;
                }
                else{
                    newChildren.push_back(children[i].mergeTextTags());
                }
            }
            children = newChildren;
            return *this;
        }
        
    
};