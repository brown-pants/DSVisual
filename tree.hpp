#ifndef TREE_H
#define TREE_H
#include <list>

/*------------------------二叉树------------------------*/
class NodeItem;

template<typename T>
struct BinTreeNode
{
    //指针域
    BinTreeNode *left;
    BinTreeNode *right;
    NodeItem *p_item;
    //数据域
    T data;
};

enum BinTreeOrder
{
    Preorder,
    Inorder,
    Postorder
};

enum BinTreeLinkDir
{
    LinkLeft,
    LinkRight
};

class BinTree
{
public:
    template<typename T>
    static void DeleteTree(BinTreeNode<T> *root)
    {
        if(root != nullptr)
        {
            DeleteTree(root->left);
            DeleteTree(root->right);
            root->left = nullptr;
            root->right = nullptr;
            delete root;
        }
    }

    template<typename T>
    static void CopyTree(BinTreeNode<T> *src, BinTreeNode<T> *&target)
    {
        if(src == nullptr)
            target = nullptr;
        else {
            target = new BinTreeNode<T>;
            target->data = src->data;
            CopyTree(src->left, target->left);
            CopyTree(src->right, target->right);
        }
    }

    template<typename T>
    static int NodeCount(BinTreeNode<T> *root)
    {
        if(root == nullptr) return 0;
        else return NodeCount(root->left) + NodeCount(root->right) + 1;
    }

    template<typename T>
    static int TreeDepth(BinTreeNode<T> *root)
    {
        if(root == nullptr) return 0;
        else {
            int leftDepth = TreeDepth(root->left);
            int rightDepth = TreeDepth(root->right);
            return leftDepth > rightDepth ? leftDepth + 1 : rightDepth + 1;
        }
    }

    template<typename T>
    static void Traversal(BinTreeOrder order, BinTreeNode<T> *root, std::list<BinTreeNode<T> *> &nodeList)
    {
        if(root == nullptr) return;
        else if(order == Preorder)
        {
            nodeList.push_back(root);
            Traversal(order, root->left, nodeList);
            Traversal(order, root->right, nodeList);
        }
        else if(order == Inorder)
        {
            Traversal(order, root->left, nodeList);
            nodeList.push_back(root);
            Traversal(order, root->right, nodeList);
        }
        else if(order == Postorder)
        {
            Traversal(order, root->left, nodeList);
            Traversal(order, root->right, nodeList);
            nodeList.push_back(root);
        }
    }
};

class BinSortTree
{
public:
    //二叉排序树插入数据 返回父结点 参数(dir == -1 为左结点 | dir == 1 为右结点)
    template<typename T>
    static BinTreeNode<T> *InsertNode(BinTreeNode<T> *root, T num, BinTreeLinkDir &dir)
    {
        if(root == nullptr) return nullptr;
        if(num < root->data)
        {
            if(root->left == nullptr)
            {
                dir = LinkLeft;
                return root;
            }
            else
            {
                return InsertNode(root->left, num, dir);
            }
        }
        else
        {
            if(root->right == nullptr)
            {
                dir = LinkRight;
                return root;
            }
            else
            {
                return InsertNode(root->right, num, dir);
            }
        }
    }

    //二叉排序树查找数据 参数(nodeList 存放遍历的结点)
    template<typename T>
    static void FindNode(BinTreeNode<T> *root, T num, std::list<BinTreeNode<T> *> &nodeList)
    {
        if(root == nullptr) return;
        nodeList.push_back(root);
        if(root->data == num) return;
        else if(num < root->data) FindNode(root->left, num, nodeList);
        else FindNode(root->right, num, nodeList);
    }
};

#endif // TREE_H
