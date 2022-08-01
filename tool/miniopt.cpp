/**
 * The MIT License
 *
 * Copyright 2022 Krishna sssky307@163.com
 *
 * For the full copyright and license information, please view the LICENSE
 * file that was distributed with this source code.
 */

/**
 *  Code generator.
 */

#include "config.h"
#include "miniopt.h"
#include "packres.h"

#include <string>
#include <iostream>
#include <fstream>
#include <sstream>
#include <regex>
#include <vector>
#include <ctime>
#include <set>

/**
 * @brief Option info.
 */
struct OptInfo
{
    std::string shortName;
    std::string longName;
    std::string argHint;
    std::vector<std::string> descList;
    void clear(){
        shortName.clear();
        longName.clear();
        argHint.clear();
        descList.clear();
    }
};

using OptInfoArray = std::vector<OptInfo>;

enum class Result{
    eof,        ///< End of File.
    option,     ///< Match an option.
    comment,    ///< Match a comment.
    unmatched,  ///< Cannot match anything.
    empty       ///< Empty line.
};

/**
 * @brief Check whether a line of string is empty.
 * 
 * If all chars is space, it is seen as empty.
 * 
 * @param[in] line  Input line.
 * 
 * @return true     Empty line.
 * @return false    Not empty line.
 */
bool IsEmpty(const std::string &line) {
    static const std::regex re("([[:s:]]+)");

    if (line.empty()) return true;
    std::smatch match;
    if (std::regex_match(line, match, re)) return true;
    return false;
}

/**
 * @brief C++ std::regex patterns.
 */
#define a     "[^\\-=[:s:]]"                  // a short key char
#define ck    "(" a ")"                       // capture short key
#define ckey  "(" a "[^=[:s:]]{0,31}" ")"     // capture long key
#define msp   "[[:s:]]*"                      // may space
#define hsp   "[[:s:]]+"                      // has space
#define meos  "(?:[\t ]*[=]?[\t ]*)"          // may equal or space
#define heos  "(?:(?:[\t ]*=[\t ]*)|[\t ]+)"  // has equal or space
#define d1    "[\\-]"                         // dash
#define d2    "[\\-][\\-]"                    // dash dash
#define hsoc  "(?:(?:[\t ]*,[\t ]*)|[\t ]+)"  // has space or comma
#define cv    "(" "<[^[:s:]]+" ")"            // capture value, need check '>'
#define cd    "(" ".*" ")"                    // capture description

/**
 * @brief   Convert a line of string to OptInfo.
 *
 * The output depends on the result.
 * 
 * @param[in] line          Input line.
 * @param[out] OptInfo      Output OptInfo.
 * @param[out] comment      Output comment.
 * 
 * @return Result
 */
Result LineToOptInfo(const std::string &line, OptInfo &optInfo,
                     std::string &comment) {
    // -k,--key=<value1>    some description 1...
    // -k,--key <value2>    some description 2...
    // -k --key=<value3>    some description 3...
    // -k --key <value4>    some description 4...
    static const std::regex re1(msp d1 ck hsoc d2 ckey heos cv hsp cd);
    // -k,--key             some description 1...
    static const std::regex re1b(msp d1 ck hsoc d2 ckey hsp cd);
    // --key=<val>          some description 1...
    // --key <val>          some description 2...
    static const std::regex re2(msp d2 ckey heos cv hsp cd);
    // -k<val>              some description 1...
    // -k=<val>             some description 2...
    // -k <val>             some description 3...
    static const std::regex re3(msp d1 ck meos cv hsp cd);
    // -k                   some description ...
    static const std::regex re4(msp d1 ck hsp cd);
    // -any str begin with '-' but do not matched....
    static const std::regex re5(msp d1 cd);

    if (IsEmpty(line)){
        return Result::empty;
    }

    optInfo.clear();
    Result result = Result::option;
    std::smatch match;

    auto checkOptInfo = [&](){
        if(!optInfo.argHint.empty() && optInfo.argHint.back() != '>'){
            result = Result::unmatched;
        }
    };

    if (std::regex_match(line, match, re1)) {
        optInfo.shortName = match[1].str();
        optInfo.longName = match[2].str();
        optInfo.argHint = match[3].str();
        optInfo.descList.push_back(match[4].str());
        checkOptInfo();
    } else if (std::regex_match(line, match, re1b)) {
        optInfo.shortName = match[1].str();
        optInfo.longName = match[2].str();
        optInfo.descList.push_back(match[3].str());
    } else if (std::regex_match(line, match, re2)) {
        optInfo.longName = match[1].str();
        optInfo.argHint = match[2].str();
        optInfo.descList.push_back(match[3].str());
        checkOptInfo();
    } else if (std::regex_match(line, match, re3)) {
        optInfo.shortName = match[1].str();
        optInfo.argHint = match[2].str();
        optInfo.descList.push_back(match[3].str());
        checkOptInfo();
    } else if (std::regex_match(line, match, re4)) {
        optInfo.shortName = match[1].str();
        optInfo.descList.push_back(match[2].str());
    } else if (std::regex_match(line, match, re5)) {
        result = Result::unmatched;
    }
    else{
        comment = line;
        result = Result::comment;
    }

    return result;
}

