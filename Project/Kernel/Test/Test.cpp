#include "Test.h"
#include "XmlReader.h"
using namespace tinyxml2;
void Test::Exec()
{
	char path[MAX_PATH];
	SafeSprintf(path, sizeof(path), "%s", "./core/server.xml");
	//LoadConfigFile(path);
	XmlReaderFun(path);
}

bool Test::LoadConfigFile(const char *path)
{
	tinyxml2::XMLDocument doc;
	if ( tinyxml2::XML_SUCCESS != doc.LoadFile(path))
	{
		ECHO("don't find %s file", path);
		return false;
	}
	XMLElement * root = doc.RootElement();
	XMLElement *server = root->FirstChildElement("server");
	XMLElement *serverSub = server->NextSiblingElement("server");
	XMLElement *serverSub2 = serverSub->NextSiblingElement("server");
	XMLElement *serverSub3 = server->NextSiblingElement("server");


	ECHO("temp is null");
	return true;
}

bool Test::XmlReaderFun( const char *path)
{
	XmlReader reader;
	if (!reader.LoadFile(path))
	{
		ECHO("don,t find %s file", path);
		return false;
	}
	IXmlObject * root = (IXmlObject*)reader.Root();
	IXmlObject *serverNode = root->GetFirstChrild("server");
	IXmlObject *player =	 root->GetFirstChrild("player");
	while (serverNode)
	{
		s32 id = serverNode->GetAttribute_S32("id");
		ECHO("Server, ID = %d", id);
		IXmlObject *node = serverNode->GetFirstChrild("node");
		if (node)
		{
			s32 nodeId = node->GetAttribute_S32("id");
			ECHO("Node, ID = %d", nodeId);
		}

		serverNode = serverNode->GetNextSibling();
	}
	while (player)
	{
		s32 age = player->GetAttribute_S32("age");
		ECHO("Player Age = %d", age);
		player = player->GetNextSibling();
	}
	return true;
}