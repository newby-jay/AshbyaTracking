// Part of DTSource. Copyright 2004-2017. David Adalsteinsson.
// see http://www.visualdatatools.com/DTSource/license.html for more information.

#include "DTSeriesPath2D.h"

#include "DTPath2D.h"
#include "DTSeriesTemplates.h"

map<string,DTSeriesPath2D> DTGlobalSeriesPath2D;

void DTSeriesPath2D::Register() const
{
    Register(Name());
}

void DTSeriesPath2D::Register(string name) const
{
    DTRegisteredSeries<DTSeriesPath2D,map<string,DTSeriesPath2D>::const_iterator>(DTGlobalSeriesPath2D,name,"DTSeriesPath2D",this);
}

void DTSeriesPath2D::pregistered(void)
{
    DTPrintRegisteredSeries<DTSeriesPath2D,map<string,DTSeriesPath2D>::const_iterator>(DTGlobalSeriesPath2D);
}

bool DTSeriesPath2D::HasBeenRegistered(string name)
{
    return DTHasRegisteredSeries<DTSeriesPath2D,map<string,DTSeriesPath2D>::const_iterator>(DTGlobalSeriesPath2D,name);
}

DTSeriesPath2D DTSeriesPath2D::ByName(string name)
{
    return DTFindRegisteredSeries<DTSeriesPath2D,map<string,DTSeriesPath2D>::const_iterator>(DTGlobalSeriesPath2D,name,"DTSeriesPath2D");
}

void DTSeriesPath2D::Add(DTPath2D v,double time)
{
    if (SharedSave(time)==false) return;

    Write(DataFile(),baseName(),v);
    AddToBBox(BoundingBox(v));
    DataFile().Flush();
}

DTPath2D DTSeriesPath2D::Get(double time) const
{
    int k = IndexForTime(time);
    if (k<0) return DTPath2D();
    DTPath2D toReturn;
    Read(DataFile(),baseName(k),toReturn);
    return toReturn;
}

void Read(const DTDataStorage &input,const std::string &name,DTSeriesPath2D &toReturn)
{
    string variableName;
    DTMutablePointer<DTDataStorage> storage = DTSeries::ReferencedDataFile(input,name,variableName);
    toReturn = DTSeriesPath2D(*storage,variableName);
}
