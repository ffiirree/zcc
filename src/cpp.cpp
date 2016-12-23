#include <algorithm>
#include <iterator>
#include "cpp.h"
#include "error.h"


void Preprocessor::init()
{
    // 包含默认路径
    paths_.push_back("D:/zcc/include/");
    paths_.push_back("C:/zcc/include/");

    // 包含默认宏定义
    macros_.push_back({ "__ZCC__", Token(T_INTEGER, "1"), Macro::M_PRE});
    macros_.push_back({ "__ZCC_VERSION__", Token(T_STRING, "Version 0.02"), Macro::M_PRE });
    macros_.push_back({ "__FILE__", Macro::M_PRE });
    macros_.push_back({ "__LINE__", Macro::M_PRE });
    macros_.push_back({ "__FUNC__", Macro::M_PRE });
    macros_.push_back({ "__DATE__", Macro::M_PRE });
    macros_.push_back({ "__STDC__", Token(T_INTEGER, "0"), Macro::M_PRE });
    macros_.push_back({ "__TIME__" , Macro::M_PRE });
    macros_.push_back({ "__cplusplus", Token(T_INTEGER, "0"), Macro::M_PRE });
}

void Preprocessor::expand(TokenSequence is, TokenSequence &os)
{
    while (!is.end())
    {
        Macro *macro;
        int index = 0;
        std::string _name;
        Token tok = is.peek();

        if (tok.getType() == T_KEYWORD && tok.getId() == '#') {
            if (isExpandExpr)
                return;
            group_part(is, os);
        }
        else if (invalid_) {
            is.next();
        }
        else if (!tok.needExpand()) {
            if (isOnlyPP) {
                os.push_back(tok);
            }
            else if (tok.getType() != T_NEWLINE) {
                os.push_back(tok);
            }
            is.next();
        }
        else if(tok.getType() == T_IDENTIFIER && ((macro = searchMacro(tok.getSval())))){

            if (macro->name_ == "__LINE__") {
                Token r(T_INTEGER, std::to_string(tok.getPos().line));
                macro->replist_.push_back(r);
                os.push_back(r);
                is.next();
            }
            else if (macro->name_ == "__FILE__") {
                Token r(T_STRING, is.getFileName());
                macro->replist_.push_back(r);
                os.push_back(r);
                is.next();
            }
            else if (macro->name_ == "__FUNC__") {
                is.next();
            }
            else if (macro->name_ == "__DATE__") {
                Token r(T_STRING, Date::getDate());
                macro->replist_.push_back(r);
                os.push_back(r);
                is.next();
            }
            else if (macro->name_ == "__TIME__") {
                is.next();
            }
            else if(macro->type_ == Macro::M_OBJLIKE){
                tok = is.next();
                TokenSequence repTs = ts(macro->name_);
                HideSet *hs = tok.hs_ ? tok.hs_ : new HideSet();
                hs->insert(macro->name_);
                TokenSequence substOs;

                subst(repTs, std::vector<std::string>(), TokenSequence(), hs, substOs);
                is.insert(substOs);
            }
            else if (macro->type_ == Macro::M_FUNCLIKE) {
                tok = is.next();

                TokenSequence repTs = ts(macro->name_);
                HideSet *hs = tok.hs_ ? tok.hs_ : new HideSet();
                hs->insert(macro->name_);
                TokenSequence ap = getAP(is);
                TokenSequence substOs;
                subst(repTs, macro->params_, ap, hs, substOs);
                is.insert(substOs);
            }
        }
        else {
            if (isOnlyPP) {
                os.push_back(tok);
            }
            else if (tok.getType() != T_NEWLINE) {
                os.push_back(tok);
            }  
            is.next();
        }
    }
}



void Preprocessor::group_part(TokenSequence &is, TokenSequence &os)
{
	Token t = is.peek2();

	if (t.getType() == T_NEWLINE)
		return;

	if (t.getType() == T_INTEGER || t.getType() == T_FLOAT)
		return;
		
	if (t.getType() == T_IDENTIFIER) {
        if (t.getSval() == "if")           If(is);
        else if (t.getSval() == "ifdef")   Ifdef(is);
        else if (t.getSval() == "ifndef")  Ifndef(is);
        else if (t.getSval() == "elif")    Elif(is);
        else if (t.getSval() == "else")    Else(is);
        else if (t.getSval() == "endif")   Endif(is);
        else if (t.getSval() == "include") Include(is, os);
        else if (t.getSval() == "define")  Define(is);
        else if (t.getSval() == "undef")   Undef(is);
        else if (t.getSval() == "line")    Line(is);
        else if (t.getSval() == "pragma")  Pragma(is);
	}
}

