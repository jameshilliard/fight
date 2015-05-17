#include "json.h"
bool JosnEnCoder(map<std::string,std::string> &mCmdStr,std::string &sJosnStr)
{
	Json::Value root;
	Json::FastWriter fast_writer;
	map<std::string,std::string>::iterator it = mCmdStr.begin();
	for (it; it != mCmdStr.end(); it ++)
	{
		root[it->first] = Json::Value(it->second);
	}
	sJosnStr = fast_writer.write(root).c_str();
	return true;
}

bool JosnDeCoder(std::string sJosnStr,map<std::string ,std::string > &mCmdStr)
{
	Json::Reader reader;
	Json::Value json_object;
	Json::Value json_value;
	if (!reader.parse(sJosnStr.c_str(), json_object))
	{
		return false;
	}
	std::string sKey;
	Json::Value::iterator it = json_object.begin();
	for (it; it != json_object.end(); it ++)
	{
		sKey = it.memberName();
		if (sKey=="")
			continue;
		json_value = json_object[sKey];
		if(json_value.isString())
		{
			mCmdStr[sKey] = json_value.asCString();
		}
		else if(json_value.isInt())
		{
			char TmpIntStr[20];
			sprintf(TmpIntStr,"%d",json_value.asInt());
			mCmdStr[sKey] = TmpIntStr;
		}
	}
	return true;
}

