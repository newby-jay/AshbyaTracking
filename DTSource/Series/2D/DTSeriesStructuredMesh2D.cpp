// Part of DTSource. Copyright 2004-2015. David A. David Adalsteinsson.
// see http://www.visualdatatools.com/DTSource/license.html for more information.

#include "DTSeriesStructuredMesh2D.h"

#include "DTStructuredMesh2D.h"
#include "DTSeriesTemplates.h"

map<string,DTSeriesStructuredMesh2D> DTGlobalSeriesStructuredMesh2D;

void DTSeriesStructuredMesh2D::Register() const
{
    Register(Name());
}

void DTSeriesStructuredMesh2D::Register(string name) const
{
    DTRegisteredSeries<DTSeriesStructuredMesh2D,map<string,DTSeriesStructuredMesh2D>::const_iterator>(DTGlobalSeriesStructuredMesh2D,name,"DTSeriesStructuredMesh2D",this);
}

void DTSeriesStructuredMesh2D::pregistered(void)
{
    DTPrintRegisteredSeries<DTSeriesStructuredMesh2D,map<string,DTSeriesStructuredMesh2D>::const_iterator>(DTGlobalSeriesStructuredMesh2D);
}

bool DTSeriesStructuredMesh2D::HasBeenRegistered(string name)
{
    return DTHasRegisteredSeries<DTSeriesStructuredMesh2D,map<string,DTSeriesStructuredMesh2D>::const_iterator>(DTGlobalSeriesStructuredMesh2D,name);
}

DTSeriesStructuredMesh2D DTSeriesStructuredMesh2D::ByName(string name)
{
    return DTFindRegisteredSeries<DTSeriesStructuredMesh2D,map<string,DTSeriesStructuredMesh2D>::const_iterator>(DTGlobalSeriesStructuredMesh2D,name,"DTSeriesStructuredMesh2D");
}

void DTSeriesStructuredMesh2D::Add(DTStructuredMesh2D v,double time)
{
    if (SharedSave(time)==false) return;

    std::string name = baseName();
    if (v.Grid()==lastGrid && !lastGrid.IsEmpty()) {
        // Where to get the grid.
        Write(DataFile(),name+"_g",nameForLastGrid);
    }
    else {
        Write(DataFile(),name+"_g",v.Grid());
        lastGrid = v.Grid();
        nameForLastGrid = name+"_g";
    }
    Write(DataFile(),baseName(),v);

    AddToBBox(BoundingBox(v));
    AddToValueRange(ValueRange(v));
    DataFile().Flush();
}

DTStructuredMesh2D DTSeriesStructuredMesh2D::Get(double time) const
{
    int k = IndexForTime(time);
    if (k<0) return DTStructuredMesh2D();
    DTStructuredMesh2D toReturn;
    Read(DataFile(),baseName(k),toReturn);
    return toReturn;
}

void Read(const DTDataStorage &input,const std::string &name,DTSeriesStructuredMesh2D &toReturn)
{
    string variableName;
    DTMutablePointer<DTDataStorage> storage = DTSeries::ReferencedDataFile(input,name,variableName);
    toReturn = DTSeriesStructuredMesh2D(*storage,variableName);
}