/**
 * @berif
 * @param is[in]: input sequence
 * @param fp: formal parameters
 * @param ap: Actual Parameters
 * @param hs: hide set
 * @param os: output sequence
 * @ret None
 */ 
void Preprocessor::subst(TokenSequence &is, std::vector<std::string> fp, TokenSequence &ap, HideSet* hs, TokenSequence& os)
{
    int iOfFP = 0;
    if (is.end()) {
        hasadd(hs, os);
        return;
    }
    else if (is.test('#') && (iOfFP = isInFP(is.peek2(), fp)) != -1) {
        is.next();
        is.next();
        os.insertBack(stringize(select(iOfFP, ap)));
        subst(is, fp, ap, hs, os);
        return;
    }
    else if (is.test(OP_DS) && (iOfFP = isInFP(is.peek2(), fp)) != -1) {
        is.next();
        is.next();

        if (select(iOfFP, ap).empty())
            subst(is, fp, ap, hs, os);
        else {
            glue(os, select(iOfFP, ap));
            subst(is, fp, ap, hs, os);
        }
            
        return;
    }
    else if (is.test(OP_DS)) {
        is.next(); // ##
        TokenSequence t;
        t.push_back(is.next()); //T
        glue(os, t);
        subst(is, fp, ap, hs, os);
        return;
    }
    else if ((iOfFP = isInFP(is.peek(), fp)) != -1 && is.test2(OP_DS)) {
        is.next();
        is.next();
        if (select(iOfFP, ap).empty()) {
            if ((iOfFP = isInFP(is.peek(), fp)) != -1) {
                is.next();
                os.insertBack(select(iOfFP, ap));
                subst(is, fp, ap, hs, os);
            }
            else {
                return subst(is, fp, ap, hs, os);
            }
        }
        else {
            is.back();
            os.insertBack(select(iOfFP, ap));
            subst(is, fp, ap, hs, os);
        }
        return;
    }
    else if ((iOfFP = isInFP(is.peek(), fp)) != -1) {
        is.next();
        TokenSequence repAp;
        TokenSequence getAP = select(iOfFP, ap);
        expand(getAP, repAp);
        os.insertBack(repAp);
        subst(is, fp, ap, hs, os);
        return;
    }

    os.push_back(is.next());
    subst(is, fp, ap, hs, os);
}


/**
 * @berif 检查一个Token是否是宏形参，是则返回其位置
 * @param[in] t:Token
 * @param[fp] fp: formal parameters
 * @ret -1: no
 *      > -1: 在fp中的位置
 */
int Preprocessor::isInFP(Token &t, std::vector<std::string> fp)
{
    for (size_t i = 0; i < fp.size(); ++i) {
        if (t.getType() == T_IDENTIFIER && t.getSval() == fp.at(i))
            return i;
    }
    return -1;
}
void Preprocessor::glue(TokenSequence &ls, TokenSequence &rs)
{
    if (ls.end() || rs.end())
        error("invlid ls||rs.");

    if (ls.restSize() == 1) {

        Token lt = ls.next();
        Token rt = rs.next();

        Token gt = Token(T_IDENTIFIER, lt.toString() + rt.toString());
        gt.hs_ = new HideSet();

        if(lt.hs_ != nullptr && rt.hs_ != nullptr)
            std::set_intersection(lt.hs_->begin(), lt.hs_->end(), rt.hs_->begin(), rt.hs_->end(), std::inserter(*gt.hs_, gt.hs_->begin()));

        ls.pop_back();
        ls.push_back(gt);
        ls.back();
        while(!rs.end())
            ls.push_back(rs.next());

        return;
    }

    ls.next();
    glue(ls, rs);
}

/**
 * @berif 向ts中添加hs中的元素
 * @param[in] hs: Hide set
 * @param[out] ts: TokenSequence
 * @ret None
 */
void Preprocessor::hasadd(HideSet *hs, TokenSequence &ts)
{
    if (ts.end())
        return;
    if (!hs)
        return;

    // 如果
    Token &tok = ts.next();
    if(tok.hs_ == nullptr) tok.hs_ = new HideSet();
    tok.hs_->insert(hs->begin(), hs->end());

    hasadd(hs, ts);
}

/**
 * @berif 接收一个宏名字，返回它的替换列表
 * @param[in] _macro_name 宏名字
 * @ret   rts 宏的替换列表
 */
