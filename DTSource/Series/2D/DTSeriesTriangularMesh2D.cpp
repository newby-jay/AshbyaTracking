// Part of DTSource. Copyright 2004-2017. David Adalsteinsson.
// see http://www.visualdatatools.com/DTSource/license.html for more information.

#include "DTSeriesTriangularMesh2D.h"

#include "DTTriangularMesh2D.h"
#include "DTSeriesTemplates.h"

map<string,DTSeriesTriangularMesh2D> DTGlobalSeriesTriangularMesh2D;

void DTSeriesTriangularMesh2D::Register() const
{
    Register(Name());
}

void DTSeriesTriangularMesh2D::Register(string name) const
{
    DTRegisteredSeries<DTSeriesTriangularMesh2D,map<string,DTSeriesTriangularMesh2D>::const_iterator>(DTGlobalSeriesTriangularMesh2D,name,"DTSeriesTriangularMesh2D",this);
}

void DTSeriesTriangularMesh2D::pregistered(void)
{
    DTPrintRegisteredSeries<DTSeriesTriangularMesh2D,map<string,DTSeriesTriangularMesh2D>::const_iterator>(DTGlobalSeriesTriangularMesh2D);
}

bool DTSeriesTriangularMesh2D::HasBeenRegistered(string name)
{
    return DTHasRegisteredSeries<DTSeriesTriangularMesh2D,map<string,DTSeriesTriangularMesh2D>::const_iterator>(DTGlobalSeriesTriangularMesh2D,name);
}

DTSeriesTriangularMesh2D DTSeriesTriangularMesh2D::ByName(string name)
{
    return DTFindRegisteredSeries<DTSeriesTriangularMesh2D,map<string,DTSeriesTriangularMesh2D>::const_iterator>(DTGlobalSeriesTriangularMesh2D,name,"DTSeriesTriangularMesh2D");
}

void DTSeriesTriangularMesh2D::Add(DTTriangularMesh2D v,double time)
{
    if (SharedSave(time)==false) return;
    
    Write(DataFile(),baseName(),v,saveInfo);
    
    AddToBBox(BoundingBox(v));
    AddToValueRange(ValueRange(v));
    DataFile().Flush();
}

DTTriangularMesh2D DTSeriesTriangularMesh2D::Get(double time) const
{
    int k = IndexForTime(time);
    if (k<0) return DTTriangularMesh2D();
    DTTriangularMesh2D toReturn;
    Read(DataFile(),baseName(k),toReturn,saveInfo);
    return toReturn;
}

void Read(const DTDataStorage &input,const std::string &name,DTSeriesTriangularMesh2D &toReturn)
{
    string variableName;
    DTMutablePointer<DTDataStorage> storage = DTSeries::ReferencedDataFile(input,name,variableName);
    toReturn = DTSeriesTriangularMesh2D(*storage,variableName);
}
