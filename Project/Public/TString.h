#ifndef __X_TString_h__
#define __X_TString_h__
#include "MultiSys.h"
//#include "Tools.h"
namespace tlib{
    template< s16 _size = 64>
    class TString
    {
    public:
        ~TString(){};
        TString() :_len(0){ _buffer[0] = 0; };
        TString(const char *str)
        {
            _len = (s16)strlen(str);
            _len = _len > _size ? _size : _len;
            memcpy(_buffer, str, _len);
            _buffer[_len] = 0;
        }

        void Assign(const char *str, const s16 len)
        {
            len = len > _size ? _size : len;
            _len = strlen(str);
            _len = len > _len ? _len : len;
            memcpy(_buffer, str, _len);
            _buffer[_len] = 0;
        }

        void Assign(const char *str)
        {
            _len = strlen(str);
            if (_len > _size)
                _len = _size;
            memcpy(_buffer, str, _len);
            _buffer[_len] = 0;
        }

        inline const char * GetString() const { return _buffer; };
        inline void Clear(){ _len = 0; _buffer[0] = 0; };
        inline s32 Length()const{ return _len; };

        bool operator == (const char *str) const { return 0 == strcmp(_buffer, str); };
        bool operator != (const char *str) const { return 0 != strcmp(_buffer, str); };
        bool operator <  (const char *str) const { return strcmp(_buffer, str) < 0; };
        TString & operator = (const char *str)
        {
            _len = (s16)strlen(str);
            _len = _len > _size ? _size : _len;
            memcpy(_buffer, str, _len);
            _buffer[_len] = 0;
            return *this;
        }
        TString & operator = (float value)
        {
            SafeSprintf(_buffer, sizeof(_buffer), "%0.2f", value);
            _len = strlen(_buffer);
            return *this;
        }
        TString & operator = (s64 value)
        {
            SafeSprintf(_buffer, sizeof(_buffer), "%ld", value);
            _len = strlen(_buffer);
            return *this;
        }
        TString & operator = (s32 value)
        {
            SafeSprintf(_buffer, sizeof(_buffer), "%d", value);
            _len = strlen(_buffer);
            return *this;
        }
        TString & operator = (char val)
        {
            _buffer[0] = val;
            _len = 1;
            _buffer[_len] = 0;
            return *this;
        }
        TString & operator = (const TString &str)
        {
            _len = str._len > _size ? _size : str._len;
            memcpy(_buffer, str._buffer, _len);
            _buffer[_len] = 0;
            return *this;
        }

        TString & operator << (const char *val)
        {
            SafeSprintf(_buffer + _len, sizeof(_buffer)-_len, "%s", val);
            _len = strlen(_buffer);
            return *this;
        }

        TString & operator << (const TString &val)
        {
            s32 len = _size - _len;
            if (len < val._len)
                len = val._len;
            memcpy(_buffer + _len, val._buffer, len);
            _len += len;
            _buffer[_len] = 0;
            return *this;
        }

        TString & operator << (s32 val)
        {
            SafeSprintf(_buffer + _len, sizeof(_buffer)-_len, "%d", val);
            _len = strlen(_buffer);
            return *this;
        }

        TString & operator << (s64 val)
        {
            SafeSprintf(_buffer + _len, sizeof(_buffer)-_len, "%ld", val);
            _len = strlen(_buffer);
            return *this;
        }

        TString & operator << (float val)
        {
            SafeSprintf(_buffer + _len, sizeof(_buffer)-_len, "%0.2f", val);
            _len = strlen(_buffer);
            return *this;
        }

        TString & operator << (char val)
        {
            if (_len < _size)
            {
                _buffer[_len] = val;
                _buffer[++_len] = 0;
            }
            return *this;
        }

		TString & AppendFormat(const char *format, ...)
		{
			va_list argList;
			va_start(argList, format);
			vsnprintf(_buffer + _len, _size - _len, format, argList);
			va_end(argList);
			_len = strlen(_buffer);

			return *this;
		}

        operator size_t() const
        {
            size_t hash = 0;
            for (s32 i = 0; i < _len; i++)
            {
                hash = hash * 33 + (size_t)_buffer[i];
            }
            return hash;
        }
    protected:
    private:
        s16  _len;
        char _buffer[_size + 1];
    };
}
#endif
