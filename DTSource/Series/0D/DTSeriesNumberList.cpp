// Part of DTSource. Copyright 2004-2017. David Adalsteinsson.
// see http://www.visualdatatools.com/DTSource/license.html for more information.

#include "DTSeriesNumberList.h"

#include "DTDoubleArray.h"
#include "DTFloatArray.h"
#include "DTIntArray.h"
#include "DTError.h"
#include "DTSeriesTemplates.h"

map<string,DTSeriesNumberList> DTGlobalSeriesNumberList;

void DTSeriesNumberList::Register() const
{
    Register(Name());
}

void DTSeriesNumberList::Register(string name) const
{
    DTRegisteredSeries<DTSeriesNumberList,map<string,DTSeriesNumberList>::const_iterator>(DTGlobalSeriesNumberList,name,"DTSeriesNumberList",this);
}

void DTSeriesNumberList::pregistered(void)
{
    DTPrintRegisteredSeries<DTSeriesNumberList,map<string,DTSeriesNumberList>::const_iterator>(DTGlobalSeriesNumberList);
}

bool DTSeriesNumberList::HasBeenRegistered(string name)
{
    return DTHasRegisteredSeries<DTSeriesNumberList,map<string,DTSeriesNumberList>::const_iterator>(DTGlobalSeriesNumberList,name);
}

DTSeriesNumberList DTSeriesNumberList::ByName(string name)
{
    return DTFindRegisteredSeries<DTSeriesNumberList,map<string,DTSeriesNumberList>::const_iterator>(DTGlobalSeriesNumberList,name,"DTSeriesNumberList");
}

void DTSeriesNumberList::Add(const DTDoubleArray &v,double time)
{
    if (SharedSave(time)==false) return;

    Write(DataFile(),baseName(),v);
    AddToValueRange(ValueRange(v));
    DataFile().Flush();
}

void DTSeriesNumberList::Add(const DTFloatArray &v,double time)
{
    if (SharedSave(time)==false) return;

    Write(DataFile(),baseName(),v);
    AddToValueRange(ValueRange(v));
    DataFile().Flush();
}

void DTSeriesNumberList::Add(const DTIntArray &v,double time)
{
    if (SharedSave(time)==false) return;

    Write(DataFile(),baseName(),v);
    AddToValueRange(ValueRange(v));
    DataFile().Flush();
}

DTDoubleArray DTSeriesNumberList::Get(double time) const
{
    int k = IndexForTime(time);
    if (k<0) return DTDoubleArray();

    DTDoubleArray toReturn;
    Read(DataFile(),baseName(k),toReturn);
    return toReturn;
}

DTIntArray DTSeriesNumberList::GetIntArray(double time) const
{
    int k = IndexForTime(time);
    if (k<0) return DTIntArray();
	
    DTIntArray toReturn;
    Read(DataFile(),baseName(k),toReturn);
    return toReturn;
}

void Read(const DTDataStorage &input,const std::string &name,DTSeriesNumberList &toReturn)
{
    string variableName;
    DTMutablePointer<DTDataStorage> storage = DTSeries::ReferencedDataFile(input,name,variableName);
    toReturn = DTSeriesNumberList(*storage,variableName);
}
