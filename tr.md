# 6.5 Expressions
## 6.5.01 Primary expressions // 主表达式
primary_expr = ID {push(D) | call(p,n)}
             | constant {push(constant)} 
             | string_literal {push(constant)}
             | '(' expression ')'

## 6.5.02 Postfix operators // 后缀操作符
postfix_expr = primary_expr postfix_expr_tail
             | '(' type ')' '{' init_list '}' postfix_expr_tail
             | '(' type ')' '{' init_list ',' '}' postfix_expr_tail

postfix_expr_tail = '[' expression ']' postfix_expr_tail
                  | '(' [argument_expr_list] ')' postfix_expr_tail
                  | ('.' | '->') ID postfix_expr_tail
                  | ('++' | '--') postfix_expr_tail
                  | empty

argument_expr_list = assignment_expr {',' assignment_expr}

## 6.5.03 Unary operators // 一元运算符
unary_expr = postfix_expr
           | ('++' | '--') unary_expr
           | unary_operator cast_expr
           | 'sizeof' unary_expr
           | 'sizeof' '(' type ')'
unary_operator = '&' | '*' | '+' | '-' | '˜' | '!'

## 6.5.04 Cast operators // 强制类型转换
cast_expr = unary_expr
          | '(' type ')' cast_expr

## 6.5.05 Multiplicative operators // 乘除模运算符
multi_expr = cast_expr {('*' | '/' | '%') {GEQ()} cast_expr}

## 6.5.06 Additive operators // 加减运算
add_expr = multi_expr {('+' | '-') {GEQ()} multi_expr}

## 6.5.07 Bitwise shift operators // 移位运算
shift_expr = add_expr {('<<' | '>>') add_expr}

## 6.5.08 Relational operators // 比较运算
relational_expr = shift_expr {relational_op {GEQ()} shift_expr}
relational_op = '<' | '>' | '<=' | '>='

## 6.5.09 Equality operators // 相等运算
equal_expr = relational_expr {('==' | '!=') {GEQ()} relational_expr}

## 6.5.10 Bitwise AND operator // 位&运算
bit_and_expr = equal_expr {'&'{GEQ()} equal_expr}

## 6.5.11 Bitwise exclusive OR operator
bit_xor_expr = bit_and_expr {'ˆ' {GEQ()} bit_and_expr}

## 6.5.12 Bitwise inclusive OR operator
bit_or_expr = bit_xor_expr {'|' {GEQ()} bit_xor_expr}

## 6.5.13 Logical AND operator
logical_AND_expr = bit_or_expr {'&&' {GEQ()} bit_or_expr}

## 6.5.14 Logical OR operator
logical_OR_expr = logical_AND_expr {'||' {GEQ()} logical_AND_expr}

## 6.5.15 Conditional operator // ?:
conditional_expr = logical_OR_expr
                 | logical_OR_expr '?' expression ':' conditional_expr

## 6.5.16 Assignment operators
assignment_expr = conditional_expr
                | unary_expr assignment_operator assignment_expr
assignment_operator = '=' | '*=' | '/=' | '%=' | '+=' | '-=' | '<<=' | '>>=' | '&=' | 'ˆ=' | '|='

## 6.5.17 Comma operator
expression = assignment_expr {',' assignment_expr}

# 6.6 Constant expressions // 常量表达式
const_expr = conditional_expr

# 6.9 External definitions  // 外部定义
translation_unit = { func_definition | declaration }

## 6.9.1  // 函数定义
func_definition = decl_specifiers declarator [declaration {declaration}] compound_stmt

# 6.7 Declarations // 声明
declaration = decl_specifiers [declarator ['=' initializer] {',' declarator ['=' initializer]}] ';'
decl_specifiers = storage_class_specifier [decl_specifiers]
                | type_specifier [decl_specifiers]
                | type_qualifier [decl_specifiers]
                | function_specifier [decl_specifiers]

## 6.7.1 Storage-class specifiers // 
storage_class_specifier = 'typedef' | 'extern' | 'static' | 'auto' | 'register'

## 6.7.2 Type specifiers // 类型说明符
type_specifier = 'void' | 'char' | 'short' | 'int' | 'long' 
               | 'float' | 'double' | 'signed' | 'unsigned'
               | '_Bool' | '_Complex' | '_Imaginary'
               | struct_or_union_specifier
               | enum_specifier
               #| typedef_name

### 6.7.2.1 Structure and union specifiers // 说明符
struct_or_union_specifier = struct_or_union [ID] '{' struct_decl_list '}'
                          | struct_or_union ID

