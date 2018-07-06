// Part of DTSource. Copyright 2004-2017. David Adalsteinsson.
// see http://www.visualdatatools.com/DTSource/license.html for more information.

#include "DTSeriesTetrahedralVectorField3D.h"

#include "DTTetrahedralVectorField3D.h"
#include "DTSeriesTemplates.h"

map<string,DTSeriesTetrahedralVectorField3D> DTGlobalSeriesTetrahedralVectorField3D;

void DTSeriesTetrahedralVectorField3D::Register() const
{
    Register(Name());
}

void DTSeriesTetrahedralVectorField3D::Register(string name) const
{
    DTRegisteredSeries<DTSeriesTetrahedralVectorField3D,map<string,DTSeriesTetrahedralVectorField3D>::const_iterator>(DTGlobalSeriesTetrahedralVectorField3D,name,"DTSeriesTetrahedralVectorField3D",this);
}

void DTSeriesTetrahedralVectorField3D::pregistered(void)
{
    DTPrintRegisteredSeries<DTSeriesTetrahedralVectorField3D,map<string,DTSeriesTetrahedralVectorField3D>::const_iterator>(DTGlobalSeriesTetrahedralVectorField3D);
}

bool DTSeriesTetrahedralVectorField3D::HasBeenRegistered(string name)
{
    return DTHasRegisteredSeries<DTSeriesTetrahedralVectorField3D,map<string,DTSeriesTetrahedralVectorField3D>::const_iterator>(DTGlobalSeriesTetrahedralVectorField3D,name);
}

DTSeriesTetrahedralVectorField3D DTSeriesTetrahedralVectorField3D::ByName(string name)
{
    return DTFindRegisteredSeries<DTSeriesTetrahedralVectorField3D,map<string,DTSeriesTetrahedralVectorField3D>::const_iterator>(DTGlobalSeriesTetrahedralVectorField3D,name,"DTSeriesTetrahedralVectorField3D");
}

void DTSeriesTetrahedralVectorField3D::Add(DTTetrahedralVectorField3D v,double time)
{
    if (SharedSave(time)==false) return;
    
    Write(DataFile(),baseName(),v,saveInfo);
    
    AddToBBox(BoundingBox(v));
    DataFile().Flush();
}

DTTetrahedralVectorField3D DTSeriesTetrahedralVectorField3D::Get(double time) const
{
    int k = IndexForTime(time);
    if (k<0) return DTTetrahedralVectorField3D();
    DTTetrahedralVectorField3D toReturn;
    Read(DataFile(),baseName(k),toReturn,saveInfo);
    return toReturn;
}

DTTetrahedralVectorField3D DTSeriesTetrahedralVectorField3D::operator()(int index) const
{
    if (index<0 || index>=HowManySaved()) {
        DTErrorOutOfRange("DTSeriesTetrahedralVectorField3D(i)",index,HowManySaved());
        return DTTetrahedralVectorField3D();
    }
    DTTetrahedralVectorField3D toReturn;
    Read(DataFile(),baseName(index),toReturn);
    return toReturn;
}

void Read(const DTDataStorage &input,const std::string &name,DTSeriesTetrahedralVectorField3D &toReturn)
{
    string variableName;
    DTMutablePointer<DTDataStorage> storage = DTSeries::ReferencedDataFile(input,name,variableName);
    toReturn = DTSeriesTetrahedralVectorField3D(*storage,variableName);
}
