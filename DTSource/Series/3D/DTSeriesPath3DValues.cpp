// Part of DTSource. Copyright 2004-2017. David Adalsteinsson.
// see http://www.visualdatatools.com/DTSource/license.html for more information.

#include "DTSeriesPath3DValues.h"

#include "DTPath3DValues.h"
#include "DTSeriesTemplates.h"

map<string,DTSeriesPath3DValues> DTGlobalSeriesPath3DValues;

void DTSeriesPath3DValues::Register() const
{
    Register(Name());
}

void DTSeriesPath3DValues::Register(string name) const
{
    DTRegisteredSeries<DTSeriesPath3DValues,map<string,DTSeriesPath3DValues>::const_iterator>(DTGlobalSeriesPath3DValues,name,"DTSeriesPath3DValues",this);
}

void DTSeriesPath3DValues::pregistered(void)
{
    DTPrintRegisteredSeries<DTSeriesPath3DValues,map<string,DTSeriesPath3DValues>::const_iterator>(DTGlobalSeriesPath3DValues);
}

bool DTSeriesPath3DValues::HasBeenRegistered(string name)
{
    return DTHasRegisteredSeries<DTSeriesPath3DValues,map<string,DTSeriesPath3DValues>::const_iterator>(DTGlobalSeriesPath3DValues,name);
}

DTSeriesPath3DValues DTSeriesPath3DValues::ByName(string name)
{
    return DTFindRegisteredSeries<DTSeriesPath3DValues,map<string,DTSeriesPath3DValues>::const_iterator>(DTGlobalSeriesPath3DValues,name,"DTSeriesPath3DValues");
}

void DTSeriesPath3DValues::Add(DTPath3DValues v,double time)
{
    if (SharedSave(time)==false) return;

    Write(DataFile(),baseName(),v);
    AddToValueRange(ValueRange(v));
    DataFile().Flush();
}

DTPath3DValues DTSeriesPath3DValues::Get(double time) const
{
    int k = IndexForTime(time);
    if (k<0) return DTPath3DValues();
    DTPath3DValues toReturn;
    Read(DataFile(),baseName(k),toReturn);
    return toReturn;
}

DTPath3DValues DTSeriesPath3DValues::operator()(int index) const
{
    if (index<0 || index>=HowManySaved()) {
        DTErrorOutOfRange("DTSeriesPath3DValues(i)",index,HowManySaved());
        return DTPath3DValues();
    }
    DTPath3DValues toReturn;
    Read(DataFile(),baseName(index),toReturn);
    return toReturn;
}

void Read(const DTDataStorage &input,const std::string &name,DTSeriesPath3DValues &toReturn)
{
    string variableName;
    DTMutablePointer<DTDataStorage> storage = DTSeries::ReferencedDataFile(input,name,variableName);
    toReturn = DTSeriesPath3DValues(*storage,variableName);
}
