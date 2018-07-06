// Part of DTSource. Copyright 2016-2017. David Adalsteinsson.
// see http://www.visualdatatools.com/DTSource/license.html for more information.

#include "DTNetwork.h"
#include "DTError.h"
#include "DTDataStorage.h"

DTNetwork::DTNetwork(const DTIntArray &indices,const DTDoubleArray &weights,int maxNode)
{
    int maxIndex = -1;
    if (ValidIndices(indices,maxIndex)==false) {
        // Stay empty
        return;
    }
    if (weights.IsEmpty()==false && (weights.Length()!=indices.n() || weights.m()!=weights.Length())) {
        DTErrorMessage("DTNetwork(indices,weight)","Weight list has the wrong length");
        return;
    }
    
    _edges = indices;
    _weights = weights;
    _numberOfNodes = (maxNode<0 ? maxIndex : std::max(maxNode,maxIndex));
}

bool DTNetwork::ValidIndices(const DTIntArray &indices,int &maxNodeIndex)
{
    maxNodeIndex = 0;
    if (indices.IsEmpty()) return true;
    if (indices.m()!=2 || indices.o()!=1) {
        DTErrorMessage("Network::ValidIndices","Array has the wrong size.");
        return false;
    }
    size_t i,N = indices.n();
    int idx=0;
    int maxSoFar = 0;
    for (i=0;i<N;i++) {
        idx = indices(0,i);
        if (idx<0) break;
        if (idx>maxSoFar) maxSoFar = idx;
        idx = indices(1,i);
        if (idx<0) break;
        if (idx>maxSoFar) maxSoFar = idx;
    }
    if (i<N) {
        if (idx<0) {
            DTErrorMessage("Network::ValidIndices","Index is < 0.");
            return false;
        }
    }
    maxNodeIndex = maxSoFar+1;
    
    return true;
}

DTNetwork::DTNetwork(const DTIntArray &indices,const DTDoubleArray &weights,const DTIntArray &index,int maxNode)
{
    int maxIndex = -1;
    if (ValidIndices(indices,maxIndex)==false) {
        // Stay empty
        return;
    }
    
    if (weights.IsEmpty()==false && (weights.Length()!=indices.n() || weights.m()!=weights.Length())) {
        DTErrorMessage("DTNetwork(indices,weight)","Weight list has the wrong length");
        return;
    }

    if (index.IsEmpty()==false && (index.Length()>maxIndex)) {
        DTErrorMessage("DTNetwork(indices,weights,index)","index out of bounds");
        return;
    }
    
    _edges = indices;
    _weights = weights;
    _index = index;
    _numberOfNodes = (maxNode<0 ? maxIndex : std::max(maxNode,maxIndex));
}

DTNetwork::DTNetwork(const DTIntArray &indices,const DTIntArray &index,int maxNode)
{
    int maxIndex = -1;
    if (ValidIndices(indices,maxIndex)==false) {
        // Stay empty
        return;
    }
    
    if (index.IsEmpty()==false && (index.Length()>maxIndex)) {
        DTErrorMessage("DTNetwork(indices,weights,index)","index out of bounds");
        return;
    }
    
    _edges = indices;
    _index = index;
    _numberOfNodes = (maxNode<0 ? maxIndex : std::max(maxNode,maxIndex));
}

DTDoubleArray DTNetwork::Weights(void) const
{
    if (_weights.IsEmpty() && _edges.n()!=0) DTErrorMessage("DTNetwork::Weights","No weight specified");
    return _weights;
}

DTIntArray DTNetwork::Index(void) const
{
    if (_index.IsEmpty() && _edges.n()!=0) DTErrorMessage("DTNetwork::Weights","No weight specified");
    return _index;
}

void DTNetwork::pinfo(void) const
{
    if (IsEmpty()) {
        std::cerr << "Empty\n" << std::flush;
    }
    else {
        std::cerr << M() << " edges";
        
        if (WeightDefined()) std::cerr << " + weights";
        if (IndexDefined()) std::cerr << " + index";
        
        std::cerr << std::endl << std::flush;
    }
}

void DTNetwork::pall(void) const
{
    // connectivity and weights in a table
    
}

void DTNetwork::pedge(int) const
{
    // a single edge with weight if given
}

void DTNetwork::pnode(int) const
{
    // all edges that contain a given node
}
    
DTNetwork OverwriteWeights(const DTNetwork &net,const DTDoubleArray &weights)
{
    return DTNetwork(net.Edges(),weights,net.NumberOfNodes());
}

DTNetwork OverwriteIndex(const DTNetwork &net,const DTIntArray &index)
{
    if (net.WeightDefined()) {
        return DTNetwork(net.Edges(),net.Weights(),index,net.NumberOfNodes());
    }
    else {
        return DTNetwork(net.Edges(),index,net.NumberOfNodes());
    }
}

DTNetwork RemoveWeights(const DTNetwork &net)
{
    if (net.WeightDefined()==false)
        return net;

    return DTNetwork(net.Edges(),DTDoubleArray());
}

void Read(const DTDataStorage &input,const std::string &name,DTNetwork &toReturn)
{
    DTIntArray edges = input.ReadIntArray(name);
    if (edges.IsEmpty()) {
        toReturn = DTNetwork();
        return;
    }
    
    DTDoubleArray weights;
    if (input.Contains(name+"_weight")) {
        weights = input.ReadDoubleArray(name+"_weight");
    }
    DTIntArray indices;
    if (input.Contains(name+"_index")) {
        indices = input.ReadIntArray(name+"_index");
    }
    
    toReturn = DTNetwork(edges,weights,indices);
}

void Write(DTDataStorage &output,const std::string &name,const DTNetwork &theVar)
{
    if (theVar.WeightDefined())
        output.Save(theVar.Weights(),name+"_weight");
    else if (output.Contains(name+"_weight"))
        output.Save(DTDoubleArray(),name+"_weight");
    
    if (theVar.IndexDefined())
        output.Save(theVar.Index(),name+"_index");
    else if (output.Contains(name+"_index"))
        output.Save(DTIntArray(),name+"_index");
    
    output.Save(theVar.Edges(),name);
}

void WriteOne(DTDataStorage &output,const std::string &name,const DTNetwork &toWrite)
{
    Write(output,name,toWrite);
    Write(output,"Seq_"+name,"Network");
    output.Flush();
}

