#include <iostream>
#include <vector>
#include <regex>

using namespace std;

using logFunc = std::function<void(std::stringstream &, const string& input )>;


static void aa(std::stringstream& ss, const string& input ){
     ss << "aa";
};

static void bb(std::stringstream& ss, const string& input ){
    ss << "bb";
}

static void cc(std::stringstream& ss, const string& input ){
    ss << "cc";
}

std::map<std::string, logFunc> logFuncMap_({
        {"%a", aa},
        {"%b", bb},
        {"%c", cc},
});

vector<logFunc> logFunctions_;

logFunc newLogFunction(const std::string &placeholder) {
    auto iter = logFuncMap_.find(placeholder);
    if (iter != logFuncMap_.end())
    {
        return iter->second;
    }
    return [placeholder](std::stringstream &stream,
                         const std::string &) {
        stream << placeholder;
    };
}

void createLogFunctions(std::string &format) {
    logFunctions_.clear();

    std::string rawString;
    while (!format.empty()) {
        auto pos = format.find('%');
        if (pos != std::string::npos) {
            rawString += format.substr(0, pos);

            format = format.substr(pos);
            std::regex e{"^\\%[a-zA-Z0-9\\-_]+"};
            std::smatch m;
            if (std::regex_search(format, m, e)) {
                if (!rawString.empty()) {
                    // '%' 이전까지 출력되는 함수
                    logFunctions_.emplace_back(
                            [rawString](std::stringstream &stream,
                                        const string &) {
                                stream << rawString;
                            });
                    rawString.clear();
                }
                auto placeholder = m[0];
                logFunctions_.emplace_back(newLogFunction(placeholder));
                format = m.suffix().str();
            } else {
                rawString += '%';
                format = format.substr(1);
            }
        } else {
            rawString += format;
            logFunctions_.emplace_back(
                    [rawString](std::stringstream &stream,
                                const string &) {
                        stream << rawString;
                    });
            rawString.clear();
            break;
        }

        if (!rawString.empty()) {
            // 최종적으로 rawString에 남겨진 문자들을 stram에 넣음
            logFunctions_.emplace_back([rawString](std::stringstream &stream,
                                                   const std::string &) {
                stream << rawString;
            });
        }
    }
}

int main() {
    stringstream ss;
    string format = "{ %a %b }";
    createLogFunctions(format);

    for(auto &func : logFunctions_){
        func(ss, "");
    }

    cout << ss.str();

    return 0;
}
