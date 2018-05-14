/*
 * File:	ISearch.h
 * Author:	xuping
 * 
 * Created On 2018/1/2 20:32:57
 */

#ifndef  __ISearch_h__
#define __ISearch_h__
#include "IModule.h"
#include <functional>

class ISearch : public IModule
{
public:
	enum 
	{
		SEARCH_MODEL_MIN = 0,
		SEARCH_MODEL_PARTNER = 1,		//��Ѱ���
		SEARCH_MODEL_PAIR_GAME = 2,		//��Ѱ˫��
		SEARCH_MODEL_MAX,
	};

	enum 
	{
		SEARCH_TRANSF_MIN = 0,
		SEARCH_TRANSF_LINEAR = 1,				//���Ա仯
		SEARCH_TRANSF_LINEAR_ACCELERATION = 2,	//���Լ���
		SEARCH_TRANSF_MAX,
	};

	const static s32 SEARCH_INTERVAL_MIN = 100;		//��Ѱ��ͼ��ʱ��
	const static s32 TRANSF_INTERVAL_MIN = 1000;	//�任��ͼ��ʱ��

	struct SetInfo
	{
		s32 num;
		s64 *id;
	};

	typedef std::function<void(s32 count, const SetInfo *setInfo)> SearchCallBack;

	struct  SearchConfig
	{
		s32 min;			//��ͷ�ֵ
		s32 max;			//����ֵ
		s32 cellVal;		//��Ԫֵ

		s8 num;				//��������
		s8 numMin;			//�������������>0 �����reduce ����ÿ�ֽ��� �ɽ��뼯������
		float reduce;		//����ֵ
		s8 model;			//��Ѱģʽ

		s32 searchInterval; //��Ѱ���	ms
		s32 transfDelay;	//�任ʱ��	ms
		s32 transfRadio;	//�任�뾶	2
		s32 transfModel;    //�任ģ��
		s32 transfMax;		//�任������
		SearchCallBack calBack; //�ص�����
	};


public:
    virtual ~ISearch(){};
    
	virtual s64 RegisterSearchChannel(const SearchConfig *config) = 0;
	virtual bool DelSearchChannel(s64 channelId) = 0;
	virtual bool EnterIndividualSearchChannel(s64 channelId, s64 personId, s32 val) = 0;
	virtual bool EnterTeamSearchChannel(s64 channelId, s64 teamId, s32 val, s32 num, s64 *memberId) = 0;
	virtual bool LeaveIndividualSearchChannel(s64 channelId, s64 personId) = 0;
	virtual bool LeaveTeamSearchChannel(s64 channelId, s64 teamId) = 0;
};
#endif