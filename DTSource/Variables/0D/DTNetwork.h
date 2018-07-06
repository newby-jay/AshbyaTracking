// Part of DTSource. Copyright 2016-2017. David Adalsteinsson.
// see http://www.visualdatatools.com/DTSource/license.html for more information.

#ifndef DTNetwork_H
#define DTNetwork_H

#include "DTIntArray.h"
#include "DTDoubleArray.h"

class DTDataStorage;

class DTNetwork
{
public:
    DTNetwork() {}
    DTNetwork(const DTIntArray &indices,const DTDoubleArray &weights,int maxNode = -1);
    DTNetwork(const DTIntArray &indices,const DTDoubleArray &weights,const DTIntArray &index,int maxNode = -1);
    DTNetwork(const DTIntArray &indices,const DTIntArray &index,int maxNode = -1);
   
    // High level description
    bool IsEmpty(void) const {return _edges.IsEmpty();}
    ssize_t NumberOfNodes(void) const {return _numberOfNodes;}
    ssize_t N(void) const {return _numberOfNodes;}
    ssize_t NumberOfEdges(void) const {return _edges.n();}
    ssize_t M(void) const {return _edges.n();}
    bool WeightDefined(void) const {return (_edges.IsEmpty() || _weights.IsEmpty()==false);}
    bool IndexDefined(void) const {return (_edges.IsEmpty() || _index.IsEmpty()==false);}
    
    // Access to the data
    DTIntArray Edges(void) const {return _edges;}
    DTDoubleArray Weights(void) const; // Complains if there is no weight
    DTIntArray Index(void) const; // Complains if there is no index
    
    // Debug aids, print to cerr
    void pinfo(void) const;
    void pall(void) const; // connectivity and weights in a table
    void pedge(int) const; // a single edge with weight if given
    void pnode(int) const; // all edges that contain a given node
    
    static bool ValidIndices(const DTIntArray &,int &maxNodeIndex);
    
private:
    ssize_t _numberOfNodes;
    DTIntArray _edges; // 2xN list
    DTDoubleArray _weights; // Optional, per edge
    DTIntArray _index; // Optional, per node
};

// To modify an entry, use these functions.  Will print out error messages if there is a problem.
extern DTNetwork OverwriteWeights(const DTNetwork &,const DTDoubleArray &); // Changes/overwrites the weights.  Length needs to match
extern DTNetwork OverwriteIndex(const DTNetwork &,const DTIntArray &);
extern DTNetwork RemoveWeights(const DTNetwork &);


// Reading and writing
extern void Read(const DTDataStorage &input,const std::string &name,DTNetwork &toReturn);
extern void Write(DTDataStorage &output,const std::string &name,const DTNetwork &theVar);
extern void WriteOne(DTDataStorage &output,const std::string &name,const DTNetwork &toWrite); // One time value, self documenting.

#endif
