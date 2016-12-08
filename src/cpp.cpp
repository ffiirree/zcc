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

void Preprocessor::expand(TokenSquence is, TokenSquence &os)
{
    while (!is.end())
    {
        Macro *macro;
        int index = 0;
        std::string _name;
        Token tok = is.peek();

        if (tok.getType() == KEYWORD && tok.getId() == '#') {
            group_part(is, os);
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
                is.next();
                TokenSquence repTs = ts(macro->_name);
                HideSet hs;
                hs.insert(macro->_name);
                TokenSquence substOs;

                subst(repTs, std::vector<std::string>(), TokenSquence(), hs, substOs);
                os.insertBack(substOs);
            }
            else if (macro->_type == M_FUNCLIKE) {
                is.next();
                TokenSquence repTs = ts(macro->_name);
                HideSet hs;
                TokenSquence ap = getAP(is);
                TokenSquence substOs;
                subst(repTs, macro->_params, ap, hs, substOs);
                os.insertBack(substOs);
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
	Token t = is.next();
     t = is.next();

	if (t.getType() == TNEWLINE)
		return;

	if (t.getType() == INTEGER || t.getType() == FLOAT) {
		// linemarker(tok);
		return;
	}
		
	if (t.getType() == ID) {
        if (t.getSval() == "if") {

        }
        else if (t.getSval() == "ifdef") {

        }
        else if (t.getSval() == "ifndef") {

        }
        else if (t.getSval() == "elif") {

        }
        else if (t.getSval() == "else") {
        }
        else if (t.getSval() == "endif") {

        }
        else if (t.getSval() == "include") {
            Include(is, os);
        }
        else if (t.getSval() == "define") {
            PP_define(is);
        }
        else if (t.getSval() == "undef") {

        }
        else if (t.getSval() == "line") {

        }
	}
}

/**
 * @berif
 * @param is[in]: input squence
 * @param fp: formal parameters
 * @param ap: Actual Parameters
 * @param hs: hide set
 * @param os: output sqence
 */ 
void Preprocessor::subst(TokenSquence &is, std::vector<std::string> fp, TokenSquence &ap, HideSet& hs, TokenSquence& os)
{
    int iOfFP = 0;
    if (is.end()) {
        hasadd(hs, os);
        return;
    }
    else if (is.test('#') && (iOfFP = isInFP(is.peek2(), fp)) != -1) {
        return;
    }
    else if (is.test(DS) && (iOfFP = isInFP(is.peek2(), fp)) != -1) {
        return;
    }
    else if (is.test(DS)) {
        return;
    }
    else if ((iOfFP = isInFP(is.peek(), fp)) != -1 && is.test2(DS)) {
        return;
    }
    else if ((iOfFP = isInFP(is.peek(), fp)) != -1) {
        is.next();
        TokenSquence repAp;
        expand(select(iOfFP, ap), repAp);
        os.insertBack(repAp);
        subst(is, fp, ap, hs, os);
        return;
    }

    os.push_back(is.next());
    subst(is, fp, ap, hs, os);
}

void Preprocessor::Include(TokenSquence &is, TokenSquence &os)
{
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
    error("can not open file : %s.", _fn);
}


/**
| '#' 'define' ID replacement_list new_line
| '#' 'define' ID lparen [ID_list] ) replacement_list new_line
//| '#' 'define' ID lparen ... ) replacement_list new_line
//| '#' 'define' ID lparen ID_list , ... ) replacement_list new_line
*/
void Preprocessor::PP_define(Lex &is)
{
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
    macros.push_back(_macro);
}

int Preprocessor::isInFP(Token &t, std::vector<std::string> fp)
{
    for (size_t i = 0; i < fp.size(); ++i) {
        if (t.getType() == ID && t.getSval() == fp.at(i))
            return i;
    }
    return -1;
}
void Preprocessor::glue(TokenSquence &ls, TokenSquence &rs)
{

}
void Preprocessor::hasadd(HideSet &hs, TokenSquence &ts)
{

}
TokenSquence Preprocessor::ts(const std::string &_macro_name)
{
    TokenSquence rts;

    for (const auto &_m : macros) {
        if (_m._name == _macro_name)
            return _m._replist;
    }

    error("Do not have this macros :%s.", _macro_name);
    return rts;
}
TokenSquence Preprocessor::fp(const std::string &_macro_name)
{
    TokenSquence rts;
    return rts;
}
TokenSquence Preprocessor::select(int i, TokenSquence &ts)
{
    size_t counter = 0;
    TokenSquence rts;
    for (size_t i = 0; i < ts.size(); ++i) {
        if (ts.at(i).getType() == KEYWORD && ts.at(i).getId() == ',')
            counter++;
        if (counter == i)
            rts.push_back(ts.at(i));
    }
    return rts;
}
std::string Preprocessor::stringize(TokenSquence &ts)
{
    return std::string();
}

TokenSquence Preprocessor::getAP(TokenSquence &is)
{
    TokenSquence rts;
    is.expect('(');
    while (!is.next_is(')')) {
        if (!is.next_is(','))
            rts.push_back(is.next());
    }

    return rts;
}

int Preprocessor::isMacro(const std::string &_n)
{
    for (size_t i = 0;i < macros.size(); ++i) {
        if (_n == macros.at(i)._name) {
            return i;
        }
    }
    return -1;
}


Macro *Preprocessor::searchMacro(const std::string &_n)
{
    for (auto iter = macros.begin(); iter != macros.end(); ++iter) {
        if (_n == (*iter)._name)
            return iter._Ptr;
    }
    return nullptr;
}
