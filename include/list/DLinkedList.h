/*
 * File:  DLinkedList.h
 */

#ifndef DLINKEDLIST_H
#define DLINKEDLIST_H

#include "list/IList.h"

#include <sstream>
#include <iostream>
#include <type_traits>
using namespace std;

template <class T>
class DLinkedList : public IList<T>
{
public:
    class Node;        // Forward declaration
    class Iterator;    // Forward declaration
    class BWDIterator; // Forward declaration

protected:
    Node *head; // this node does not contain user's data
    Node *tail; // this node does not contain user's data
    int count;
    bool (*itemEqual)(T &lhs, T &rhs);        // function pointer: test if two items (type: T&) are equal or not
    void (*deleteUserData)(DLinkedList<T> *); // function pointer: be called to remove items (if they are pointer type)

public:
    DLinkedList(
        void (*deleteUserData)(DLinkedList<T> *) = 0,
        bool (*itemEqual)(T &, T &) = 0);
    DLinkedList(const DLinkedList<T> &list);
    DLinkedList<T> &operator=(const DLinkedList<T> &list);
    ~DLinkedList();

    // Inherit from IList: BEGIN
    void add(T e);
    void add(int index, T e);
    T removeAt(int index);
    bool removeItem(T item, void (*removeItemData)(T) = 0);
    bool empty();
    int size();
    void clear();
    T &get(int index);
    int indexOf(T item);
    bool contains(T item);
    string toString(string (*item2str)(T &) = 0);
    // Inherit from IList: END

    void println(string (*item2str)(T &) = 0)
    {
        cout << toString(item2str) << endl;
    }
    void setDeleteUserDataPtr(void (*deleteUserData)(DLinkedList<T> *) = 0)
    {
        this->deleteUserData = deleteUserData;
    }
    void setItemEqual(bool (*func)(T&, T&)) {
        this->itemEqual = func;
    }
    bool contains(T array[], int size)
    {
        int idx = 0;
        for (DLinkedList<T>::Iterator it = begin(); it != end(); it++)
        {
            if (!equals(*it, array[idx++], this->itemEqual))
                return false;
        }
        return true;
    }

    /*
     * free(DLinkedList<T> *list):
     *  + to remove user's data (type T, must be a pointer type, e.g.: int*, Point*)
     *  + if users want a DLinkedList removing their data,
     *      he/she must pass "free" to constructor of DLinkedList
     *      Example:
     *      DLinkedList<T> list(&DLinkedList<T>::free);
     */
    static void free(DLinkedList<T> *list)
    {
        typename DLinkedList<T>::Iterator it = list->begin();
        while (it != list->end())
        {
            delete *it;
            it++;
        }
    }

    /* begin, end and Iterator helps user to traverse a list forwardly
     * Example: assume "list" is object of DLinkedList

     DLinkedList<char>::Iterator it;
     for(it = list.begin(); it != list.end(); it++){
            char item = *it;
            std::cout << item; //print the item
     }
     */
    Iterator begin()
    {
        return Iterator(this, true);
    }
    Iterator end()
    {
        return Iterator(this, false);
    }

    /* last, beforeFirst and BWDIterator helps user to traverse a list backwardly
     * Example: assume "list" is object of DLinkedList

     DLinkedList<char>::BWDIterator it;
     for(it = list.last(); it != list.beforeFirst(); it--){
            char item = *it;
            std::cout << item; //print the item
     }
     */
    BWDIterator bbegin()
    {
        return BWDIterator(this, true);
    }
    BWDIterator bend()
    {
        return BWDIterator(this, false);
    }

protected:
    static bool equals(T &lhs, T &rhs, bool (*itemEqual)(T &, T &))
    {
        if (itemEqual == 0)
            return lhs == rhs;
        else
            return itemEqual(lhs, rhs);
    }
    void copyFrom(const DLinkedList<T> &list);
    void removeInternalData();
    Node* getPreviousNodeOf(int index);

    //////////////////////////////////////////////////////////////////////
    ////////////////////////  INNER CLASSES DEFNITION ////////////////////
    //////////////////////////////////////////////////////////////////////
public:
    class Node
    {
    public:
        T data;
        Node *next;
        Node *prev;
        friend class DLinkedList<T>;

    public:
        Node(Node *next = 0, Node *prev = 0)
        {
            this->next = next;
            this->prev = prev;
        }
        Node(T data, Node *next = 0, Node *prev = 0)
        {
            this->data = data;
            this->next = next;
            this->prev = prev;
        }
    };

    //////////////////////////////////////////////////////////////////////
    class Iterator
    {
    private:
        DLinkedList<T> *pList;
        Node *pNode;

