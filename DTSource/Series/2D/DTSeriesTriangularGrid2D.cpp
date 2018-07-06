// Part of DTSource. Copyright 2004-2017. David Adalsteinsson.
// see http://www.visualdatatools.com/DTSource/license.html for more information.

#include "DTSeriesTriangularGrid2D.h"

#include "DTTriangularGrid2D.h"
#include "DTSeriesTemplates.h"

map<string,DTSeriesTriangularGrid2D> DTGlobalSeriesTriangularGrid2D;

void DTSeriesTriangularGrid2D::Register() const
{
    Register(Name());
}

void DTSeriesTriangularGrid2D::Register(string name) const
{
    DTRegisteredSeries<DTSeriesTriangularGrid2D,map<string,DTSeriesTriangularGrid2D>::const_iterator>(DTGlobalSeriesTriangularGrid2D,name,"DTSeriesTriangularGrid2D",this);
}

void DTSeriesTriangularGrid2D::pregistered(void)
{
    DTPrintRegisteredSeries<DTSeriesTriangularGrid2D,map<string,DTSeriesTriangularGrid2D>::const_iterator>(DTGlobalSeriesTriangularGrid2D);
}

bool DTSeriesTriangularGrid2D::HasBeenRegistered(string name)
{
    return DTHasRegisteredSeries<DTSeriesTriangularGrid2D,map<string,DTSeriesTriangularGrid2D>::const_iterator>(DTGlobalSeriesTriangularGrid2D,name);
}

DTSeriesTriangularGrid2D DTSeriesTriangularGrid2D::ByName(string name)
{
    return DTFindRegisteredSeries<DTSeriesTriangularGrid2D,map<string,DTSeriesTriangularGrid2D>::const_iterator>(DTGlobalSeriesTriangularGrid2D,name,"DTSeriesTriangularGrid2D");
}

void DTSeriesTriangularGrid2D::Add(DTTriangularGrid2D v,double time)
{
    if (SharedSave(time)==false) return;
    
    Write(DataFile(),baseName(),v,saveInfo);
    
    AddToBBox(BoundingBox(v));
    DataFile().Flush();
}

DTTriangularGrid2D DTSeriesTriangularGrid2D::Get(double time) const
{
    int k = IndexForTime(time);
    if (k<0) return DTTriangularGrid2D();
    DTTriangularGrid2D toReturn;
    Read(DataFile(),baseName(k),toReturn,saveInfo);
    return toReturn;
}

void Read(const DTDataStorage &input,const std::string &name,DTSeriesTriangularGrid2D &toReturn)
{
    string variableName;
    DTMutablePointer<DTDataStorage> storage = DTSeries::ReferencedDataFile(input,name,variableName);
    toReturn = DTSeriesTriangularGrid2D(*storage,variableName);
}
