#include <cstdlib>
#include <iostream>
#include <fstream>
#include <vector>
#include <map>
#include <string>
using namespace std;

class SyntaxAnalyzer{
    private:
        vector<string> lexemes;
        vector<string> tokens;
        vector<string>::iterator lexitr;
        vector<string>::iterator tokitr;

        // other private methods
        bool vdec();
        int vars();
        bool stmtlist();
        int stmt();
        bool ifstmt();
        bool elsepart();
        bool whilestmt();
        bool assignstmt();
        bool inputstmt();
        bool outputstmt();
        bool expr();
        bool simpleexpr();
        bool term();
        bool logicop();
        bool arithop();
        bool relop();
        std::istream& getline_safe(std::istream& input, std::string& output);
    public:
        SyntaxAnalyzer(istream& infile);
        // pre: 1st parameter consists of an open file containing a source code's
        //	valid scanner output.  This data must be in the form
        //			token : lexeme
        // post: the vectors have been populated

        bool parse();
        // pre: none
        // post: The lexemes/tokens have been parsed.
        // If an error occurs, a message prints indicating the token/lexeme pair
        // that caused the error.  If no error, data is loaded in vectors
};
SyntaxAnalyzer::SyntaxAnalyzer(istream& infile){
    string line, tok, lex;
    int pos;
    getline_safe(infile, line);
    bool valid = true;
    while(!infile.eof() && (valid)){
        pos = line.find(":");
        tok = line.substr(0, pos);
        lex = line.substr(pos+1, line.length());
        cout << pos << " " << tok << " " << lex << endl;
        tokens.push_back(tok);
        lexemes.push_back(lex);
        getline_safe(infile, line);
    }
    tokitr = tokens.begin();
    lexitr = lexemes.begin();
}

bool SyntaxAnalyzer::parse(){
    if (vdec()){
        if (tokitr!=tokens.end() && *tokitr=="t_main"){
            tokitr++; lexitr++;
            if (stmtlist()){                                   //Thomas Neumann inefficent code
            	if (tokitr!=tokens.end()) // should be at end token
                	if (*tokitr == "t_end"){
                		tokitr++; lexitr++;
                		if (tokitr==tokens.end()){  // end was last thing in file
                			cout << "Valid source code file" << endl;
                			return true;
                		}else{
                			cout << "end came too early" << endl;
                		}
                	}else{
                		cout << "invalid statement ending code" << endl;
                }else{
                	cout << "no end" << endl;
                }
            }else{
            	cout << "bad/no stmtlist" << endl;
            }
        }else{
        	cout << "no main" << endl;
        }
    }else{
    	cout << "bad var list" << endl;
    }
    return false;

}

bool SyntaxAnalyzer::vdec(){
    if (tokitr != tokens.end() && *tokitr != "t_var"){   //Thomas Neumann Check if at end
        return true;
    }else{
        tokitr++; lexitr++;
        int result = 0;   // 0 - valid, 1 - done, 2 - error
        result = vars();
        if (result == 2){
            return false;
        }
        while (result == 0){
            if (tokitr!=tokens.end()){
                result = vars(); // parse vars
            }
        }

        if (result == 1){
            return true;
        }else{
            return false;
        }
    }
}

int SyntaxAnalyzer::vars(){
    int result = 0;  // 0 - valid, 1 - done, 2 - error
    string temp;
    if (tokitr != tokens.end() && *tokitr == "t_integer"){  //Thomas Neumann Check if at end
        temp = "t_integer";
        tokitr++; lexitr++;
    }else if (tokitr != tokens.end() && *tokitr == "t_string"){  //thomas Neumann Check if at end
        temp = "t_string";
        tokitr++; lexitr++;
    }
    else
        return 1;
    bool semihit = false;
    while (tokitr != tokens.end() && result == 0 && !semihit){
        if (*tokitr == "t_id"){
            tokitr++; lexitr++;
            if (tokitr != tokens.end() && *tokitr == "s_comma"){
                tokitr++; lexitr++;
            }
            else if (tokitr != tokens.end() && *tokitr == "s_semi"){
                semihit = true;
                tokitr++; lexitr++;
            }
            else
                result = 2;
        }
        else{
            result = 2;
        }
    }
    return result;
}

