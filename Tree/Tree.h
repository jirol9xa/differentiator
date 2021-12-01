#ifndef TREE_H
    #define THEE_H


    typedef char *type_t;

    
    union Value
    {
        double number;
        char   symbol;
        char  *func;
    };

    struct Node 
    {
        Value value;
        int   node_type;
        Node *left_child;
        Node *right_child;
        Node *parent;
    };
    struct Tree
    {
        Node* root;
        int size;
        int status;
    };
    struct Path
    {
        type_t *path;
        int    size;
        int    iterator;
    };
    


    enum StatAndErrs
    {
        EMPTY_TREE      = 1 << 0,
        UPDATED_TREE    = 1 << 1,
        DESTRUCTED_TREE = 1 << 2,
    };

    /*!
        \brief  Функция создания узла
        \param  [Node *]parent Указатель на родителя
        \param  [Node *]new_node Указатель на
                создаваемый узел
        \param  [int]is_left 1, если узел является
                левым сыном, 0 в противном случае
        \return Указатель на новый узел в случае успеха, 
                -1 в противном случае
    */
    Node *nodeCtor (Node *parent, Node *new_node, int is_left);
    /*!
        \brief  Функция создания дерева
        \param  [Tree *]tree Указатель на дерево
        \return 0 в случае успеха, -1 в противном 
                случае
    */
    int   treeCtor (Tree *tree);
    /*!
        \brief  Фукнция дампа дерева
        \param  [Tree *]tree Указатель на дерево
        \return 0 в случае успеха, -1 в противном
                случае
    */
    void  treeDump (Tree *tree);
    /*!
        \brief  Функия распечатки узла
        \param  [Node *]node Указатель на узел
        \return 0 в случае успеха, -1 в противном
                случае
    */
    void  printNode(Node *node);
    /*!
        \brief  Функция уничтожения дерева
        \param  [Tree *]tree Указатель на дерево
        \return 0 вслучае успеха, -1 в противном
                случае
    */
    int   treeDtor (Tree *tree);
    /*!
        \brief  Функия уничтожения узла
        \param  [Node *]node Указатель на узел
        \return 0 в случае успеха, -1 в противном
                случае
    */
    int   nodeDtor (Node *node);
    /*!
        \brief  Функция сохранения дерева
        \param  [Tree *]tree Указатель на дерево
        \return 0 в случае успеха, -1 в противном
                случае
    */
    int   saveTree(Tree *tree);
    /*!
        \brief  Функция сохранения узла
        \param  [Node *]tree Указатель на узел
        \return 0 в случае успеха, -1 в противном
                случае
    */
    int saveNode(Node *node);
    /*!
        \brief  Функия поиска элемента
        \param  [Node *]node Указатель на узел
        \param  [type_t]elem Значение искомого
                элемента
        \return Указатель на новый узел, -1 в противном
                случае
    */
    Node *searchElem(Node *node, type_t elem);

#endif