    public:
        Iterator(DLinkedList<T> *pList = 0, bool begin = true)
        {
            if (begin)
            {
                if (pList != 0)
                    this->pNode = pList->head->next;
                else
                    pNode = 0;
            }
            else
            {
                if (pList != 0)
                    this->pNode = pList->tail;
                else
                    pNode = 0;
            }
            this->pList = pList;
        }

        Iterator &operator=(const Iterator &iterator)
        {
            this->pNode = iterator.pNode;
            this->pList = iterator.pList;
            return *this;
        }
        void remove(void (*removeItemData)(T) = 0)
        {
            pNode->prev->next = pNode->next;
            pNode->next->prev = pNode->prev;
            Node *pNext = pNode->prev; // MUST prev, so iterator++ will go to end
            if (removeItemData != 0)
                removeItemData(pNode->data);
            delete pNode;
            pNode = pNext;
            pList->count -= 1;
        }

        T &operator*()
        {
            return pNode->data;
        }
        bool operator!=(const Iterator &iterator)
        {
            return pNode != iterator.pNode;
        }
        // Prefix ++ overload
        Iterator &operator++()
        {
            pNode = pNode->next;
            return *this;
        }
        // Postfix ++ overload
        Iterator operator++(int)
        {
            Iterator iterator = *this;
            ++*this;
            return iterator;
        }
    };
    class BWDIterator {
    private:
        DLinkedList<T>* pList;
        Node* pNode;

    public:
        BWDIterator(DLinkedList<T>* pList = 0, bool last = true) {
            if (last) {
                if (pList != 0)
                    this->pNode = pList->tail->prev;
                else
                    pNode = 0;
            }
            else {
                if (pList != 0)
                    this->pNode = pList->head;
                else
                    pNode = 0;
            }
            this->pList = pList;
        }

        BWDIterator& operator=(const BWDIterator& iterator) {
            this->pNode = iterator.pNode;
            this->pList = iterator.pList;
            return *this;
        }

        void remove(void (*removeItemData)(T) = 0) {
            pNode->prev->next = pNode->next;
            pNode->next->prev = pNode->prev;
            Node* pPrev = pNode->next; // MUST be next, so iterator-- will go to end
            if (removeItemData != 0)
                removeItemData(pNode->data);
            delete pNode;
            pNode = pPrev;
            pList->count -= 1;
        }

        T& operator*() {
            return pNode->data;
        }

        bool operator!=(const BWDIterator& iterator) {
            return pNode != iterator.pNode;
        }

        // Prefix ++ overload
        BWDIterator& operator++() {
            pNode = pNode->prev;
            return *this;
        }

        // Postfix ++ overload
        BWDIterator operator++(int) {
            BWDIterator iterator = *this;
            ++*this;
            return iterator;
        }
    };

};
//////////////////////////////////////////////////////////////////////
// Define a shorter name for DLinkedList:

template <class T>
using List = DLinkedList<T>;

//////////////////////////////////////////////////////////////////////
////////////////////////     METHOD DEFNITION      ///////////////////
//////////////////////////////////////////////////////////////////////

template <class T>
DLinkedList<T>::DLinkedList(
    void (*deleteUserData)(DLinkedList<T> *),
    bool (*itemEqual)(T &, T &)) : head(new Node()), tail(new Node()), count(0), deleteUserData(deleteUserData), itemEqual(itemEqual)
{
    this->head->next = this->tail;
    this->tail->prev = this->head;
}

template <class T>
DLinkedList<T>::DLinkedList(const DLinkedList<T>& list)
{
    head = new Node(); 
    tail = new Node(); 
    head->next = tail;
    tail->prev = head;
    count = 0;
    deleteUserData = list.deleteUserData;
    itemEqual = list.itemEqual;

    // Copy each element from the provided list
    Node* current = list.head->next;
    while (current != list.tail) {   
        this->add(current->data);
        current = current->next;
    }
}


template <class T>
DLinkedList<T> &DLinkedList<T>::operator=(const DLinkedList<T> &list)
{
    if (this != &list) {
        removeInternalData();
        deleteUserData = list.deleteUserData;
        itemEqual = list.itemEqual;

        Node* current = list.head->next; 
        while (current != list.tail) {   
            this->add(current->data);
            current = current->next;
        }
    }
    return *this;
}

template <class T>
DLinkedList<T>::~DLinkedList()
{
    removeInternalData();
    delete this->head;
    delete this->tail;
}

