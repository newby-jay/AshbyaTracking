// Part of DTSource. Copyright 2004-2017. David Adalsteinsson.
// see http://www.visualdatatools.com/DTSource/license.html for more information.

#include "DTSeriesArray.h"

#include "DTDoubleArray.h"
#include "DTError.h"
#include "DTSeriesTemplates.h"

map<string,DTSeriesArray> DTGlobalSeriesArray;

void DTSeriesArray::Register() const
{
    Register(Name());
}

void DTSeriesArray::Register(string name) const
{
    DTRegisteredSeries<DTSeriesArray,map<string,DTSeriesArray>::const_iterator>(DTGlobalSeriesArray,name,"DTSeriesArray",this);
}

void DTSeriesArray::pregistered(void)
{
    DTPrintRegisteredSeries<DTSeriesArray,map<string,DTSeriesArray>::const_iterator>(DTGlobalSeriesArray);
}

bool DTSeriesArray::HasBeenRegistered(string name)
{
    return DTHasRegisteredSeries<DTSeriesArray,map<string,DTSeriesArray>::const_iterator>(DTGlobalSeriesArray,name);
}

DTSeriesArray DTSeriesArray::ByName(string name)
{
    return DTFindRegisteredSeries<DTSeriesArray,map<string,DTSeriesArray>::const_iterator>(DTGlobalSeriesArray,name,"DTSeriesArray");
}

void DTSeriesArray::Add(const DTDoubleArray &v,double time)
{
    if (SharedSave(time)==false) return;

    Write(DataFile(),baseName(),v);
    AddToValueRange(ValueRange(v));
    DataFile().Flush();
}

void DTSeriesArray::Add(const DTFloatArray &v,double time)
{
    if (SharedSave(time)==false) return;
    
    Write(DataFile(),baseName(),v);
    AddToValueRange(ValueRange(v));
    DataFile().Flush();
}

void DTSeriesArray::Add(const DTIntArray &v,double time)
{
    if (SharedSave(time)==false) return;
    
    Write(DataFile(),baseName(),v);
    AddToValueRange(ValueRange(v));
    DataFile().Flush();
}

DTDoubleArray DTSeriesArray::Get(double time) const
{
    int k = IndexForTime(time);
    if (k<0) return DTDoubleArray();
    DTDoubleArray toReturn;
    Read(DataFile(),baseName(k),toReturn);
    return toReturn;
}

void Read(const DTDataStorage &input,const std::string &name,DTSeriesArray &toReturn)
{
    string variableName;
    DTMutablePointer<DTDataStorage> storage = DTSeries::ReferencedDataFile(input,name,variableName);
    toReturn = DTSeriesArray(*storage,input.ReadString(name));
}
