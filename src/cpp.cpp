#include <algorithm>
#include <iterator>
#include "cpp.h"
#include "error.h"


void Preprocessor::init()
{
    // ����Ĭ��·��
    _paths.push_back("D:/zcc/include/");
    _paths.push_back("C:/zcc/include/");

    // ����Ĭ�Ϻ궨��
    macros.push_back({ "__ZCC__", Token(T_INTEGER, "1"), M_PRE});
    macros.push_back({ "__ZCC_VERSION__", Token(T_STRING, "Version 0.02"), M_PRE });
    macros.push_back({ "__FILE__", M_PRE });
    macros.push_back({ "__LINE__", M_PRE });
    macros.push_back({ "__FUNC__", M_PRE });
    macros.push_back({ "__DATE__", M_PRE });
    macros.push_back({ "__STDC__", Token(T_INTEGER, "0"), M_PRE });
    macros.push_back({ "__TIME__" , M_PRE });
    macros.push_back({ "__cplusplus", Token(T_INTEGER, "0"), M_PRE });
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

            if (macro->_name == "__LINE__") {
                Token r(T_INTEGER, std::to_string(tok.getPos().line));
                macro->_replist.push_back(r);
                os.push_back(r);
                is.next();
            }
            else if (macro->_name == "__FILE__") {
                Token r(T_STRING, is.getFileName());
                macro->_replist.push_back(r);
                os.push_back(r);
                is.next();
            }
            else if (macro->_name == "__FUNC__") {
                is.next();
            }
            else if (macro->_name == "__DATE__") {
                Token r(T_STRING, Date::getDate());
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
                HideSet *hs = tok.hs_ ? tok.hs_ : new HideSet();
                hs->insert(macro->_name);
                TokenSequence substOs;

                subst(repTs, std::vector<std::string>(), TokenSequence(), hs, substOs);
                is.insert(substOs);
            }
            else if (macro->_type == M_FUNCLIKE) {
                tok = is.next();

                TokenSequence repTs = ts(macro->_name);
                HideSet *hs = tok.hs_ ? tok.hs_ : new HideSet();
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

	if (t.getType() == T_INTEGER || t.getType() == T_FLOAT) {
		return;
	}
		
	if (t.getType() == T_IDENTIFIER) {
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
 * @berif ���һ��Token�Ƿ��Ǻ��βΣ����򷵻���λ��
 * @param[in] t:Token
 * @param[fp] fp: formal parameters
 * @ret -1: no
 *      > -1: ��fp�е�λ��
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
 * @berif ��ts�����hs�е�Ԫ��
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

    // ���
    Token &tok = ts.next();
    if(tok.hs_ == nullptr) tok.hs_ = new HideSet();
    tok.hs_->insert(hs->begin(), hs->end());

    hasadd(hs, ts);
}

/**
 * @berif ����һ�������֣����������滻�б�
 * @param[in] _macro_name ������
 * @ret   rts ����滻�б�
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
 * @berif �Զ���Ϊ�ָ��ts���ҵ������ص�i������֮�������Token
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
 * @berif ��ts����ת��Ϊһ���ַ���Token
 * @param[in] ts: �����Tokensequence
 * @ret rts: ����һ���ַ�����Token����
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
 * @berif ������is�л�ȡʵ�ʲ���
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
 * @berif ���һ���ַ����Ƿ�Ϊ�궨��
 * @param[in] _n: ����������
 * @ret -1: ���Ǻ궨��
 *      > -1: �ں��б��е�λ��
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
 * @berif ���Ҳ����غ궨���ָ��
 * @param[in] _n: ������
 * @ret nullptr: δ�ҵ�
 *      !nullptr: ָ���ҵ��ĺ��ָ��
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

    // ���ļ�������
    for (const std::string path:_paths) {
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
void Preprocessor::_define_(TokenSequence &is)
{
    is.next();
    is.next();
    Macro _macro;
    Token t = is.next();
    if (t.getType() == T_IDENTIFIER) {
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
        if (t.getType() != T_NEWLINE) {
            do {
                _macro._replist.push_back(t);
                t = is.next();
            } while (t.getType() != T_NEWLINE);
        }
    }
    else {
        _macro._type = M_OBJLIKE;
        t = is.next();
        if (t.getType() != T_NEWLINE) {
            do {
                _macro._replist.push_back(t);
                t = is.next();
            } while (t.getType() != T_NEWLINE);
        }
    }
    if(!invalid_)
        macros.push_back(_macro);
}

/**
 * @berif #undef
 */
void Preprocessor::_undef_(TokenSequence &is)
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
void Preprocessor::_if_(TokenSequence &is)
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
void Preprocessor::_ifndef_(TokenSequence &is)
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
void Preprocessor::_ifdef_(TokenSequence &is)
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
void Preprocessor::_elif_(TokenSequence &is)
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
void Preprocessor::_else_(TokenSequence &is)
{
    is.expect('#');
    if (is.next().toString() != "else")
        error("not else .");

    if (is.next().getType() != T_NEWLINE)
        error("else need a new line.");

    invalid_ = cheak_else() || preInvalid_;

    stk_if_else.push_back({ "else", invalid_ });
}


void Preprocessor::_endif_(TokenSequence &is)
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


void Preprocessor::_line_(TokenSequence &is)
{

}
void Preprocessor::_pragma_(TokenSequence &is)
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