/**
 * @brief Get next item(option or comment) from file stream.
 * 
 * If (return Result::option) Output OptInfo is valid.
 * If (return Result::comment) Output commment is valid.
 * 
 * @param[in] ifs           File stream.
 * @param[in] useIfs        Using file strem or stdin.
 * @param[out] OptInfo      Output OptInfo.
 * @param[out] commment     Output commment.
 * 
 * @return Result           Result decides output.
 */
Result GetNextItem(std::ifstream &ifs, bool useIfs, OptInfo &OptInfo,
                   std::string &commment) {
    std::string line;

    while (std::getline(useIfs ? ifs : std::cin, line)) {
        return LineToOptInfo(line, OptInfo, commment);
    }

    return Result::eof;
}

/**
 * @brief Do some check to the optInfoArray.
 * 
 * @param[in] optInfoArray  OptInfoArray to be checked.
 * 
 * @return 0        Pass.
 * @return other    Fail. 
 */
int ValidateOptInfoArray(const OptInfoArray &optInfoArray){
    if(optInfoArray.empty()) return -1;

    std::set<std::string> tokenSet;
    for(auto& opt: optInfoArray){
        if(!opt.shortName.empty()){
            if(!tokenSet.insert(opt.shortName).second){
                printf("error: option short name = [%s] is duplicate.\n", 
                       opt.shortName.c_str());
                return -1;
            }
        }
    }

    tokenSet.clear();
    for(auto& opt: optInfoArray){
        if(!opt.longName.empty()){
            if(!tokenSet.insert(opt.longName).second){
                printf("error: option long name = [%s] is duplicate.\n", 
                        opt.longName.c_str());
                return -1;
            }
        }
    }

    return 0;
}

/**
 * @brief Convert file to OptInfoArray
 * 
 * @param[in] in                Input file name.
 * @param[out] optInfoArray     Output OptInfoArray.
 * 
 * @return 0                    Pass.
 * @return other                Fail.
 */
int FileToOptInfoArray(const std::string in, OptInfoArray &optInfoArray) {
    std::ifstream ifs;
    if (!in.empty()) {
        ifs.open(in);
        if (!ifs) {
            printf("error: cannot read file = %s\n", in.c_str());
            return -1;
        }
    }

    optInfoArray.clear();
    OptInfo optInfo;
    std::string comment;
    std::size_t lineno = 0;
    Result result;
    while ((result = GetNextItem(ifs, !in.empty(), optInfo, comment)) !=
           Result::eof) {
        ++ lineno;
        if (result == Result::option) {
            optInfoArray.emplace_back(optInfo);
        } else if (result == Result::comment) {
            if(!optInfoArray.empty()){
                optInfoArray.back().descList.push_back(comment);
            }
        } else if(result == Result::unmatched){
            printf("error: found unmatched option at line %lld\n", lineno);
        }
    }

    if(optInfoArray.empty()){
        printf("error: read empty option array.\n");
        return -1;
    }

    return ValidateOptInfoArray(optInfoArray);
}

/**
 * @brief Convert OptInfoArray to stream.
 * 
 * @param[in] OptInfoArray  Input OptInfoArray
 * @param[out] code         Output code.
 * 
 * @return 0                Pass.
 * @return other            Fail.
 */