bool SyntaxAnalyzer::stmtlist(){
    int result = stmt();

    while (result == 1){
    	result = stmt();
    }
    if (result == 0){
        return false;
    }else{
        return true;
    }
}
int SyntaxAnalyzer::stmt(){  // returns 1 or 2 if valid, 0 if invalid
	if (tokitr != tokens.end() && *tokitr == "t_if"){   //Thomas Neumann check if at end
        tokitr++; lexitr++;
        if (ifstmt()) return 1;
        else return 0;
    }
    else if (tokitr != tokens.end() && *tokitr == "t_while"){  //Thomas Neumann check if at end
        tokitr++; lexitr++;
        if (whilestmt()) return 1;
        else return 0;
    }
    else if (tokitr != tokens.end() && *tokitr == "t_id"){  // assignment starts with identifier   //thomas Neumann check if at end
        tokitr++; lexitr++;
        cout << "t_id" << endl;
        if (assignstmt()) return 1;
        else return 0;
    }
    else if (tokitr != tokens.end() && *tokitr == "t_input"){  // thomas neumann check if at end
        tokitr++; lexitr++;
        if (inputstmt()) return 1;
        else return 0;
    }
    else if (tokitr != tokens.end() && *tokitr == "t_output"){ // thomas neumann check if at end
        tokitr++; lexitr++;
        cout << "t_output" << endl;
        if (outputstmt()) return 1;
        else return 0;
    }
    return 2;  //stmtlist can be null
}

//Thomas Neumann Changed this function
//pre:  the tokiter will be at the beginning of the if statement
//post: the tokiter will be at the end of the if statement
//      the method also returns of the if statement is valid or not
//desc: this method runs through the tokens through out an if statement
//      and determines if the grammar is valid
//      the method will also output an error to the console if the grammar is invalid
//
//      IFSTMT ->  if  (EXPR)  then  STMTLIST  ELSEPART  end if
bool SyntaxAnalyzer::ifstmt(){
    //token t_if was checked in the method that calls this one

    if(tokitr != tokens.end() && *tokitr == "s_lparen"){
        tokitr++; lexitr++;

        if(expr()){

            if(tokitr != tokens.end() && *tokitr == "s_rparen"){
                tokitr++; lexitr++;

                if(tokitr != tokens.end() && *tokitr == "t_then"){
                    tokitr++; lexitr++;

                    if(stmtlist()){

                        if(elsepart()){

                            if(tokitr != tokens.end() && *tokitr == "t_end"){
                                tokitr++; lexitr++;

                                if(tokitr != tokens.end() && *tokitr == "t_if"){
                                    tokitr++; lexitr++;
                                    return true;
                                }
                            }
                        }
                    }
                }
            }
        }
    }

    cout << "Error: Bad If Statement" << endl;
    return false;
}

bool SyntaxAnalyzer::elsepart(){
    if (tokitr != tokens.end() && *tokitr == "t_else"){ //thomas neumann check if at end
        tokitr++; lexitr++;
        if (stmtlist()){
            return true;
        }else{
            return false;
        }
    }
    return true;   // elsepart can be null
}


//thomas neumann I wrote this method
//pre:  The tokitr will be at the beginning of the while statement
//post: the tokitr will be at the end of the while loop
//      it will also return if the while statement is valid or not
//desc: this method will loop through the tokens until it reaches the end
//      of the while statement or it will leave early if the grammar is invalid
//      the method will also output an error to the console if the grammar is invalid
//
//      WHILESTMT ->  while  (EXPR)  loop  STMTLIST  end loop
bool SyntaxAnalyzer::whilestmt(){
    //token t_while was checked in the method that calls this one

    if(tokitr != tokens.end() && *tokitr == "s_lparen"){
        tokitr++; lexitr++;

        if(expr()){

            if(tokitr != tokens.end() && *tokitr == "s_rparen"){
                tokitr++; lexitr++;

                if(tokitr != tokens.end() && *tokitr == "t_loop"){
                    tokitr++; lexitr++;

                    if(stmtlist()){

                        if(tokitr != tokens.end() && *tokitr == "t_end"){
                            tokitr++; lexitr++;

                            if(tokitr != tokens.end() && *tokitr == "t_loop"){
                                tokitr++; lexitr++;
                                return true;
                            }
                        }
                    }
                }
            }
        }
    }
    cout << "Error: Bad While Statement" << endl;
	return false;
}

//thomas neumann i wrote this method
//pre:  tokiter will be at the beginning of the assignment statement
//post: tokiter will bea the end of the assignment statement
//      the method will also return if the grammar is valid
//desc: the method will cycle through the tokens until the end of the assignment statement
//      it will also determine if there are any errors in the assignment statement
//      the method will also output an error to the console if the grammar is invalid
//
//      ASSIGNSTMT  ->  id = EXPR ;
bool SyntaxAnalyzer::assignstmt(){
    // token t_id is checked before the method before this

    if(tokitr != tokens.end() && *tokitr == "s_assign"){
        tokitr++; lexitr++;

        if(expr()){

            if(tokitr != tokens.end() && *tokitr == "s_semi"){
                tokitr++; lexitr++;
                return true;
            }
        }
    }
    cout << "Error: Bad Assignment Statement" << endl;
	return false;
}
bool SyntaxAnalyzer::inputstmt(){
    if (tokitr != tokens.end() && *tokitr == "s_lparen"){ //thomas neumann check if at end
        tokitr++; lexitr++;
        if (tokitr != tokens.end() && *tokitr == "t_id"){ //thomas neumann check if at end
            tokitr++; lexitr++;
            if (tokitr != tokens.end() && *tokitr == "s_rparen"){ //thomas neumann check if at end
                tokitr++; lexitr++;
                return true;
            }
        }
    }
    return false;
}

