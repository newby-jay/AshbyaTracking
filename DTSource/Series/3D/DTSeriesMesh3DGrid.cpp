// Part of DTSource. Copyright 2004-2017. David Adalsteinsson.
// see http://www.visualdatatools.com/DTSource/license.html for more information.

#include "DTSeriesMesh3DGrid.h"

#include "DTMesh3DGrid.h"
#include "DTSeriesTemplates.h"

map<string,DTSeriesMesh3DGrid> DTGlobalSeriesMesh3DGrid;

void DTSeriesMesh3DGrid::Register() const
{
    Register(Name());
}

void DTSeriesMesh3DGrid::Register(string name) const
{
    DTRegisteredSeries<DTSeriesMesh3DGrid,map<string,DTSeriesMesh3DGrid>::const_iterator>(DTGlobalSeriesMesh3DGrid,name,"DTSeriesMesh3DGrid",this);
}

void DTSeriesMesh3DGrid::pregistered(void)
{
    DTPrintRegisteredSeries<DTSeriesMesh3DGrid,map<string,DTSeriesMesh3DGrid>::const_iterator>(DTGlobalSeriesMesh3DGrid);
}

bool DTSeriesMesh3DGrid::HasBeenRegistered(string name)
{
    return DTHasRegisteredSeries<DTSeriesMesh3DGrid,map<string,DTSeriesMesh3DGrid>::const_iterator>(DTGlobalSeriesMesh3DGrid,name);
}

DTSeriesMesh3DGrid DTSeriesMesh3DGrid::ByName(string name)
{
    return DTFindRegisteredSeries<DTSeriesMesh3DGrid,map<string,DTSeriesMesh3DGrid>::const_iterator>(DTGlobalSeriesMesh3DGrid,name,"DTSeriesMesh3DGrid");
}

void DTSeriesMesh3DGrid::Add(DTMesh3DGrid v,double time)
{
    if (SharedSave(time)==false) return;

    Write(DataFile(),baseName(),v);
    AddToBBox(BoundingBox(v));
    DataFile().Flush();
}

DTMesh3DGrid DTSeriesMesh3DGrid::Get(double time) const
{
    int k = IndexForTime(time);
    if (k<0) return DTMesh3DGrid();
    DTMesh3DGrid toReturn;
    Read(DataFile(),baseName(k),toReturn);
    return toReturn;
}

DTMesh3DGrid DTSeriesMesh3DGrid::operator()(int index) const
{
    if (index<0 || index>=HowManySaved()) {
        DTErrorOutOfRange("DTSeriesMesh3DGrid(i)",index,HowManySaved());
        return DTMesh3DGrid();
    }
    DTMesh3DGrid toReturn;
    Read(DataFile(),baseName(index),toReturn);
    return toReturn;
}

void Read(const DTDataStorage &input,const std::string &name,DTSeriesMesh3DGrid &toReturn)
{
    string variableName;
    DTMutablePointer<DTDataStorage> storage = DTSeries::ReferencedDataFile(input,name,variableName);
    toReturn = DTSeriesMesh3DGrid(*storage,variableName);
}
