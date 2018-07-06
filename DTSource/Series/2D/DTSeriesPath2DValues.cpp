// Part of DTSource. Copyright 2004-2017. David Adalsteinsson.
// see http://www.visualdatatools.com/DTSource/license.html for more information.

#include "DTSeriesPath2DValues.h"

#include "DTPath2DValues.h"
#include "DTSeriesTemplates.h"

map<string,DTSeriesPath2DValues> DTGlobalSeriesPath2DValues;

void DTSeriesPath2DValues::Register() const
{
    Register(Name());
}

void DTSeriesPath2DValues::Register(string name) const
{
    DTRegisteredSeries<DTSeriesPath2DValues,map<string,DTSeriesPath2DValues>::const_iterator>(DTGlobalSeriesPath2DValues,name,"DTSeriesPath2DValues",this);
}

void DTSeriesPath2DValues::pregistered(void)
{
    DTPrintRegisteredSeries<DTSeriesPath2DValues,map<string,DTSeriesPath2DValues>::const_iterator>(DTGlobalSeriesPath2DValues);
}

bool DTSeriesPath2DValues::HasBeenRegistered(string name)
{
    return DTHasRegisteredSeries<DTSeriesPath2DValues,map<string,DTSeriesPath2DValues>::const_iterator>(DTGlobalSeriesPath2DValues,name);
}

DTSeriesPath2DValues DTSeriesPath2DValues::ByName(string name)
{
    return DTFindRegisteredSeries<DTSeriesPath2DValues,map<string,DTSeriesPath2DValues>::const_iterator>(DTGlobalSeriesPath2DValues,name,"DTSeriesPath2DValues");
}

void DTSeriesPath2DValues::Add(DTPath2DValues v,double time)
{
    if (SharedSave(time)==false) return;

    Write(DataFile(),baseName(),v);
    AddToBBox(BoundingBox(v));
    AddToValueRange(ValueRange(v));
    DataFile().Flush();
}

DTPath2DValues DTSeriesPath2DValues::Get(double time) const
{
    int k = IndexForTime(time);
    if (k<0) return DTPath2DValues();
    DTPath2DValues toReturn;
    Read(DataFile(),baseName(k),toReturn);
    return toReturn;
}

void Read(const DTDataStorage &input,const std::string &name,DTSeriesPath2DValues &toReturn)
{
    string variableName;
    DTMutablePointer<DTDataStorage> storage = DTSeries::ReferencedDataFile(input,name,variableName);
    toReturn = DTSeriesPath2DValues(*storage,variableName);
}