//thomas neumann i wrote this method
//pre:  the tokiter will be at the beginning of the output statement
//post: the tokiter will be at the end of the output statement
//      the method will also return if the output statement's grammar is valid or not
//desc: the method will go through each token in the output statement
//      it will then determine if the output statement is valid grammar wise
//      the method will also output an error to the console if the grammar is invalid
//
//      OUTPUTSTMT  ->  output (EXPR) | output (string)
bool SyntaxAnalyzer::outputstmt(){
    //token t_output was checked in the method that calls this one

    if(tokitr != tokens.end() && *tokitr == "s_lparen"){
        tokitr++; lexitr++;

        if(expr()){

            if(tokitr != tokens.end() && *tokitr == "s_rparen"){
                tokitr++; lexitr++;
                return true;
            }
        }else if(tokitr != tokens.end() && *tokitr == "t_string"){
            tokitr++; lexitr++;

            if(tokitr != tokens.end() && *tokitr == "s_rparen"){
                tokitr++; lexitr++;
                return true;
            }
        }
    }
    cout << "Error: Bad Output Statement" << endl;
	return false;
}

bool SyntaxAnalyzer::expr(){
    if (simpleexpr()){
        if (logicop()){
            if (simpleexpr()){
                return true;
            }                         //thomas Neumann redundant code
        }else{
            return true;
        }                        //thomas Neumann redundant code
    }
    return false;
}

//thomas neumann i wrote this method
//pre:  the tokiter will be at the beginning of the simple expression
//post: the tokiter will be at the end of the of the simple expression
//      the method will also return if the grammar is valid for the simple expression
//desc: the method will loop through each token and determine if the simple expression is valid
//      the method will also output an error to the console if the grammar is invalid
//
//      [] -> there can be 0 or 1 of these
//      SIMPLEEXPR -> TERM  [ARITHOP TERM | RELOP TERM]
bool SyntaxAnalyzer::simpleexpr(){
    if(term()){
        if(arithop()){
            if(term()){
                return true;
            }
        }else if(relop()){
            if(term()){
                return true;
            }
        }else{
            return true;
        }
    }
    cout << "Error: Bad Simple Expression" << endl;
	return false;
}

bool SyntaxAnalyzer::term(){
    if(tokitr != tokens.end()){  //thomas neumann check if at end
        if ((*tokitr == "t_int") || (*tokitr == "t_str") || (*tokitr == "t_id")){
            tokitr++; lexitr++;
            return true;
        }else{
            if (tokitr != tokens.end() && *tokitr == "s_lparen"){ // thomas neumann check if at end
                tokitr++; lexitr++;
                if (expr()){
                    if (tokitr != tokens.end() && *tokitr == "s_rparen"){ //thomas neumann check if at end
                        tokitr++; lexitr++;
                        return true;
                    }
                }
            }            //thomas neumann redundant code 
        }
    }
    return false;
}

bool SyntaxAnalyzer::logicop(){
    if(tokitr != tokens.end()){ //thomas neumann check if at end
        if ((*tokitr == "s_and") || (*tokitr == "s_or")){
            tokitr++; lexitr++;
            return true;
        }
    }                      //Thomas neumann redundant code
    return false;
}

bool SyntaxAnalyzer::arithop(){
    if(tokitr != tokens.end()){ //Thomas neumann check if at end
        if ((*tokitr == "s_mult") || (*tokitr == "s_plus") || (*tokitr == "s_minus") || (*tokitr == "s_div")	|| (*tokitr == "s_mod")){
            tokitr++; lexitr++;
            return true;
        }
    }                               //thomas neumann redundant code
    return false;
}

bool SyntaxAnalyzer::relop(){
    if(tokitr != tokens.end()){  //thomas neumann check if at end
        if ((*tokitr == "s_lt") || (*tokitr == "s_gt") || (*tokitr == "s_ge") || (*tokitr == "s_eq") || (*tokitr == "s_ne") || (*tokitr == "s_le")){
            tokitr++; lexitr++;
            return true;
        }
    }                     //thomas neumann redundant code
    return false;
}
std::istream& SyntaxAnalyzer::getline_safe(std::istream& input, std::string& output)
{
    char c;
    output.clear();

    input.get(c);
    while (input && c != '\n')
    {
        if (c != '\r' || input.peek() != '\n')
        {
            output += c;
        }
        input.get(c);
    }

    return input;
}

int main(){


    //! Should Change this so the user has to enter the file name
    ifstream infile("codelexemes.txt");
    if (!infile){
    	cout << "error opening codelexemes.txt file" << endl;
        exit(-1);
    }
    SyntaxAnalyzer sa(infile);
    sa.parse();
    return 1;
}