template <class T>
void DLinkedList<T>::add(T e)
{
    Node* newNode = new Node(e, this->tail, this->tail->prev);
    this->tail->prev->next = newNode;
    this->tail->prev = newNode;
    this->count++;
}
template <class T>
void DLinkedList<T>::add(int index, T e)
{
    if (index < 0 || index > count) {
        throw std::out_of_range("Index is out of range!");
    }
    Node* newNode = new Node(e);
    Node* prevNode;
    if (index == 0) {
        prevNode = head;
    }
    else if (index == count) {
        prevNode = tail->prev;
    }
    else {
        prevNode = getPreviousNodeOf(index);
    }
    newNode->next = prevNode->next;
    newNode->prev = prevNode;
    prevNode->next->prev = newNode;
    prevNode->next = newNode;
    count++;
}

template <class T>
typename DLinkedList<T>::Node *DLinkedList<T>::getPreviousNodeOf(int index)
{
    /**
     * Returns the node preceding the specified index in the doubly linked list.
     * If the index is in the first half of the list, it traverses from the head; otherwise, it traverses from the tail.
     * Efficiently navigates to the node by choosing the shorter path based on the index's position.
     */
    if (index < 0 || index >= count) {
        throw std::out_of_range("Index is out of range!");
    }

    Node* p = nullptr;
    if (index < count / 2) {
        p = this->head;
        for (int i = 0; i < index; i++) p = p->next;
    }
    else {
        p = this->tail;
        for (int i = count; i >= index; i--) p = p->prev;
    }
    return p;
}

template <class T>
T DLinkedList<T>::removeAt(int index)
{
    if (index < 0 || index >= count) {
        throw std::out_of_range("Index is out of range!");
    }

    Node* nodeToRemove = getPreviousNodeOf(index)->next;
    T removedData = nodeToRemove->data;

    nodeToRemove->prev->next = nodeToRemove->next;
    nodeToRemove->next->prev = nodeToRemove->prev;

    delete nodeToRemove;
    count--;

    return removedData;
}

template <class T>
bool DLinkedList<T>::empty()
{
    return (count == 0);
}

template <class T>
int DLinkedList<T>::size()
{
    return count;
}

template <class T>
void DLinkedList<T>::clear()
{
    if (count == 0) return;
    if (deleteUserData != nullptr) {deleteUserData(this);}
    Node* current = head->next; 
    while (current != tail) {    
        Node* temp = current;
        current = current->next;
        delete temp;
        }
    head->next = tail;
    tail->prev = head;
    count = 0;
    }   

template <class T>
T& DLinkedList<T>::get(int index)
{
    if (index < 0 || index >= count) {
        throw std::out_of_range("Index is out of range!");
    }

    Node* node = getPreviousNodeOf(index)->next;
    return node->data;
}


template <class T>
int DLinkedList<T>::indexOf(T item)
{
    Node* currentNode = head->next;
    for (int i = 0; i < count; i++) {
        if (equals(currentNode->data, item, itemEqual)) {
            return i;
        }
        currentNode = currentNode->next;
    }
    return -1;
}


template <class T>
bool DLinkedList<T>::removeItem(T item, void (*removeItemData)(T))
{
    int index = indexOf(item);
    if (index != -1) {
        T removedItem = removeAt(index);
        if (removeItemData != nullptr) {
            removeItemData(removedItem);
        }
        return true;
    }
    return false;
}


template <class T>
bool DLinkedList<T>::contains(T item)
{
    return indexOf(item) != -1;
}


template <class T>
string DLinkedList<T>::toString(string (*item2str)(T &))
{
    /**
     * Converts the list into a string representation, where each element is formatted using a user-provided function.
     * If no custom function is provided, it directly uses the element's default string representation.
     * Example: If the list contains {1, 2, 3} and the provided function formats integers, calling toString would return "[1, 2, 3]".
     *
     * @param item2str A function that converts an item of type T to a string. If null, default to string conversion of T.
     * @return A string representation of the list with elements separated by commas and enclosed in square brackets.
     */
    stringstream ss;
    ss << "[";
    Node* currentNode = head->next;
    while (currentNode != tail) {
        if (item2str) {
            ss << item2str(currentNode->data);
        }
        else {
            ss << currentNode->data;
        }
        if (currentNode->next != tail) {
            ss << ", ";
        }
        currentNode = currentNode->next;
    }
    ss << "]";
    return ss.str();
}

template <class T>
void DLinkedList<T>::copyFrom(const DLinkedList<T>& list)
{
    clear();  
    Node* currentNode = list.head->next;
    while (currentNode != list.tail) {
        add(count, currentNode->data);
        currentNode = currentNode->next;
    }
}


template <class T>
void DLinkedList<T>::removeInternalData()
{
    clear();
    deleteUserData = nullptr;
    itemEqual = nullptr;
}


#endif /* DLINKEDLIST_H */