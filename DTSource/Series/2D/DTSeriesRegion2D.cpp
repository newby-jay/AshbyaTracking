// Part of DTSource. Copyright 2004-2017. David Adalsteinsson.
// see http://www.visualdatatools.com/DTSource/license.html for more information.

#include "DTSeriesRegion2D.h"

#include "DTRegion2D.h"
#include "DTSeriesTemplates.h"

map<string,DTSeriesRegion2D> DTGlobalSeriesRegion2D;

void DTSeriesRegion2D::Register() const
{
    Register(Name());
}

void DTSeriesRegion2D::Register(string name) const
{
    DTRegisteredSeries<DTSeriesRegion2D,map<string,DTSeriesRegion2D>::const_iterator>(DTGlobalSeriesRegion2D,name,"DTSeriesRegion2D",this);
}

void DTSeriesRegion2D::pregistered(void)
{
    DTPrintRegisteredSeries<DTSeriesRegion2D,map<string,DTSeriesRegion2D>::const_iterator>(DTGlobalSeriesRegion2D);
}

bool DTSeriesRegion2D::HasBeenRegistered(string name)
{
    return DTHasRegisteredSeries<DTSeriesRegion2D,map<string,DTSeriesRegion2D>::const_iterator>(DTGlobalSeriesRegion2D,name);
}

DTSeriesRegion2D DTSeriesRegion2D::ByName(string name)
{
    return DTFindRegisteredSeries<DTSeriesRegion2D,map<string,DTSeriesRegion2D>::const_iterator>(DTGlobalSeriesRegion2D,name,"DTSeriesRegion2D");
}

void DTSeriesRegion2D::Add(DTRegion2D v,double time)
{
    if (SharedSave(time)==false) return;

    Write(DataFile(),baseName(),v);
    AddToBBox(v);
    DataFile().Flush();
}

DTRegion2D DTSeriesRegion2D::Get(double time) const
{
    int k = IndexForTime(time);
    if (k<0) return DTRegion2D();
    DTRegion2D toReturn;
    Read(DataFile(),baseName(k),toReturn);
    return toReturn;
}

void Read(const DTDataStorage &input,const std::string &name,DTSeriesRegion2D &toReturn)
{
    string variableName;
    DTMutablePointer<DTDataStorage> storage = DTSeries::ReferencedDataFile(input,name,variableName);
    toReturn = DTSeriesRegion2D(*storage,variableName);
}
