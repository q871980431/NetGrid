/*
 * File:	ObjectMember.cpp
 * Author:	xuping
 * 
 * Created On 2017/12/16 22:21:28
 */

#include "ObjectMember.h"
#include "ObjectMgr.h"
#define MEMBER_EXPORT
#include "MemberDef.h"

const IMember * Root::id = nullptr;
const IMember * Logic::Root::id = nullptr;
const IMember * Logic::Wing::wingid = nullptr;
const IMember * Logic::Wing::order = nullptr;
const IMember * Logic::Player::lvl = nullptr;
const IMember * Logic::Player::bag = nullptr;
const IMember * Logic::Player::wingbag = nullptr;
const IMember * Logic::Player::Bag::place = nullptr;
const IMember * Logic::Player::Bag::randattr = nullptr;
const IMember * Logic::Player::Bag::Randattr::id = nullptr;
const IMember * Logic::Player::Bag::Randattr::type = nullptr;
const IMember * Logic::Player::Bag::Randattr::value = nullptr;
const IMember * Logic::Player::Wingbag::wing = nullptr;
const IMember * Logic::Player::Wingbag::Wing::wingid = nullptr;
const IMember * Logic::Player::Wingbag::Wing::order = nullptr;

void ObjectMember::Init()
{
    Root::id = ObjectMgr::GetMemberProperty("root::id");
    Logic::Root::id = ObjectMgr::GetMemberProperty("logic::root::id");
    Logic::Wing::wingid = ObjectMgr::GetMemberProperty("logic::wing::wingid");
    Logic::Wing::order = ObjectMgr::GetMemberProperty("logic::wing::order");
    Logic::Player::lvl = ObjectMgr::GetMemberProperty("logic::player::lvl");
    Logic::Player::bag = ObjectMgr::GetMemberProperty("logic::player::bag");
    Logic::Player::wingbag = ObjectMgr::GetMemberProperty("logic::player::wingbag");
    Logic::Player::Bag::place = ObjectMgr::GetMemberProperty("logic::player::bag::place");
    Logic::Player::Bag::randattr = ObjectMgr::GetMemberProperty("logic::player::bag::randattr");
    Logic::Player::Bag::Randattr::id = ObjectMgr::GetMemberProperty("logic::player::bag::randattr::id");
    Logic::Player::Bag::Randattr::type = ObjectMgr::GetMemberProperty("logic::player::bag::randattr::type");
    Logic::Player::Bag::Randattr::value = ObjectMgr::GetMemberProperty("logic::player::bag::randattr::value");
    Logic::Player::Wingbag::wing = ObjectMgr::GetMemberProperty("logic::player::wingbag::wing");
    Logic::Player::Wingbag::Wing::wingid = ObjectMgr::GetMemberProperty("logic::player::wingbag::wing::wingid");
    Logic::Player::Wingbag::Wing::order = ObjectMgr::GetMemberProperty("logic::player::wingbag::wing::order");
}