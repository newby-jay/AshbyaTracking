// Part of DTSource. Copyright 2004-2015. David A. David Adalsteinsson.
// see http://www.visualdatatools.com/DTSource/license.html for more information.

#include "DTSeriesTetrahedralGrid3D.h"

#include "DTTetrahedralGrid3D.h"
#include "DTSeriesTemplates.h"

map<string,DTSeriesTetrahedralGrid3D> DTGlobalSeriesTetrahedralGrid3D;

void DTSeriesTetrahedralGrid3D::Register() const
{
    Register(Name());
}

void DTSeriesTetrahedralGrid3D::Register(string name) const
{
    DTRegisteredSeries<DTSeriesTetrahedralGrid3D,map<string,DTSeriesTetrahedralGrid3D>::const_iterator>(DTGlobalSeriesTetrahedralGrid3D,name,"DTSeriesTetrahedralGrid3D",this);
}

void DTSeriesTetrahedralGrid3D::pregistered(void)
{
    DTPrintRegisteredSeries<DTSeriesTetrahedralGrid3D,map<string,DTSeriesTetrahedralGrid3D>::const_iterator>(DTGlobalSeriesTetrahedralGrid3D);
}

bool DTSeriesTetrahedralGrid3D::HasBeenRegistered(string name)
{
    return DTHasRegisteredSeries<DTSeriesTetrahedralGrid3D,map<string,DTSeriesTetrahedralGrid3D>::const_iterator>(DTGlobalSeriesTetrahedralGrid3D,name);
}

DTSeriesTetrahedralGrid3D DTSeriesTetrahedralGrid3D::ByName(string name)
{
    return DTFindRegisteredSeries<DTSeriesTetrahedralGrid3D,map<string,DTSeriesTetrahedralGrid3D>::const_iterator>(DTGlobalSeriesTetrahedralGrid3D,name,"DTSeriesTetrahedralGrid3D");
}

void DTSeriesTetrahedralGrid3D::Add(DTTetrahedralGrid3D v,double time)
{
    if (SharedSave(time)==false) return;
    
    Write(DataFile(),baseName(),v,saveInfo);
    
    AddToBBox(BoundingBox(v));
    DataFile().Flush();
}

DTTetrahedralGrid3D DTSeriesTetrahedralGrid3D::Get(double time) const
{
    int k = IndexForTime(time);
    if (k<0) return DTTetrahedralGrid3D();
    DTTetrahedralGrid3D toReturn;
    Read(DataFile(),baseName(k),toReturn,saveInfo);
    return toReturn;
}

DTTetrahedralGrid3D DTSeriesTetrahedralGrid3D::operator()(int index) const
{
    if (index<0 || index>=HowManySaved()) {
        DTErrorOutOfRange("DTSeriesTetrahedralGrid3D(i)",index,HowManySaved());
        return DTTetrahedralGrid3D();
    }
    DTTetrahedralGrid3D toReturn;
    Read(DataFile(),baseName(index),toReturn);
    return toReturn;
}

void Read(const DTDataStorage &input,const std::string &name,DTSeriesTetrahedralGrid3D &toReturn)
{
    string variableName;
    DTMutablePointer<DTDataStorage> storage = DTSeries::ReferencedDataFile(input,name,variableName);
    toReturn = DTSeriesTetrahedralGrid3D(*storage,variableName);
}
