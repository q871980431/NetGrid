/*
 * File:	ObjectMember.cpp
 * Author:	xuping
 * 
 * Created On 2019/3/27 17:12:44
 */

#include "ObjectMember.h"
#include "ObjectMgr.h"
#define MEMBER_EXPORT
#include "MemberDef.h"

const IMember * Root::id = nullptr;
const IMember * Logic::Root::id = nullptr;
const IMember * Logic::Player::name = nullptr;
const IMember * Logic::Player::lvl = nullptr;
const IMember * Logic::Player::bag = nullptr;
const IMember * Logic::Player::skill = nullptr;
const IMember * Logic::Player::notice = nullptr;
const IMember * Logic::Player::Bag::place = nullptr;
const IMember * Logic::Player::Skill::skillId = nullptr;
const IMember * Logic::Player::Notice::content = nullptr;
const IMember * Logic::Wing::wingid = nullptr;
const IMember * Logic::Wing::order = nullptr;

void ObjectMember::Init()
{
    Root::id = ObjectMgr::GetMemberProperty("root::id");
    Logic::Root::id = ObjectMgr::GetMemberProperty("logic::root::id");
    Logic::Player::name = ObjectMgr::GetMemberProperty("logic::player::name");
    Logic::Player::lvl = ObjectMgr::GetMemberProperty("logic::player::lvl");
    Logic::Player::bag = ObjectMgr::GetMemberProperty("logic::player::bag");
    Logic::Player::skill = ObjectMgr::GetMemberProperty("logic::player::skill");
    Logic::Player::notice = ObjectMgr::GetMemberProperty("logic::player::notice");
    Logic::Player::Bag::place = ObjectMgr::GetMemberProperty("logic::player::bag::place");
    Logic::Player::Skill::skillId = ObjectMgr::GetMemberProperty("logic::player::skill::skillId");
    Logic::Player::Notice::content = ObjectMgr::GetMemberProperty("logic::player::notice::content");
    Logic::Wing::wingid = ObjectMgr::GetMemberProperty("logic::wing::wingid");
    Logic::Wing::order = ObjectMgr::GetMemberProperty("logic::wing::order");
}