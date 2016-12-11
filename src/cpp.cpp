#include <algorithm>
#include <iterator>
#include "pp.h"
#include "error.h"


void Preprocessor::init()
{
    // 包含默认路径
    _paths.push_back("D:/zcc/include/");
    _paths.push_back("C:/zcc/include/");

    // 包含默认宏定义
    macros.push_back({ "__ZCC__", Token(INTEGER, "1"), M_PRE});
    macros.push_back({ "__ZCC_VERSION__", Token(STRING_, "Version 0.02"), M_PRE });
    macros.push_back({ "__FILE__", M_PRE });
    macros.push_back({ "__LINE__", M_PRE });
    macros.push_back({ "__FUNC__", M_PRE });
    macros.push_back({ "__DATE__", M_PRE });
    macros.push_back({ "__STDC__", Token(INTEGER, "0"), M_PRE });
    macros.push_back({ "__TIME__" , M_PRE });
    macros.push_back({ "__cplusplus", Token(INTEGER, "0"), M_PRE });
}

void Preprocessor::expand(TokenSequence is, TokenSequence &os)
{
    while (!is.end())
    {
        Macro *macro;
        int index = 0;
        std::string _name;
        Token tok = is.peek();

        if (tok.getType() == KEYWORD && tok.getId() == '#') {
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
            else if (tok.getType() != TNEWLINE) {
                os.push_back(tok);
            }
            is.next();
        }
        else if(tok.getType() == ID && ((macro = searchMacro(tok.getSval())))){

            if (macro->_name == "__LINE__") {
                Token r(INTEGER, std::to_string(tok.getPos().line));
                macro->_replist.push_back(r);
                os.push_back(r);
                is.next();
            }
            else if (macro->_name == "__FILE__") {
                Token r(STRING_, is.getCurrentFile().getFileName());
                macro->_replist.push_back(r);
                os.push_back(r);
                is.next();
            }
            else if (macro->_name == "__FUNC__") {
                is.next();
            }
            else if (macro->_name == "__DATE__") {
                Token r(STRING_, Date::getDate());
                macro->_replist.push_back(r);
                os.push_back(r);
                is.next();
            }
            else if (macro->_name == "__TIME__") {
                is.next();
            }
            else if(macro->_type == M_OBJLIKE){
                tok = is.next();
                TokenSequence repTs = ts(macro->_name);
                HideSet *hs = tok._hs ? tok._hs : new HideSet();
                hs->insert(macro->_name);
                TokenSequence substOs;

                subst(repTs, std::vector<std::string>(), TokenSequence(), hs, substOs);
                is.insert(substOs);
            }
            else if (macro->_type == M_FUNCLIKE) {
                tok = is.next();

                TokenSequence repTs = ts(macro->_name);
                HideSet *hs = tok._hs ? tok._hs : new HideSet();
                hs->insert(macro->_name);
                TokenSequence ap = getAP(is);
                TokenSequence substOs;
                subst(repTs, macro->_params, ap, hs, substOs);
                is.insert(substOs);
            }
        }
        else {
            if (isOnlyPP) {
                os.push_back(tok);
            }
            else if (tok.getType() != TNEWLINE) {
                os.push_back(tok);
            }  
            is.next();
        }
    }
}



