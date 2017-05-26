#ifndef __Search_Tree_h__
#define __Search_Tree_h__
#include "Vector.h"
#include <vector>
class SearchTree
{
    enum 
    {
        BRANCH_MAX_COUNT = 16,
    };

    struct Node;
    struct Branch 
    {
        MBRect3D    mbr;
        Node        *child;
        s64         identity;
    };
    struct Node
    {
        u8 level;
        u8 count;
        Branch branch[BRANCH_MAX_COUNT];
    };
    typedef std::vector<s64>    IdentityList;
public:
    SearchTree() :_root(nullptr)
    {
        ;
    }

    void Insert(s64 identity, Vector3D &pos);
    void Remove(s64 identity, Vector3D &pos);
    //void Search(MBRect3D &searchRect, std:);
protected:
private:
    inline bool IsLeaf(const Node *node){ return node->level == 0; };
private:
    Node    *_root;
};
#endif