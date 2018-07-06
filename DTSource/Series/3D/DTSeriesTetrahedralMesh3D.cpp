// Part of DTSource. Copyright 2004-2017. David Adalsteinsson.
// see http://www.visualdatatools.com/DTSource/license.html for more information.

#include "DTSeriesTetrahedralMesh3D.h"

#include "DTTetrahedralMesh3D.h"
#include "DTSeriesTemplates.h"

map<string,DTSeriesTetrahedralMesh3D> DTGlobalSeriesTetrahedralMesh3D;

void DTSeriesTetrahedralMesh3D::Register() const
{
    Register(Name());
}

void DTSeriesTetrahedralMesh3D::Register(string name) const
{
    DTRegisteredSeries<DTSeriesTetrahedralMesh3D,map<string,DTSeriesTetrahedralMesh3D>::const_iterator>(DTGlobalSeriesTetrahedralMesh3D,name,"DTSeriesTetrahedralMesh3D",this);
}

void DTSeriesTetrahedralMesh3D::pregistered(void)
{
    DTPrintRegisteredSeries<DTSeriesTetrahedralMesh3D,map<string,DTSeriesTetrahedralMesh3D>::const_iterator>(DTGlobalSeriesTetrahedralMesh3D);
}

bool DTSeriesTetrahedralMesh3D::HasBeenRegistered(string name)
{
    return DTHasRegisteredSeries<DTSeriesTetrahedralMesh3D,map<string,DTSeriesTetrahedralMesh3D>::const_iterator>(DTGlobalSeriesTetrahedralMesh3D,name);
}

DTSeriesTetrahedralMesh3D DTSeriesTetrahedralMesh3D::ByName(string name)
{
    return DTFindRegisteredSeries<DTSeriesTetrahedralMesh3D,map<string,DTSeriesTetrahedralMesh3D>::const_iterator>(DTGlobalSeriesTetrahedralMesh3D,name,"DTSeriesTetrahedralMesh3D");
}

void DTSeriesTetrahedralMesh3D::Add(DTTetrahedralMesh3D v,double time)
{
    if (SharedSave(time)==false) return;
    
    Write(DataFile(),baseName(),v,saveInfo);
    
    AddToBBox(BoundingBox(v));
    AddToValueRange(ValueRange(v));
    DataFile().Flush();
}

DTTetrahedralMesh3D DTSeriesTetrahedralMesh3D::Get(double time) const
{
    int k = IndexForTime(time);
    if (k<0) return DTTetrahedralMesh3D();
    DTTetrahedralMesh3D toReturn;
    Read(DataFile(),baseName(k),toReturn,saveInfo);
    return toReturn;
}

DTTetrahedralMesh3D DTSeriesTetrahedralMesh3D::operator()(int index) const
{
    if (index<0 || index>=HowManySaved()) {
        DTErrorOutOfRange("DTSeriesTetrahedralMesh3D(i)",index,HowManySaved());
        return DTTetrahedralMesh3D();
    }
    DTTetrahedralMesh3D toReturn;
    Read(DataFile(),baseName(index),toReturn);
    return toReturn;
}

void Read(const DTDataStorage &input,const std::string &name,DTSeriesTetrahedralMesh3D &toReturn)
{
    string variableName;
    DTMutablePointer<DTDataStorage> storage = DTSeries::ReferencedDataFile(input,name,variableName);
    toReturn = DTSeriesTetrahedralMesh3D(*storage,variableName);
}