struct_or_union = 'struct' | 'union'

struct_decl_list = struct_decl {struct_decl}

struct_decl = specifier_qualifier_list struct_declarator_list ';'

specifier_qualifier_list = type_specifier [specifier_qualifier_list]
                         | type_qualifier [specifier_qualifier_list]

struct_declarator_list = struct_declarator {',' struct_declarator}

struct_declarator = declarator | [declarator] ':' const_expr

### 6.7.2.2 Enumeration specifiers
enum_specifier = 'enum' [ID] '{' enumerator_list '}'
               | 'enum' [ID] '{' enumerator_list ',' '}'
               | 'enum' ID
               
enumerator_list = enumerator {',' enumerator}

enumerator = enumeration_constant
           | enumeration_constant '=' const_expr

### 6.7.2.3 Tags

## 6.7.3 Type qualifiers
type_qualifier = 'const' | 'restrict' | 'volatile'

## 6.7.4 Function specifiers
function_specifier = 'inline'

## 6.7.5 Declarators
// int * var, var2;
// int * (*pt)()
declarator = [pointer] ID direct_declarator_tail
           | [pointer] '(' declarator ')' direct_declarator_tail 
// 只支持定长数组,[]和[10]形式
direct_declarator_tail = '[' ']' direct_declarator_tail
                       |'[' conditional_expr ']' direct_declarator_tail

                       | '(' param_type_list ')' direct_declarator_tail                                  # Function declarators (including prototypes)
                       | '(' [ID_list] ')' direct_declarator_tail                                        # Function declarators (including prototypes)
                       | empty

pointer = '*' [type_qualifier_list] 
        | '*' [type_qualifier_list] pointer

type_qualifier_list = type_qualifier {type_qualifier}

param_type_list = param_list | param_list ',' '...'

param_list = param_decl {',' param_decl}

param_decl = decl_specifiers declarator | decl_specifiers [abstract_declarator]

ID_list = ID {',' ID}

## 6.7.6 Type names
type = specifier_qualifier_list [abstract_declarator]

abstract_declarator = pointer | [pointer] direct_abstract_declarator

direct_abstract_declarator = '(' abstract_declaratorr ')' direct_abstract_declarator_tail
                           | '[' [assignment_expr] ']' direct_abstract_declarator_tail
                           | '[' '*' ']' direct_abstract_declarator_tail
                           | '(' [param_type_list] ')' direct_abstract_declarator_tail

direct_abstract_declarator_tail = '[' [assignment_expr] ']' direct_abstract_declarator_tail
                                | '[' '*' ']' direct_abstract_declarator_tail
                                | '(' [param_type_list] ')' direct_abstract_declarator_tail
                                | empty

## 6.7.7 Type definitions
typedef_name = ID

## 6.7.8 Initialization
initializer = assignment_expr
            | '{' init_list [','] '}'

init_list = [designator_list '='] initializer {',' [designator_list '='] initializer}

designator_list = designator {designator}
designator = '[' const_expr ']' 
           | '.' ID

# 6.8 Statements and blocks
statement = labeled_stmt 
          | compound_stmt 
          | if_stmt
          | switch_stmt
          | for_stmt
          | while_stmt
          | do_while_stmt
          | expr_stmt 
          | goto_stmt
          | continue_stmt
          | return_stmt
          | break_stmt
          | case_label_stmt
          | default_label_stmt

## 6.8.1 Labeled statements
case_label_stmt = 'case' const_expr ':' statement 
default_label_stmt = 'default' ':' statement
labeled_stmt = ID ':' statement

## 6.8.2 Compound statement
compound_stmt = '{' {(declaration | statement)} '}' 

## 6.8.3 Expression and null statements
expr_stmt = ';' | expression ';'

## 6.8.4 Selection statements
if_stmt = 'if' '(' expression ')' statement
        | 'if' '(' expression ')' statement 'else' statement 
switch_stmt = 'switch' '(' expression ')' statement

## 6.8.5 Iteration statements
while_stmt = 'while' '(' expression ')' statement 
do_while_stmt = 'do' statement 'while' '(' expression ')' ';' 
for_stmt = 'for' '(' [expression] ';' [expression] ';' [expression] ')' statement 
         | 'for' '(' declaration [expression] ';' [expression] ')' statement

## 6.8.6 Jump statements
goto_stmt = 'goto' ID ';'
return_stmt = 'return' [expression] ';'
continue_stmt = 'continue' ';'
break_stmt = 'break' ';'