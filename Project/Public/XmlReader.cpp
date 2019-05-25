#include "XmlReader.h"
#include <unordered_map>
#include "TString.h"
#include <vector>
using namespace tinyxml2;

class XmlObject : public IXmlObject
{
	typedef std::unordered_map< s64, XmlObject *> XmlObjects;
	typedef tlib::TString<MAX_PATH> ElementName;
	typedef std::vector<XmlObject *>	V_XMLOBJ;
public:
	XmlObject(XMLElement *element, const char *name) : _element(element) {
		if (nullptr != name)
			_name << name;
	};

	virtual ~XmlObject() {
		_element = nullptr;
		for ( auto iter : _childes)
			SAFE_DELETE(iter);
		_childes.clear();
	};

	virtual s8  GetAttribute_S8(const char *name)
	{
		return (s8)_element->IntAttribute(name);
	}
	virtual s16 GetAttribute_S16(const char *name)
	{
		
		return (s16)_element->IntAttribute(name);
	}
	virtual s32 GetAttribute_S32(const char *name)
	{
		return _element->IntAttribute(name);
	}
	virtual s64 GetAttribute_S64(const char *name)
	{
		return atoll(_element->Attribute(name));
	}
	virtual const char * GetAttribute_Str(const char *name)
	{
		return _element->Attribute(name);
	}
	virtual const char * GetContent()
	{
		return _element->GetText();
	}

    virtual bool GetAttribute_Bool(const char *name)
    {
        const char *tmp = _element->Attribute(name);
        return strcmp(tmp, "true") == 0;
    }

    virtual bool HasAttribute(const char *name)
    {
        const char *tmp = _element->Attribute(name);
        return tmp != nullptr;
    }

	virtual  IXmlObject * GetFirstChrild(const char *name)
	{
		XMLElement *child = _element->FirstChildElement(name);
		if (child)
		{
			XmlObject *object = new XmlObject(child, name);
			_childes.push_back(object);
			return object;
		}
		return nullptr;
	}
	virtual  IXmlObject * GetNextSibling()
	{
			if (_name.Length() > 0)
			{
				XMLElement *sibling = _element->NextSiblingElement(_name.GetString());
				if (sibling)
				{
					XmlObject *object = new XmlObject(sibling, _name.GetString());
					_childes.push_back(object);
					return object;
				}
			}
	
		return nullptr;
	}

	virtual const char *  GetXmlObjectName() const
	{
		return _name.GetString();
	}

protected:
private:
	XMLElement *_element;
	ElementName _name;
	V_XMLOBJ	_childes;
};

IXmlObject * XmlReader::Root()
{
	if (_obj != nullptr)
		return _obj;

	_obj = new XmlObject(_doc.RootElement(), nullptr);

	return _obj;
}