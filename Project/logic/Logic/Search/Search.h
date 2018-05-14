/*
 * File:	Search.h
 * Author:	xuping
 * 
 * Created On 2018/1/2 20:32:57
 */

#ifndef __Search_h__
#define __Search_h__
#include "ISearch.h"
#include <unordered_map>

class SearchUnit;
class Search : public ISearch
{
	typedef std::unordered_map<s64, SearchUnit *> SearchMap;
public:
    virtual ~Search(){};

    virtual bool Initialize(IKernel *kernel);
    virtual bool Launched(IKernel *kernel);
    virtual bool Destroy(IKernel *kernel);

	virtual s64 RegisterSearchChannel(const SearchConfig *config);
	virtual bool DelSearchChannel(s64 channelId);
	virtual bool EnterIndividualSearchChannel(s64 channelId, s64 personId, s32 val);
	virtual bool EnterTeamSearchChannel(s64 channelId, s64 teamId, s32 val, s32 num, s64 *memberId);
	virtual bool LeaveIndividualSearchChannel(s64 channelId, s64 personId);
	virtual bool LeaveTeamSearchChannel(s64 channelId, s64 teamId);
private:

protected:
private:
    static Search     * s_self;
    static IKernel    * s_kernel;
	static s64		    s_guid;
	static SearchMap	s_serachMap;
};
#endif