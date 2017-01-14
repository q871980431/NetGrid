#ifndef __Test_h__
#define __Test_h__
#include "Tools.h"
#include "TString.h"
#include <unordered_map>
#include "tinyxml2.h"

typedef tlib::TString<> PlayerName;
//typedef std::unordered_map<PlayerName, s32, tools::TemplateHash<PlayerName>, tools::TemplateEquial<PlayerName> > NameScore;
//typedef std::unordered_map<char *, s32, tools::TemplateHash<char *>, tools::TemplateEquial<char *> > NameScore1;
class Test
{
public:
	Test() {};
	~Test() {};

	void Exec();

protected:
private:
	bool LoadConfigFile( const char *path );
	bool XmlReaderFun(const char *path);
private:
	//NameScore _scores;
};
#endif