void Preprocessor::group_part(Lex &is, Lex &os)
{
	Token t = is.peek2();

	if (t.getType() == TNEWLINE)
		return;

	if (t.getType() == INTEGER || t.getType() == FLOAT) {
		return;
	}
		
	if (t.getType() == ID) {
        if (t.getSval() == "if") {
            _if_(is);
        }
        else if (t.getSval() == "ifdef") {
            _ifdef_(is);
        }
        else if (t.getSval() == "ifndef") {
            _ifndef_(is);
        }
        else if (t.getSval() == "elif") {
            _elif_(is);
        }
        else if (t.getSval() == "else") {
            _else_(is);
        }
        else if (t.getSval() == "endif") {
            _endif_(is);
        }
        else if (t.getSval() == "include") {
            Include(is, os);
        }
        else if (t.getSval() == "define") {
            _define_(is);
        }
        else if (t.getSval() == "undef") {
            _undef_(is);
        }
        else if (t.getSval() == "line") {
            _line_(is);
        }
        else if (t.getSval() == "pragma") {
            _pragma_(is);
        }
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
    else if (is.test(DS) && (iOfFP = isInFP(is.peek2(), fp)) != -1) {
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
    else if (is.test(DS)) {
        is.next(); // ##
        TokenSequence t;
        t.push_back(is.next()); //T
        glue(os, t);
        subst(is, fp, ap, hs, os);
        return;
    }
    else if ((iOfFP = isInFP(is.peek(), fp)) != -1 && is.test2(DS)) {
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
        if (t.getType() == ID && t.getSval() == fp.at(i))
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

        Token gt = Token(ID, lt.to_string() + rt.to_string());
        gt._hs = new HideSet();

        if(lt._hs != nullptr && rt._hs != nullptr)
            std::set_intersection(lt._hs->begin(), lt._hs->end(), rt._hs->begin(), rt._hs->end(), std::inserter(*gt._hs, gt._hs->begin()));

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
    tok._hs ? !nullptr : tok._hs = new HideSet();
    tok._hs->insert(hs->begin(), hs->end());

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

    for (const auto &_m : macros) {
        if (_m._name == _macro_name)
            return _m._replist;
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
        if (ts.at(i).getType() == KEYWORD && ts.at(i).getId() == ',')
            counter++;
        if (counter == _i && ts.at(i).to_string() != ",")
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
        str += ts.at(i).to_string();
    }
    rts.push_back(Token(STRING_, str));
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
    for (size_t i = 0;i < macros.size(); ++i) {
        if (_n == macros.at(i)._name) {
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
    for (auto iter = macros.begin(); iter != macros.end(); ++iter) {
        if (_n == (*iter)._name)
            return iter._Ptr;
    }
    return nullptr;
}

bool Preprocessor::deleteMacro(const std::string &_n)
{
    for (auto iter = macros.begin(); iter != macros.end(); ++iter) {
        if (_n == (*iter)._name)
            macros.erase(iter);
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
            _fn += is.next().to_string();
        }
        is.expect('>');
    }
    else {
        _fn = is.next().to_string();
        is.expect('\"');
    }
    if (is.next().getType() != TNEWLINE)
        error("need new_line.");

    // 打开文件并插入
    for (const std::string path:_paths) {
        std::string _file = path + _fn;
        std::ifstream in(_file, std::ios::_Nocreate);
        if (in.is_open()) {
            Lex lex(_file);
            lex.pop_back();        // 删除EOF
            is.insert(lex);
            return;
        }
    }
    error("can not open file : %s.", _fn.c_str());
}


/**
 * @berif #define 
 */
void Preprocessor::_define_(Lex &is)
{
    is.next();
    is.next();
    Macro _macro;
    Token t = is.next();
    if (t.getType() == ID) {
        _macro._name = t.getSval();
    }

    if (is.next_is('(')) {
        _macro._type = M_FUNCLIKE;

        if (!is.next_is(')')) {
            do {
                t = is.next();
                _macro._params.push_back(t.getSval());
            } while (is.next_is(','));

            is.expect(')');
        }

        t = is.next();
        if (t.getType() != TNEWLINE) {
            do {
                _macro._replist.push_back(t);
                t = is.next();
            } while (t.getType() != TNEWLINE);
        }
    }
    else {
        _macro._type = M_OBJLIKE;
        t = is.next();
        if (t.getType() != TNEWLINE) {
            do {
                _macro._replist.push_back(t);
                t = is.next();
            } while (t.getType() != TNEWLINE);
        }
    }
    if(!invalid_)
        macros.push_back(_macro);
}

/**
 * @berif #undef
 */
void Preprocessor::_undef_(Lex &is)
{
    is.expect('#');
    if (is.next().to_string() != "undef")
        error("not undef macro.");

    if (is.peek().getType() != ID)
        error("undef need a iden.");

    deleteMacro(is.next().getSval());
    
    if (is.next().getType() != TNEWLINE)
        error("undef end with a new_line.");
}

/**
 * @berif #if
 */
void Preprocessor::_if_(Lex &is)
{
    _BEGIN_IF_();

    is.expect('#');
    if (is.next().to_string() != "if")
        error("not if macro.");

    TokenSequence is_ = is, ts;
    isExpandExpr = true;
    expand(is_, ts);
    ts.push_back(Token(KEYWORD, ';'));
    isExpandExpr = false;

    Parser parser(ts);
    invalid_ = (!parser.compute_bool_expr()) || preInvalid_;

    while (is.next().getType() != TNEWLINE);

    stk_if_else.push_back({ "if", invalid_ });
}
void Preprocessor::_ifndef_(Lex &is)
{
    _BEGIN_IF_();

    is.next();
    is.next();
    Token tok = is.next();
    if (tok.getType() != ID)
        errorp(tok.getPos(), "ifdef need a iden....");

    if (is.next().getType() != TNEWLINE)
        errorp(tok.getPos(), "should new line.");

    invalid_ = searchMacro(tok.getSval()) || preInvalid_;
    stk_if_else.push_back({ "if", invalid_ });
}

/**
 * @berif #ifdef
 */
void Preprocessor::_ifdef_(Lex &is)
{
    _BEGIN_IF_();

    is.next(); // #
    is.next(); // ifdef
    Token tok = is.next();
    if (tok.getType() != ID)
        errorp(tok.getPos(), "ifdef need a iden....");

    if (is.next().getType() != TNEWLINE)
        errorp(tok.getPos(), "should new line.");

    invalid_ = !searchMacro(tok.getSval()) || preInvalid_;
    stk_if_else.push_back({ "if", invalid_ });
}

/**
 * @berif #elif
 */
void Preprocessor::_elif_(Lex &is)
{
    invalid_ = false;

    is.expect('#');
    if (is.next().to_string() != "elif")
        error("not if macro.");

    TokenSequence is_ = is, ts;
    isExpandExpr = true;
    expand(is_, ts);
    ts.push_back(Token(KEYWORD, ';'));
    isExpandExpr = false;

    Parser parser(ts);
    invalid_ = !parser.compute_bool_expr() || preInvalid_ || cheak_else();

    while (is.next().getType() != TNEWLINE);

    stk_if_else.push_back({ "elif", invalid_ });
}
/**
 * @berif #elif_groups
 */
void Preprocessor::_else_(Lex &is)
{
    is.expect('#');
    if (is.next().to_string() != "else")
        error("not else .");

    if (is.next().getType() != TNEWLINE)
        error("else need a new line.");

    invalid_ = cheak_else() || preInvalid_;

    stk_if_else.push_back({ "else", invalid_ });
}


void Preprocessor::_endif_(Lex &is)
{
    is.expect('#');
    if (is.next().to_string() != "endif")
        error("not if macro.");
    if (is.next().getType() != TNEWLINE)
        error("endif should be end with new_line.");


    while (!stk_if_else.empty() && stk_if_else.back().first != "if")
        stk_if_else.pop_back();
    stk_if_else.pop_back();

    _END_IF_();
}


void Preprocessor::_line_(Lex &is)
{

}
void Preprocessor::_pragma_(Lex &is)
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