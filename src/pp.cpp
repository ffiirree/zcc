#include "pp.h"
#include "error.h"
/*
# 6.10 Preprocessing directives  // 预处理
preprocessing_file = [group]

group = group_part {group_part}

group_part = if_section | control_line | text_line | '#' non_directive

if_section = if_group [elif_groups] [else_group] endif_line

if_group = '#' 'if' const_expr new_line groupopt
         | '#' 'ifdef' ID new_line groupopt
         | '#' 'ifndef' ID new_line groupopt

elif_groups = elif_group | elif_groups elif_group

elif_group = '#' 'elif' const_expr new_line groupopt

else_group = '#' 'else' new_line [group]

endif_line = '#' 'endif' new_line

control_line = '#' 'include' pp_tokens new_line
             | '#' 'define' ID replacement_list new_line
             | '#' 'define' ID lparen [ID_list] ) replacement_list new_line
             | '#' 'define' ID lparen ... ) replacement_list new_line
             | '#' 'define' ID lparen ID_list , ... ) replacement_list new_line
             | '#' 'undef' ID new_line
             | '#' 'line' pp_tokens new_line
             //| '#' 'error' [pp_tokens] new_line
             //| '#' 'pragma' [pp_tokens] new_line
             //| '#' new_line

text_line = [pp_tokens] new_line

non_directive = pp_tokens new_line

lparen = a ( character not immediately preceded by white_space

replacement_list = [pp_tokens]

pp_tokens = preprocessing_token {preprocessing_token}

new_line = the new_line character

preprocessing_token = header_name 
                    | ID 
                    | pp_number 
                    | character_constant 
                    | string_literal 
                    | punctuator
*/

void Preprocessor::group()
{
	for (;;) {
		Token t = lex.next();
		if (t.getType() == TNEWLINE)
			continue;
		if (t.getId() == '#')
			group_part();
	}
}

void Preprocessor::group_part()
{
	Token t = lex.next();

	if (t.getType() == TNEWLINE)
		return;
	if (t.getType() == INTEGER || t.getType() == FLOAT) {
		// linemarker(tok);
		return;
	}
		
	if (t.getType() == ID) {
		if (t.getSval() == "if") return;
		if (t.getSval() == "ifdef") return;
		if (t.getSval() == "ifndef") return;
		if (t.getSval() == "elif") return;
		if (t.getSval() == "else")return;
		if (t.getSval() == "endif") return;
		if (t.getSval() == "include") read_header_name();
		if (t.getSval() == "define") return;
		if (t.getSval() == "undef") return;
		if (t.getSval() == "line") return;
	}
}
inline bool Preprocessor::next_is(int id)
{
	if (lex.next().getId() == id)
		return true;
	lex.back();
	return false;
}

inline void Preprocessor::expect(int id)
{
	Token t = lex.next();
	if (t.getId() != id)
		error("expect '%c', but not is '%c'", id, t.getId());
}


void Preprocessor::read_header_name()
{
	std::string header_name;
	std::string end;

	if (next_is('\"'))
		end = '\"';
	else if (next_is('<'))
		end = '>';
	else
		error("Not include file.");

	if (lex.peek().getType() == ID) {
		header_name = lex.next().getSval();
	}
	expect('.');
	if (lex.peek().getSval() != "h")
		error("Not '.h' file.");
	lex.next();
	if (end == "\"")
		expect('\"');
	else
		expect('>');

	printf("");

	// 读取头文件到
}