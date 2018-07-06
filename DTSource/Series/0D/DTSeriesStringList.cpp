// Part of DTSource. Copyright 2004-2017. David Adalsteinsson.
// see http://www.visualdatatools.com/DTSource/license.html for more information.

#include "DTSeriesStringList.h"

#include "DTStringList.h"
#include "DTError.h"
#include "DTSeriesTemplates.h"

map<string,DTSeriesStringList> DTGlobalSeriesStringList;

void DTSeriesStringList::Register() const
{
    Register(Name());
}

void DTSeriesStringList::Register(string name) const
{
    DTRegisteredSeries<DTSeriesStringList,map<string,DTSeriesStringList>::const_iterator>(DTGlobalSeriesStringList,name,"DTSeriesStringList",this);
}

void DTSeriesStringList::pregistered(void)
{
    DTPrintRegisteredSeries<DTSeriesStringList,map<string,DTSeriesStringList>::const_iterator>(DTGlobalSeriesStringList);
}

bool DTSeriesStringList::HasBeenRegistered(string name)
{
    return DTHasRegisteredSeries<DTSeriesStringList,map<string,DTSeriesStringList>::const_iterator>(DTGlobalSeriesStringList,name);
}

DTSeriesStringList DTSeriesStringList::ByName(string name)
{
    return DTFindRegisteredSeries<DTSeriesStringList,map<string,DTSeriesStringList>::const_iterator>(DTGlobalSeriesStringList,name,"DTSeriesStringList");
}

void DTSeriesStringList::Add(DTStringList v,double time)
{
    if (SharedSave(time)==false) return;
    Write(DataFile(),baseName(),v);
    DataFile().Flush();
}

DTStringList DTSeriesStringList::Get(double time) const
{
    int k = IndexForTime(time);
    if (k<0) return DTStringList();
    DTStringList toReturn;
    Read(DataFile(),baseName(k),toReturn);
    return toReturn;
}

void Read(const DTDataStorage &input,const std::string &name,DTSeriesStringList &toReturn)
{
    string variableName;
    DTMutablePointer<DTDataStorage> storage = DTSeries::ReferencedDataFile(input,name,variableName);
    toReturn = DTSeriesStringList(*storage,variableName);
}
