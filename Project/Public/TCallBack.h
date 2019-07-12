#ifndef __TCallBack_h__
#define __TCallBack_h__
#include "Tools.h"
#include <list>
#include <unordered_map>
namespace tlib
{
    template<typename FuncType>
    struct CallInfo
    {
        FuncType _func;
        char _trace[256];
        CallInfo(FuncType func, const char *trace)
        {
            _func = func;
            if (trace != nullptr)
                tools::SafeStrcpy(_trace, sizeof(_trace), trace, strlen(trace));
        }
        CallInfo(const CallInfo &right)
        {
            tools::SafeMemcpy(this, sizeof(CallInfo), &right, sizeof(CallInfo));
        }

        CallInfo & operator = (const CallInfo &right)
        {
            if (this != &right)
            {
                tools::SafeMemcpy(this, sizeof(CallInfo), &right, sizeof(CallInfo));
            }
            return *this;
        }
        bool operator == (const CallInfo &right){ return _func == right._func; };
    };
    template< typename T, typename FuncType, typename... Args>
    class TCallBack
    {
        typedef CallInfo<FuncType> CallUnit;
        typedef std::list<CallUnit> CallLists;
        typedef std::unordered_map<T, CallLists> CallMap;
    public:
        TCallBack(){};
        ~TCallBack(){};

        bool RegisterCallBack(const T &id, const FuncType callFunc, const char *debug)
        {
            CallUnit unit(callFunc, debug);
            auto iter = _callMap.find(id);
            if (iter == _callMap.end())
            {
                auto ret = _callMap.insert(std::make_pair(id, CallLists()));
                ASSERT(ret.second, "error");
                iter = ret.first;
            }
#ifdef _DEBUG
            auto findIter = std::find(iter->second.begin(), iter->second.end(), unit);
            if (findIter != iter->second.end())
            {
                ASSERT(false, "id has exist");
                return false;
            }
#endif
            iter->second.push_back(unit);
            return true;
        }

        bool UnRegisterCallBack(const T &id, const FuncType callFunc)
        {
            CallUnit unit(callFunc, nullptr);
            auto iter = _callMap.find(id);
            if (iter != _callMap.end())
            {
                auto findIter = std::find(iter->second.begin(), iter->second.end(), unit);
                if (findIter != iter->second.end())
                {
                    iter->second.erase(findIter);
                    return true;
                }
            }
            ASSERT(false, "don't find call func");
            return false;
        }

        bool ClearCallBack(const T &id)
        {
            auto iter = _callMap.find(id);
            if (iter != _callMap.end())
                iter->second.clear();

			return true;
        }

        bool CallWithCondition(const T &id, bool condition, Args ...args)
        {
            auto iter = _callMap.find(id);
            if (iter == _callMap.end())
                return true;
            for (auto unit : iter->second)
            {
                if (condition == unit._func(args...))
                    return condition;
            }

            return !condition;
        }

        void Call(const T &id, Args ...args)
        {
            auto iter = _callMap.find(id);
            if (iter != _callMap.end())
            {
                for ( auto unit : iter->second)
                    unit._func(args...);
            }
        }
    protected:
    private:
        CallMap _callMap;
    };
}
#endif