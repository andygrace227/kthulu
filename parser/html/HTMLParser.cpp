#pragma once
#include<vector>
#include<iostream>
#include<algorithm>
#include"Tag.cpp"


class HTMLParser{

    public:
        std::vector<Block> blocks;
        std::string HTML;
        std::vector<int> mergedBlocks;
        Tag root;

        HTMLParser(std::string h){
            this->HTML = h;
            root = Tag("root");
        }

        void formBlocks(){
            int i = 0;
            char delimiters[5]= {' ', '<', '>', '"', '\''};
            std::string builder = "";
            for(; i < HTML.length(); i++){
                char c = HTML.at(i);

                    builder += c;

                if(std::find(std::begin(delimiters), std::end(delimiters), c) != std::end(delimiters) || i == HTML.length() - 1){
                    Block b(builder);
                    blocks.push_back(b);
                    builder = "";
                    builder += c;
                }
            }
            //std::cout << "formed blocks" << std::endl;

        }

        void mergeBlocksInTags(){
            
            std::vector<Block> newBlocks;
            int i = 0;
            for(; i < blocks.size(); i++){
                ////std::cout << "processing " << blocks[i].content << std::endl;
                if(blocks[i].beginningChar == '<'){

                    if(blocks[i].content.substr(0,4) == "<!--" ){
                        //In a comment. Go until we find a block with --> at the end
                        std::string newContent = blocks[i].content.substr(0, blocks[i].content.length() - 1);
                        
                        int j = i + 1;
                        bool endFound = false;
                        for(; j < blocks.size() && endFound == false; j++){
                            ////std::cout << blocks[j].content << std::endl;
                            newContent += blocks[j].content.substr(0, blocks[j].content.length() - 1 );

                            if(newContent.substr(newContent.length() - 4, 3 ) == "-->"){
                               
                                Block b(newContent);
                                newBlocks.push_back(b);
                                i = j - 1;
                                mergedBlocks.push_back(newBlocks.size() - 1);
                                endFound = true;
                            }
                        }
                    }                    

                    else{
                        //In a tag. Go until we find a >
                        std::string newContent = blocks[i].content.substr(0, blocks[i].content.length() - 1);
                        int j = i + 1;
                        char inQuoteChar = ' ';
                        bool endFound = false;
                        for(; j < blocks.size() && endFound == false ; j++){
                            //handle attributes with quotes which may contain HTML tags.
                            if(blocks[j].beginningChar == '"' || blocks[j].beginningChar == '\''){
                                if(inQuoteChar == ' ') inQuoteChar = blocks[j].beginningChar;
                                else inQuoteChar = ' ';
                            }

                            if( (inQuoteChar == ' ' && blocks[j].beginningChar == '>') || j == blocks.size() - 1 ){
                                //End the tag, push a new block on.
                                Block b(newContent + ">");
                                newBlocks.push_back(b);
                                i = j - 1;
                                mergedBlocks.push_back(newBlocks.size() - 1);
                                endFound = true;
                            }
                            else{
                                newContent += blocks[j].content.substr(0, blocks[j].content.length() - 1);
                            }
                        }
                    }
                }
                else{
                    newBlocks.push_back(blocks[i]);
                }
            }
            //std::cout << "merged blocks" << std::endl;
            for(int i = 0; i < newBlocks.size(); i++){
                //std::cout << newBlocks[i].content << std::endl;
            }
            //std::cout << "end of blocks" << std::endl;
            blocks = newBlocks;
        }

