// Part of DTSource. Copyright 2004-2017. David Adalsteinsson.
// see http://www.visualdatatools.com/DTSource/license.html for more information.

#include "DTSeriesString.h"

#include "DTError.h"
#include "DTSeriesTemplates.h"

map<string,DTSeriesString> DTGlobalSeriesString;

void DTSeriesString::Register() const
{
    Register(Name());
}

void DTSeriesString::Register(string name) const
{
    DTRegisteredSeries<DTSeriesString,map<string,DTSeriesString>::const_iterator>(DTGlobalSeriesString,name,"DTSeriesString",this);
}

void DTSeriesString::pregistered(void)
{
    DTPrintRegisteredSeries<DTSeriesString,map<string,DTSeriesString>::const_iterator>(DTGlobalSeriesString);
}

bool DTSeriesString::HasBeenRegistered(string name)
{
    return DTHasRegisteredSeries<DTSeriesString,map<string,DTSeriesString>::const_iterator>(DTGlobalSeriesString,name);
}

DTSeriesString DTSeriesString::ByName(string name)
{
    return DTFindRegisteredSeries<DTSeriesString,map<string,DTSeriesString>::const_iterator>(DTGlobalSeriesString,name,"DTSeriesString");
}

void DTSeriesString::Add(string v,double time)
{
    if (SharedSave(time)==false) return;

    DataFile().Save(v,baseName());
    DataFile().Flush();
}

std::string DTSeriesString::Get(double time) const
{
    int k = IndexForTime(time);
    if (k<0) return std::string();
    return DataFile().ReadString(baseName(k));
}

void Read(const DTDataStorage &input,const std::string &name,DTSeriesString &toReturn)
{
    string variableName;
    DTMutablePointer<DTDataStorage> storage = DTSeries::ReferencedDataFile(input,name,variableName);
    toReturn = DTSeriesString(*storage,variableName);
}
