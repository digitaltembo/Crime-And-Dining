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
 ******************************************************************************/


// a few constructors:
template<class T>
QuadTree<T>::QuadTree(){
    root = NULL;
}

template<class T>
QuadTree<T>::QuadTree(Location l, T* rootData){
    root = newNode(l,  rootData);
}



// Creates a QuadNode out of a location and an object pointer
template<class T>
struct QuadNode<T>* QuadTree<T>::newNode(Location l, T* data){
    struct QuadNode<T>* n = new struct QuadNode<T>;
    n->children[0] = n->children[1] =
        n->children[2] = n->children[3] = NULL;
    n->l = l;
    n->data = data;
}

// creates a QuadNode with newNode and inserts it into the tree with insertNode
template<class T>
void QuadTree<T>::insert(Location l, T* data){
    if(!root){
        root = newNode(l, data);
        return;
    }
    struct QuadNode<T>* toInsert = newNode(l, data);
    insertNode(root, toInsert);
}

/* This function returns the index of a QuadNode children array that location b
 * should have relative to location a.
 * The numbers in this grid correspond to the index in the QuadNode children array
 *   * ****|******
 *   *  1  |  0  *
 *   *-----+-----*
 *   *  2  |  3  *
 *   ******|******
 * 
 * That is, looking at the position of b relative to a, if it is NE this returns 0,
 * NW = 1, SW = 2, and SE = 4
 */
template<class T>
int QuadTree<T>::comparePositions(Location a, Location b){
    
    
    if(b.y > a.y){
        if(b.x > a.x)
            return 0;
        return 1;
    }
    if(b.x < a.x)
        return 2;
    return 3;
}

// A recursive function to insert the node toInsert into the subtree with a root at node
template<class T>
void QuadTree<T>::insertNode(struct QuadNode<T>* node, struct QuadNode<T>* toInsert){
    char childIndex = comparePositions(node->l, toInsert->l);
    if(node->children[childIndex] == NULL)
        node->children[childIndex] = toInsert;
    else
        insertNode(node->children[childIndex], toInsert);
}

// Calls the function mapFunction taking argument object pointer and void* for each
// object in the quadtree and with the closure cl.
template<class T>
void QuadTree<T>::mapNodes( void (*mapFunction)(T*, void*), void* cl ){
    mapNodesRecursive(root, mapFunction, cl);
}

// Recursively calls the function mapFunction taking argument object pointer and void*
// for eac object in the quadtree and with the closure cl.
template<class T>
void QuadTree<T>::mapNodesRecursive(struct QuadNode<T>* node,  void (*mapFunction)(T*, void*), void* cl){
    if(node)
        mapFunction(node->data, cl);
    for(int i=0;i<4;i++)
        if(node->children[i])
            mapNodesRecursive(node->children[i], mapFunction, cl);
}

// returns a vector of object pointers within a radius radius of the location l
template<class T>
std::vector<T*> QuadTree<T>::findNodes(Location l, int radius){
    std::vector<T*> v;
    findNodesRecursive(root, l, radius, v);
    return v;
}

// apparently I needed an abs function
double abs(double x){
    return (x<0) ? -x : x;
}

// a recursive function to fund nodes in the subtree with a root at node that
// are of a distance less than radius from the location l
// If it finds any, it adds them to the referenced vector v
template<class T>
void QuadTree<T>::findNodesRecursive(struct QuadNode<T>* node, 
                                                Location l, 
                                                int radius, 
                                                std::vector<T*>& v){
    /*Remember:
     * 
     ******|******
     *  1  |  0  *
     *-----+-----*
     *  2  |  3  *
     ******|******
     
     NE = 0, NW = 1, SW = 2, SE = 4
     */
    if(node == NULL)
        return;

    bool withinX = abs(node->l.x - l.x) < radius;
    bool withinY = abs(node->l.y - l.y) < radius;
    if(withinX && withinY){
        if(node->l.distSquared(l) < radius*radius){
            v.push_back(node->data);
        }
        for(int i=0;i<4;i++)
            if(node->children[i])
                findNodesRecursive(node->children[i], l, radius, v);
    }else if(withinX){
        if(node->l.y < l.y){
            findNodesRecursive(node->children[0], l, radius, v);
            findNodesRecursive(node->children[1], l, radius, v);
        }else{
            findNodesRecursive(node->children[2], l, radius, v);
            findNodesRecursive(node->children[3], l, radius, v);
        }
    }else if(withinY){
        if(node->l.x < l.x){
            findNodesRecursive(node->children[0], l, radius, v);
            findNodesRecursive(node->children[3], l, radius, v);
        }else{
            findNodesRecursive(node->children[1], l, radius, v);
            findNodesRecursive(node->children[2], l, radius, v);
        }
    }else{
        findNodesRecursive(node->children[comparePositions(node->l, l)], l, radius, v);
    }    
}

// The destructor merely calls deleteNode for the root node. 
template<class T>
QuadTree<T>::~QuadTree(){
    if(root!=NULL)
        deleteNode(root);
}

// deleteNode deletes the data of a node and recurses on such children as it has
template<class T>
void QuadTree<T>::deleteNode(struct QuadNode<T>* node){
    delete node->data;
    for(int i=0;i<4;i++)
        if(node->children[i])
            deleteNode(node->children[i]);
}