TokenSequence Preprocessor::ts(const std::string &_macro_name)
{
    TokenSequence rts;

    for (const auto &m : macros_) {
        if (m.name_ == _macro_name)
            return m.replist_;
    }

    error("Do not have this macros :%s.", _macro_name.c_str());
    return rts;
}
TokenSequence Preprocessor::fp(const std::string &_macro_name)
{
    TokenSequence rts;
    return rts;
}

/**
 * @berif 以逗号为分割从ts中找到并返回第i个逗号之后的所有Token
 * @param[in] _i
 * @param[in] ts
 * @ret rts
 */
TokenSequence Preprocessor::select(int _i, TokenSequence &ts)
{
    size_t counter = 0;
    TokenSequence rts;
    for (size_t i = 0; i < ts.size(); ++i) {
        if (ts.at(i).getType() == T_KEYWORD && ts.at(i).getId() == ',')
            counter++;
        if (counter == _i && ts.at(i).toString() != ",")
            rts.push_back(ts.at(i));
    }
    return rts;
}

/**
 * @berif 将ts序列转换为一个字符串Token
 * @param[in] ts: 输入的Tokensequence
 * @ret rts: 包含一个字符串的Token序列
 */
TokenSequence Preprocessor::stringize(TokenSequence &ts)
{
    TokenSequence rts;
    std::string str;
    for (size_t i = 0; i < ts.size(); ++i) {
        str += ts.at(i).toString();
    }
    rts.push_back(Token(T_STRING, str));
    return rts;
}

/**
 * @berif 从输入is中获取实际参数
 * @param[in] is: input sequence
 * @ret rts: actuls parameters
 */
TokenSequence Preprocessor::getAP(TokenSequence &is)
{
    TokenSequence rts;
    is.expect('(');
    while (!is.next_is(')')) {
        rts.push_back(is.next());
    }

    return rts;
}

/**
 * @berif 检查一个字符串是否为宏定义
 * @param[in] _n: 待检查的名字
 * @ret -1: 不是宏定义
 *      > -1: 在宏列表中的位置
 */
int Preprocessor::isMacro(const std::string &_n)
{
    for (size_t i = 0;i < macros_.size(); ++i) {
        if (_n == macros_.at(i).name_) {
            return i;
        }
    }
    return -1;
}

/**
 * @berif 查找并返回宏定义的指针
 * @param[in] _n: 宏名字
 * @ret nullptr: 未找到
 *      !nullptr: 指向找到的宏的指针
 */
Macro *Preprocessor::searchMacro(const std::string &_n)
{
    for (auto iter = macros_.begin(); iter != macros_.end(); ++iter) {
        if (_n == (*iter).name_)
            return iter._Ptr;
    }
    return nullptr;
}

bool Preprocessor::deleteMacro(const std::string &_n)
{
    for (auto iter = macros_.begin(); iter != macros_.end(); ++iter) {
        if (_n == (*iter).name_)
            macros_.erase(iter);
        return true;
    }
    return false;
}


void Preprocessor::Include(TokenSequence &is, TokenSequence &os)
{
    is.next();
    is.next();

    std::string _fn;

    bool isQuot = false;
    if (is.test('\"'))
        isQuot = true;
    else if (is.test('<'))
        isQuot = false;
    else
        error("error for include file.");

    is.next();

    if (!isQuot) {
        while (!is.test('>')) {
            _fn += is.next().toString();
        }
        is.expect('>');
    }
    else {
        _fn = is.next().toString();
        is.expect('\"');
    }
    if (is.next().getType() != T_NEWLINE)
        error("need new_line.");

    // 打开文件并插入
    for (const std::string path: paths_) {
        std::string _file = path + _fn;
        std::ifstream in(_file, std::ios::_Nocreate);
        if (in.is_open()) {
            TokenSequence ts;
            Lex lex(_file, ts);
            is.insert(ts);
            return;
        }
    }
    error("can not open file : %s.", _fn.c_str());
}


/**
 * @berif #define 
 */
void Preprocessor::Define(TokenSequence &is)
{
    is.next();
    is.next();
    Macro _macro;
    Token t = is.next();
    if (t.getType() == T_IDENTIFIER) {
        _macro.name_ = t.getSval();
    }

    if (is.next_is('(')) {
        _macro.type_ = Macro::M_FUNCLIKE;

        if (!is.next_is(')')) {
            do {
                t = is.next();
                _macro.params_.push_back(t.getSval());
            } while (is.next_is(','));

            is.expect(')');
        }

        t = is.next();
        if (t.getType() != T_NEWLINE) {
            do {
                _macro.replist_.push_back(t);
                t = is.next();
            } while (t.getType() != T_NEWLINE);
        }
    }
    else {
        _macro.type_ = Macro::M_OBJLIKE;
        t = is.next();
        if (t.getType() != T_NEWLINE) {
            do {
                _macro.replist_.push_back(t);
                t = is.next();
            } while (t.getType() != T_NEWLINE);
        }
    }
    if(!invalid_)
        macros_.push_back(_macro);
}

