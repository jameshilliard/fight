#ifndef JSON_JSON_H_INCLUDED
#define JSON_JSON_H_INCLUDED
#include <map>
#include <string>
# include "autolink.h"
# include "value.h"
# include "reader.h"
# include "writer.h"
# include "features.h"
using namespace std;
bool JosnEnCoder(std::map<std::string,std::string> &mCmdStr,std::string &sJosnStr);
bool JosnDeCoder(std::string sJosnStr,std::map<std::string ,std::string > &mCmdStr);
#endif // JSON_JSON_H_INCLUDED
