// Part of DTSource. Copyright 2004-2017. David Adalsteinsson.
// see http://www.visualdatatools.com/DTSource/license.html for more information.

#include "DTSeriesMesh2DGrid.h"

#include "DTMesh2DGrid.h"
#include "DTSeriesTemplates.h"

map<string,DTSeriesMesh2DGrid> DTGlobalSeriesMesh2DGrid;

void DTSeriesMesh2DGrid::Register() const
{
    Register(Name());
}

void DTSeriesMesh2DGrid::Register(string name) const
{
    DTRegisteredSeries<DTSeriesMesh2DGrid,map<string,DTSeriesMesh2DGrid>::const_iterator>(DTGlobalSeriesMesh2DGrid,name,"DTSeriesMesh2DGrid",this);
}

void DTSeriesMesh2DGrid::pregistered(void)
{
    DTPrintRegisteredSeries<DTSeriesMesh2DGrid,map<string,DTSeriesMesh2DGrid>::const_iterator>(DTGlobalSeriesMesh2DGrid);
}

bool DTSeriesMesh2DGrid::HasBeenRegistered(string name)
{
    return DTHasRegisteredSeries<DTSeriesMesh2DGrid,map<string,DTSeriesMesh2DGrid>::const_iterator>(DTGlobalSeriesMesh2DGrid,name);
}

DTSeriesMesh2DGrid DTSeriesMesh2DGrid::ByName(string name)
{
    return DTFindRegisteredSeries<DTSeriesMesh2DGrid,map<string,DTSeriesMesh2DGrid>::const_iterator>(DTGlobalSeriesMesh2DGrid,name,"DTSeriesMesh2DGrid");
}

void DTSeriesMesh2DGrid::Add(DTMesh2DGrid v,double time)
{
    if (SharedSave(time)==false) return;

    Write(DataFile(),baseName(),v);
    AddToBBox(BoundingBox(v));
    DataFile().Flush();
}

DTMesh2DGrid DTSeriesMesh2DGrid::Get(double time) const
{
    int k = IndexForTime(time);
    if (k<0) return DTMesh2DGrid();
    DTMesh2DGrid toReturn;
    Read(DataFile(),baseName(k),toReturn);
    return toReturn;
}

void Read(const DTDataStorage &input,const std::string &name,DTSeriesMesh2DGrid &toReturn)
{
    string variableName;
    DTMutablePointer<DTDataStorage> storage = DTSeries::ReferencedDataFile(input,name,variableName);
    toReturn = DTSeriesMesh2DGrid(*storage,variableName);
}