/**
 * @berif #undef
 */
void Preprocessor::Undef(TokenSequence &is)
{
    is.expect('#');
    if (is.next().toString() != "undef")
        error("not undef macro.");

    if (is.peek().getType() != T_IDENTIFIER)
        error("undef need a iden.");

    deleteMacro(is.next().getSval());
    
    if (is.next().getType() != T_NEWLINE)
        error("undef end with a new_line.");
}

/**
 * @berif #if
 */
void Preprocessor::If(TokenSequence &is)
{
    _BEGIN_IF_();

    is.expect('#');
    if (is.next().toString() != "if")
        error("not if macro.");

    TokenSequence is_ = is, ts;
    isExpandExpr = true;
    expand(is_, ts);
    ts.push_back(Token(T_KEYWORD, ';'));
    isExpandExpr = false;

    Parser parser(ts);
    invalid_ = (!parser.compute_bool_expr()) || preInvalid_;

    while (is.next().getType() != T_NEWLINE);

    stk_if_else.push_back({ "if", invalid_ });
}
void Preprocessor::Ifndef(TokenSequence &is)
{
    _BEGIN_IF_();

    is.next();
    is.next();
    Token tok = is.next();
    if (tok.getType() != T_IDENTIFIER)
        errorp(tok.getPos(), "ifdef need a iden....");

    if (is.next().getType() != T_NEWLINE)
        errorp(tok.getPos(), "should new line.");

    invalid_ = searchMacro(tok.getSval()) || preInvalid_;
    stk_if_else.push_back({ "if", invalid_ });
}

/**
 * @berif #ifdef
 */
void Preprocessor::Ifdef(TokenSequence &is)
{
    _BEGIN_IF_();

    is.next(); // #
    is.next(); // ifdef
    Token tok = is.next();
    if (tok.getType() != T_IDENTIFIER)
        errorp(tok.getPos(), "ifdef need a iden....");

    if (is.next().getType() != T_NEWLINE)
        errorp(tok.getPos(), "should new line.");

    invalid_ = !searchMacro(tok.getSval()) || preInvalid_;
    stk_if_else.push_back({ "if", invalid_ });
}

/**
 * @berif #elif
 */
void Preprocessor::Elif(TokenSequence &is)
{
    invalid_ = false;

    is.expect('#');
    if (is.next().toString() != "elif")
        error("not if macro.");

    TokenSequence is_ = is, ts;
    isExpandExpr = true;
    expand(is_, ts);
    ts.push_back(Token(T_KEYWORD, ';'));
    isExpandExpr = false;

    Parser parser(ts);
    invalid_ = !parser.compute_bool_expr() || preInvalid_ || cheak_else();

    while (is.next().getType() != T_NEWLINE);

    stk_if_else.push_back({ "elif", invalid_ });
}
/**
 * @berif #elif_groups
 */
void Preprocessor::Else(TokenSequence &is)
{
    is.expect('#');
    if (is.next().toString() != "else")
        error("not else .");

    if (is.next().getType() != T_NEWLINE)
        error("else need a new line.");

    invalid_ = cheak_else() || preInvalid_;

    stk_if_else.push_back({ "else", invalid_ });
}


void Preprocessor::Endif(TokenSequence &is)
{
    is.expect('#');
    if (is.next().toString() != "endif")
        error("not if macro.");
    if (is.next().getType() != T_NEWLINE)
        error("endif should be end with new_line.");


    while (!stk_if_else.empty() && stk_if_else.back().first != "if")
        stk_if_else.pop_back();
    stk_if_else.pop_back();

    _END_IF_();
}


void Preprocessor::Line(TokenSequence &is)
{

}
void Preprocessor::Pragma(TokenSequence &is)
{

}
bool Preprocessor::cheak_else() {
    for (size_t i = stk_if_else.size(); i > 0; --i) {
        if (stk_if_else.at(i - 1).first == "if") {
            return !stk_if_else.at(i - 1).second;
        }
        else {
            if (stk_if_else.at(i - 1).second == true)
                return false;
        }
    }

    error("no if for else.");
    return false;
}