        void createTags(){
            //We should have some blocks that are merged together now.
            //We'll go through each one and form a tag from it

            //Some tags don't have children
            std::vector<std::string> noChildTags;
            noChildTags.push_back("!--");
            noChildTags.push_back("area");
            noChildTags.push_back("base");
            noChildTags.push_back("br");
            noChildTags.push_back("col");
            noChildTags.push_back("command");
            noChildTags.push_back("embed");
            noChildTags.push_back("hr");
            noChildTags.push_back("img");
            noChildTags.push_back("input");
            noChildTags.push_back("keygen");
            noChildTags.push_back("link");
            noChildTags.push_back("meta");
            noChildTags.push_back("param");
            noChildTags.push_back("source");
            noChildTags.push_back("track");
            noChildTags.push_back("wbr");

            //Some tags have children, but may have closing tags.
            //We will treat the next occurence of these tags as the closing tag.
            //We also know to search through the element stack to find the appropriate 
            //next parent if the element if the element is in that vector

            std::vector<std::string> optionalClosedTags;
            optionalClosedTags.push_back("html");
            optionalClosedTags.push_back("head");
            optionalClosedTags.push_back("body");
            optionalClosedTags.push_back("p");
            optionalClosedTags.push_back("dt");
            optionalClosedTags.push_back("dd");
            optionalClosedTags.push_back("li");
            optionalClosedTags.push_back("option");
            optionalClosedTags.push_back("thead");
            optionalClosedTags.push_back("th");
            optionalClosedTags.push_back("tbody");
            optionalClosedTags.push_back("tr");
            optionalClosedTags.push_back("td");
            optionalClosedTags.push_back("tfoot");
            optionalClosedTags.push_back("colgroup");

            //We use a "stack" of tag pointers to keep track of the parent element
            //The parent element is used to find the end tag.

            std::vector< Tag * > elementStack;
            elementStack.push_back(&this->root);

            
            for(int i = 0; i < blocks.size(); i++){
                //If we've processed this tag before, we know it's probably an element tag
                if(std::find(std::begin(mergedBlocks), std::end(mergedBlocks), i) != std::end(mergedBlocks)){
                    //Do some analysis on this tag.
                    std::string tagName;
                    int j = 1;
                    for(; j < blocks[i].content.length() && blocks[i].content.at(j) != ' ' && blocks[i].content.at(j) != '>'; j++){
                        tagName += blocks[i].content.at(j);
                    }
                    //std::cout << "processing a " << tagName << "tag" << std::endl;

                    //Great! We have a tag name. Let's see if it's an opening tag or a closing tag.
                    if(tagName.substr(0,1) == "/"){
                        //Closing tag. See if our tag name matches the parent.
                        //If it does, perfect.
                        tagName = tagName.substr(1,tagName.length() - 1);
                        tagName = boost::algorithm::to_lower_copy(tagName);
                        if(elementStack.back()->tagName == tagName){
                            elementStack.pop_back();
                        }
                        //Otherwise, look through the next elements in the array, and make that the new parent.
                        else if(std::find(std::begin(optionalClosedTags), std::end(optionalClosedTags), tagName) != std::end(optionalClosedTags)){
                            int numberToPop = 0;
                            int i = elementStack.size();
                            for(; i > -1 && elementStack[i]->tagName != tagName; i++){
                                numberToPop++;
                            }
                            if(i != -1){
                                //We found an element and it makes sense to pop it off.
                                for(i = 0; i < numberToPop; i++){
                                    elementStack.pop_back();
                                }
                            }
                        }
                        //If that doesn't work, then it's probably user error.
                        //Ignore it and don't change anything.
                    }
                    else{
                        //Opening tag. Make the tag and populate the attributes.

                        Tag t(tagName);
                        if(blocks[i].content.at(j) != '>'){
                            t.attributeString = blocks[i].content.substr(tagName.length() + 1, blocks[i].content.length() - tagName.length() - 2);
                            t.parseAttributes();
                        }
                        //See if this tag is script or css, which shouldn't be converted into text nodes.
                        if(t.tagName == "script" || t.tagName == "style"){
                            //Populate the content of this tag.
                            std::string endTag = "</" + t.tagName;
                            std::string content = "";

                            int k = i + 1;
                            //std::cout << blocks[k].content;
                            for(; k < blocks.size() && blocks[k].content.find(endTag) == std::string::npos; k++){
                                if( k == i + 1){
                                    std::string str;
                                    if(blocks[k].content.substr(0,1) == ">")
                                        str = blocks[k].content.substr(1, blocks[k].content.length() - 1);
                                    if(str.substr(str.length() - 1,1) == "<")
                                        str = str.substr(0, str.length() - 1);

                                }
                                else{
                                    content += blocks[k].content.substr(0, blocks[k].content.length() - 1);
                                } 
                            }
                            i = k+1;
                            t.content = content;
                            elementStack.back()->appendChild(t);

                        }
                        //See if the tag is a comment element
                        else if(t.tagName.substr(0,3) == "!--"){
                            t.tagName = "!";
                            t.content = t.attributeString;
                            t.attributes.clear();
                            t.attributeString = "";
                            elementStack.back()->appendChild(t);
                        }

                        //It could also be a DOCTYPE element
                        else if(t.tagName.substr(0,8) == "!doctype"){
                            t.tagName = "!doctype";
                            elementStack.back()->appendChild(t);
                        }

                        //It's a normal HTML element
                        //Decide if this tag could have children
                        else if(std::find(std::begin(noChildTags), std::end(noChildTags), t.tagName) != std::end(noChildTags)){
                            //Tag does not have children, don't modify the element stack
                            elementStack.back()->appendChild(t);
                        }
                        else{
                            //Decide if this is a child or a sibling of the parent.
                            if(std::find(std::begin(optionalClosedTags), std::end(optionalClosedTags), t.tagName) != std::end(optionalClosedTags) 
                            && t.tagName ==  elementStack.back()->tagName){
                                //Tag is likely a sibling.
                                //This is a proof of concept so we're going to assume everything else is all good,
                                //and only support one level of siblingness.
                                elementStack.pop_back();
                            }
                            //Tag is definitely a child now
                            elementStack.back()->appendChild(t);
                            elementStack.push_back(&(elementStack.back()->children[ elementStack.back()->children.size() - 1 ]));
                        }
                    }
                }
                else{
                    //Assume that this is a text element
                    Tag t("kthulu-text");
                    t.content = blocks[i].content.substr(1, blocks[i].content.length()-2);
                    elementStack.back()->appendChild(t);
                }
            }
            //std::cout << "created tags" << std::endl;
        }


        void mergeTextTags(){
            root = root.mergeTextTags();
        }




    
};