int OptInfoArrayToCode(const OptInfoArray &OptInfoArray,
                          std::string &code) {
    if(OptInfoArray.empty()) return -1;

    auto searchAndReplace = [](const std::string &source,
                               const std::string &from,
                               const std::string &to) -> std::string {
        if (source.empty()) return {};
        if (from.empty()) return source;

        std::ostringstream oss;
        std::size_t pos = 0;
        std::size_t prev;
        while (true) {
            prev = pos;
            pos = source.find(from, pos);
            if (pos == std::string::npos) { break; }
            oss << source.substr(prev, pos - prev);
            oss << to;
            pos += from.size();
        }
        oss << source.substr(prev);
        return oss.str();
    };

    auto genTimeStamp = [&]() {
        auto tm = std::time(nullptr);
        auto str = std::string(std::asctime(std::localtime(&tm)));
        while (!str.empty() && (str.back() == '\n' || str.back() == '\r')) {
            str.pop_back();
        }
        return str;
    };

    auto itemToStr = [](const std::string &item, bool isShortName = false) {
        std::string str;
        if (!item.empty()) {
            if(isShortName){
                str += "'" + item + "'";
            }else{
                str += "\"" + item + "\"";
            }
        } else {
            str += "nil";
        }
        return str;
    };

    auto cmt = [](const OptInfo &optinfo) {
        std::string str = "// ";
        if (!optinfo.shortName.empty()) {
            str += "-" + optinfo.shortName;
        } else {
            str += "  ";
        }
        if (!optinfo.longName.empty()) str += " --" + optinfo.longName;
        if (!optinfo.argHint.empty()) str += " " + optinfo.argHint;
        return str;
    };

    auto genOptionList = [&]() {
        std::string optionList;
        std::string opt;
        for (auto it = OptInfoArray.begin(); it != OptInfoArray.end();
             ++it) {
            opt = std::string(config::code::OptionListIndention, ' ') + "{";
            opt += itemToStr(it->shortName, true);
            opt += ", ";
            opt += itemToStr(it->longName);
            opt += ", ";
            opt += itemToStr(it->argHint);
            opt += ", ";
            for (auto it2 = it->descList.begin(); it2 != it->descList.end();
                 ++it2) {
                opt += "\"" + *it2;
                if (it2 + 1 != it->descList.end()) { 
                    opt += "<br>";
                }
                opt += "\"";
            }
            opt += "}";
            if (it + 1 != OptInfoArray.end()) opt += ",\n";
            optionList += opt;
        }
        return optionList;
    };

    auto genCaseList = [&]() {
        std::string caseList;
        std::string item;
        int i = 0;
        for (auto it = OptInfoArray.begin(); it != OptInfoArray.end();
             ++it) {
            item = std::string(config::code::CaseListIndention, ' ');
            item += "case ";
            item += std::to_string(i++) + ": " + cmt(*it) + "\n";

            if(!it->argHint.empty()){
                item += std::string(config::code::CaseListIndention, ' ');
                item += "    // " + it->argHint + " = " + "miniopt.optarg()";
                item += "\n";
            }

            item += std::string(config::code::CaseListIndention, ' ');
            item += "break;";
            if(it + 1 != OptInfoArray.end()){
                item += "\n";
            }
            caseList += item;
        }
        return caseList;
    };

    code = config::code::CodeSample;
    code = searchAndReplace(code, config::code::TimeStamp, genTimeStamp());
    code = searchAndReplace(code, config::code::OptionList, genOptionList());
    code = searchAndReplace(code, config::code::CaseList, genCaseList());

    return 0;
}

/**
 * @brief Save code stream to file.
 * 
 * @param[in] code  Code to be saved.
 * @param[in] out   Specify filename to write.
 *                  Stdout is used if out.empty().
 * 
 * @return 0        Pass.
 * @return other    Fail.
 */
int CodeToFile(const std::string &code, const std::string &out) {
    if (code.empty()) return -1;

    if (!out.empty()) {
        std::ofstream ofs(out);
        if (ofs) {
            ofs << code;
            ofs.flush();
            ofs.close();
        } else {
            printf("error: cannot write file = %s\n", out.c_str());
            return -1;
        }
    } else {
        std::cout << code << std::endl;
    }

    return 0;
}

/**
 * @brief Convert input options to miniopt command line parser code file.
 * 
 * @param[in] in    Specify input filename.
 * @param[in] out   Specify output filename.
 * 
 * @return 0        Pass.
 * @return other    Fail.
 */
int GenCode(const std::string &in, const std::string &out) {
    OptInfoArray OptInfoArray;

    int status = FileToOptInfoArray(in, OptInfoArray);
    if (status != 0) return status;

    std::string code;
    status = OptInfoArrayToCode(OptInfoArray, code);
    if (status != 0) return status;

    return CodeToFile(code, out);
}

int ParseArgs(int argc, char *argv[]) {
    std::string in;
    std::string out;
    std::string dir;

    option options[] = {{'o', "out", "<file>", "specify output file name "
                                               "for the generated code."
                        },
                        {'e', "export", "<dir>", "specify directory to export "
                                                 "miniopt library files."},
                        {'h', "help", nil, "show help."},
                        {'v', "version", nil, "show version."}};
    const int optsum = sizeof(options) / sizeof(options[0]);

    if (miniopt.init(argc, (char **)argv, options, optsum) != 0) {
        printf("error: %s\n", miniopt.what());
        return -1;
    }

    int status;
    while ((status = miniopt.getopt()) > 0) {
        int id = miniopt.optind();
        switch (id) {
            case 0:    // -o, --out <file>
                out = miniopt.optarg();
                break;
            case 1:    // -e, --export <dir>
                dir = miniopt.optarg();
                break;
            case 2:    // -h, --help
                std::cout << config::HelpStr << std::endl;
                return 0;
            case 3:    // -v, --version
                std::cout << config::VersionStr << std::endl;
                return 0;
            default:
                in = miniopt.optarg();
                break;
        }
    }

    if (status < 0) printf("error: %s\n", miniopt.what());

    status = GenCode(in, out);
    if(status == 0 && !dir.empty()){
        status = packres::output(dir.c_str());
    }

    return status;
}

int main(int argc, char *argv[]) { 
    return ParseArgs(argc, argv);
}