// Part of DTSource. Copyright 2004-2017. David Adalsteinsson.
// see http://www.visualdatatools.com/DTSource/license.html for more information.

#include "DTSeriesPoint2DValue.h"

#include "DTPoint2DValue.h"
#include "DTSeriesTemplates.h"

map<string,DTSeriesPoint2DValue> DTGlobalSeriesPoint2DValue;

void DTSeriesPoint2DValue::Register() const
{
    Register(Name());
}

void DTSeriesPoint2DValue::Register(string name) const
{
    DTRegisteredSeries<DTSeriesPoint2DValue,map<string,DTSeriesPoint2DValue>::const_iterator>(DTGlobalSeriesPoint2DValue,name,"DTSeriesPoint2DValue",this);
}

void DTSeriesPoint2DValue::pregistered(void)
{
    DTPrintRegisteredSeries<DTSeriesPoint2DValue,map<string,DTSeriesPoint2DValue>::const_iterator>(DTGlobalSeriesPoint2DValue);
}

bool DTSeriesPoint2DValue::HasBeenRegistered(string name)
{
    return DTHasRegisteredSeries<DTSeriesPoint2DValue,map<string,DTSeriesPoint2DValue>::const_iterator>(DTGlobalSeriesPoint2DValue,name);
}

DTSeriesPoint2DValue DTSeriesPoint2DValue::ByName(string name)
{
    return DTFindRegisteredSeries<DTSeriesPoint2DValue,map<string,DTSeriesPoint2DValue>::const_iterator>(DTGlobalSeriesPoint2DValue,name,"DTSeriesPoint2DValue");
}

void DTSeriesPoint2DValue::Add(DTPoint2DValue v,double time)
{
    if (SharedSave(time)==false) return;

    Write(DataFile(),baseName(),v);
    AddToValueRange(DTRegion1D(v.value,v.value));
    AddToBBox(DTRegion2D(v.point,v.point));
    DataFile().Flush();
}

DTPoint2DValue DTSeriesPoint2DValue::Get(double time) const
{
    int k = IndexForTime(time);
    if (k<0) return DTPoint2DValue();
    DTPoint2DValue toReturn;
    Read(DataFile(),baseName(k),toReturn);
    return toReturn;
}

void Read(const DTDataStorage &input,const std::string &name,DTSeriesPoint2DValue &toReturn)
{
    string variableName;
    DTMutablePointer<DTDataStorage> storage = DTSeries::ReferencedDataFile(input,name,variableName);
    toReturn = DTSeriesPoint2DValue(*storage,variableName);
}
