/***************************************************************************
 * Basic datatypes to construct a octtree
 *
 * Author: Renaud Marlet (c) 2011-2015
 ***************************************************************************/

#pragma once

#include <string>
#include <iostream>
#include <stdexcept>



// The number of directions
const int nbDir = 8;

// Forward declarations (so that the class names can be used in OctTree)
template <typename T> class OctLeaf; // The type of a OctTree leaf
template <typename T> class OctNode; // The type of a OctTree node

/*--------------------------------------------------------------------------*
 * Abstract class for kdtrees, whose leaves contain a value of type T
 *
 * Note that this class cannot be instantiated.  Only the OctLeaf and OctNode
 * classes can be instantiated.  However, generic methods can be defined
 * at the level of this superclass.
 *--------------------------------------------------------------------------*/
template <typename T>
class OctTree {
public:
    // Constructors are defined in each subclass

    // Destruct this node or leaf and, recursively, subtrees
    // Note: deletion of oct-dag (octtree with node sharing) is not supported
    virtual ~OctTree() {}

    // Tell if this tree is a leaf
    virtual bool isLeaf() = 0; // This makes QuadTree an abstract class

    // Tell if this tree is a node
    inline virtual bool isNode() { return !isLeaf();}

    // Return the number of leaves in this tree
    virtual int nbLeaves() = 0;

    // Return the number of nodes in this tree (excluding leaves)
    virtual int nbNodes() = 0;

    // Return the number of (sub)trees in this tree (nodes and leaves)
    virtual int nbTrees() = 0;

    ///// OctLeaf methods

    // Return the value of this Tree
    // Throw domain_error if not a Leaf
    virtual const T value() const = 0;

    // Return a reference to the value of this Leaf (for assignments)
    // Throw domain_error if not a Leaf
    virtual T& value() = 0;

    ///// OctNode methods

    // Return the son of this tree in direction d
    // Throw domain_error if not an OctNode
    virtual OctTree<T>* const& son(int d) const = 0;

    // Return a reference to the son of this tree in direction d
    // Throw domain_error if not an OctNode
    virtual OctTree<T>*& son(int d) = 0;
};



/*--------------------------------------------------------------------------*
 * A leaf of an Octtree, containing a value of type T
 *--------------------------------------------------------------------------*/
template <typename T>
class OctLeaf : public OctTree<T>
{
    // The value contained in this leaf
    T val;

public:
    // Tell if this tree is a leaf
    inline bool isLeaf() { return true; }

    // Construct a OctLeaf with the given value
    OctLeaf(T value) : val(value) {}

    // Return the number of leaves in this tree
    inline int nbLeaves() { return 1; }

    // Return the number of nodes in this tree (excluding leaves)
    inline int nbNodes() { return 0; }

    // Return the number of (sub)trees in this tree (nodes and leaves)
    inline int nbTrees() { return 1; }

    ///// OctLeaf methods

    // Return the value of this OctLeaf
    inline const T value() const { return val; }

    // Return a reference to the value of this OctLeaf (for assignments)
    inline T& value() { return val; }

    ///// OctNode methods

    // Supposed to return the son of this tree in direction d
    // But throw domain_error as not an OctNode
    inline OctTree<T>* const& son(int d) const {
	throw new std::domain_error("Not an OctNode"); }

    // Supposed to return a reference to the son of this tree in direction d
    // But throw domain_error as not an OctNode
    inline OctTree<T>*& son(int d) {
	throw new std::domain_error("Not an OctNode"); }

};



/*--------------------------------------------------------------------------*
 * A branching node of an Octtree, whose leaves contain a value of type T
 *--------------------------------------------------------------------------*/
template <typename T>
class OctNode : public OctTree<T>
{
    // The sons of this node for the 8 quadrant directions
    OctTree<T>* sons[nbDir];

public:
    // Tell if this tree is a leaf
    inline bool isLeaf() { return false; }

    // Construct a new branching node given an array of 8 trees
    OctNode(OctTree<T>* trees[])
    {
	// Assign sons
        for (int i = 0; i < nbDir; i++)
	    sons[i] = trees[i];
    }

    // Construct a new branching node with empty (null) sons
    OctNode()
    {
	// Assign sons
        for (int i = 0; i < nbDir; i++)
	    sons[i] = nullptr;
    }


    // Destruct this OctNode and, recursively, subtrees
    // Note: deletion of oct-dag (tree with node sharing) is not supported
    ~OctNode()
    {
	// For each direction
        for (int i = 0; i < nbDir; i++)
	    // Delete the corresponding son
	    if (sons[i])
		delete sons[i];
    }

    // Return the number of leaves in this tree
    inline int nbLeaves()
    {
      int n = 0;
      for (int i = 0; i < nbDir; i++)
	if (sons[i] != nullptr)
	  n += sons[i]->nbLeaves();
      return n;
    }

    // Return the number of nodes in this tree (excluding leaves)
    inline int nbNodes()
    {
      int n = 1;
      for (int i = 0; i < nbDir; i++)
	if (sons[i] != nullptr)
	  n += sons[i]->nbNodes();
      return n;
    }

    // Return the number of (sub)trees in this tree (nodes and leaves)
    inline int nbTrees()
    {
      int n = 1;
      for (int i = 0; i < nbDir; i++)
	if (sons[i] != nullptr)
	  n += sons[i]->nbTrees();
      return n;
    }

    ///// OctLeaf methods

    // Supposed to return the value of this OctTree
    // But throw a domain_error as it is not a OctLeaf
    inline const T value() const { throw new std::domain_error("Not an OctLeaf"); }

    // Supposed to return a reference to the value of this OctTree
    // But throw a domain_error as it is not a OctLeaf
    inline T& value() { throw new std::domain_error("Not a OctLeaf"); }

    ///// OctNode methods

    // Return the son of this tree in direction d
    inline OctTree<T>* const& son(int d) const
    {
	// Make sure the direction makes sense
	if(!(0 <= d && d < nbDir))
	    throw new std::out_of_range("Not a valid direction: "+
					std::to_string(d));
	// Yield son
	return sons[d];
    }

    // Return a reference to the son of this tree in direction d
    inline OctTree<T>*& son(int d)
    {
	// Make sure the direction makes sense */
	if (!(0 <= d && d < nbDir))
	    throw new std::out_of_range("Not a valid direction: "+
					std::to_string(d));
	// Return son reference
	return sons[d];
    }
};

