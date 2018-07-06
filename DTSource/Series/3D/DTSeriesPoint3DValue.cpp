// Part of DTSource. Copyright 2004-2017. David Adalsteinsson.
// see http://www.visualdatatools.com/DTSource/license.html for more information.

#include "DTSeriesPoint3DValue.h"

#include "DTPoint3DValue.h"
#include "DTSeriesTemplates.h"

map<string,DTSeriesPoint3DValue> DTGlobalSeriesPoint3DValue;

void DTSeriesPoint3DValue::Register() const
{
    Register(Name());
}

void DTSeriesPoint3DValue::Register(string name) const
{
    DTRegisteredSeries<DTSeriesPoint3DValue,map<string,DTSeriesPoint3DValue>::const_iterator>(DTGlobalSeriesPoint3DValue,name,"DTSeriesPoint3DValue",this);
}

void DTSeriesPoint3DValue::pregistered(void)
{
    DTPrintRegisteredSeries<DTSeriesPoint3DValue,map<string,DTSeriesPoint3DValue>::const_iterator>(DTGlobalSeriesPoint3DValue);
}

bool DTSeriesPoint3DValue::HasBeenRegistered(string name)
{
    return DTHasRegisteredSeries<DTSeriesPoint3DValue,map<string,DTSeriesPoint3DValue>::const_iterator>(DTGlobalSeriesPoint3DValue,name);
}

DTSeriesPoint3DValue DTSeriesPoint3DValue::ByName(string name)
{
    return DTFindRegisteredSeries<DTSeriesPoint3DValue,map<string,DTSeriesPoint3DValue>::const_iterator>(DTGlobalSeriesPoint3DValue,name,"DTSeriesPoint3DValue");
}

void DTSeriesPoint3DValue::Add(DTPoint3DValue v,double time)
{
    if (SharedSave(time)==false) return;

    Write(DataFile(),baseName(),v);
    AddToValueRange(DTRegion1D(v.value,v.value));
    AddToBBox(DTRegion3D(v.point,v.point));
    DataFile().Flush();
}

DTPoint3DValue DTSeriesPoint3DValue::Get(double time) const
{
    int k = IndexForTime(time);
    if (k<0) return DTPoint3DValue();
    DTPoint3DValue toReturn;
    Read(DataFile(),baseName(k),toReturn);
    return toReturn;
}

DTPoint3DValue DTSeriesPoint3DValue::operator()(int index) const
{
    if (index<0 || index>=HowManySaved()) {
        DTErrorOutOfRange("DTSeriesPoint3DValue(i)",index,HowManySaved());
        return DTPoint3DValue();
    }
    DTPoint3DValue toReturn;
    Read(DataFile(),baseName(index),toReturn);
    return toReturn;
}

void Read(const DTDataStorage &input,const std::string &name,DTSeriesPoint3DValue &toReturn)
{
    string variableName;
    DTMutablePointer<DTDataStorage> storage = DTSeries::ReferencedDataFile(input,name,variableName);
    toReturn = DTSeriesPoint3DValue(*storage,variableName);
}
