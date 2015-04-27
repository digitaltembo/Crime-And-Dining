/******************************************************************************
 * QuadTree.hpp                                                               *
 *                                                                            *
 * Nolan Hawkins, April 2015                                                  *
 *                                                                            *
 * This file describes what I believe to be a QuadTree data structure. The    *
 * main functions implemented here are inserting an object at a specific      *
 * location with insert(location, object), and querying the quadtree for all  *
 * objects within a certain distance of a certain location with               *
 * findNodes(location, radius). Additionally, there is the mapNodes()         *
 * function, which maps a function over all the elements of the QuadTree in   *
 * no particular order.                                                       *
 *                                                                            *
 * As a side note, on naming conventions: for regular C++ classes, I use      *
 * class.h and class.cpp as the header and source files, but for templated    *
 * classes this is all thrown to whack, so I indicate that the class is       *
 * templated in the file extension with the files class.hpp and class.tpp     *
 * as the header and template source files.                                   *
 ******************************************************************************/ 

#ifndef QUADTREE
#define QUADTREE

#include "Location.h"
#include <vector>

template<class T>
struct QuadNode{
    struct QuadNode* children[4];
    Location l;
    T* data;
};

    
template<class T>
class QuadTree{
public:
    QuadTree();
    // Initialize the QuadTree with a root element
    QuadTree(Location l, T* rootData);
    
   ~QuadTree();
    
    // Main Functions:
    void insert(Location l, T* data);
    
    std::vector<T*> findNodes(Location l, int radius);
    
    void mapNodes( void (*mapFunction)(T*, void*), void* cl );

    // Auxiliary Functions:
    // For insertion
    struct QuadNode<T>* newNode(Location l, T* data);
    void insertNode(struct QuadNode<T>* node, struct QuadNode<T>* toInsert);
    int comparePositions(Location a, Location b);
    
    // for findNodes
    void findNodesRecursive(struct QuadNode<T>* node, Location l, int radius, std::vector<T*>& v);
    
    // for mapNodes
    void mapNodesRecursive(struct QuadNode<T>* node,  void (*mapFunction)(T*, void*), void* cl);
    
    // for the destructor
    void deleteNode(struct QuadNode<T>* node);
    
    
    // And lastly, the root of the QuadTree
    struct QuadNode<T>* root;
};

#include "QuadTree.tpp"

#endif