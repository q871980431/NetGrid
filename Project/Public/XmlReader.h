#ifndef __XmlReader_h__
#define __XmlReader_h__
#include "MultiSys.h"
#include "tinyxml2.h"
class IXmlObject
{
public:
	virtual~IXmlObject() {};

	virtual s8  GetAttribute_S8(const char *name) = 0;
	virtual s16 GetAttribute_S16(const char *name) = 0;
	virtual s32 GetAttribute_S32(const char *name) = 0;
	virtual s64 GetAttribute_S64(const char *name) = 0;
    virtual bool GetAttribute_Bool(const char *name) = 0;
    virtual bool HasAttribute(const char *name) = 0;
	virtual const char * GetAttribute_Str(const char *name) = 0;
	virtual const char * GetContent() = 0;
	virtual  IXmlObject * GetFirstChrild(const char *name) = 0;
	virtual  IXmlObject * GetNextSibling() = 0;
	virtual const char * GetXmlObjectName() const = 0;
};

class XmlReader
{
public:
	XmlReader() :_obj(nullptr){};
	~XmlReader() {
		SAFE_DELETE(_obj);
	};

	bool LoadFile(const char *path) { return tinyxml2::XML_SUCCESS ==_doc.LoadFile(path)? true : false; };

	IXmlObject * Root();

protected:
private:
	tinyxml2::XMLDocument _doc;
	IXmlObject	*_obj;
};

#endif