// Part of DTSource. Copyright 2004-2017. David Adalsteinsson.
// see http://www.visualdatatools.com/DTSource/license.html for more information.

#include "DTSeriesRealNumber.h"
#include "DTError.h"
#include "DTSeriesTemplates.h"

map<string,DTSeriesRealNumber> DTGlobalSeriesRealNumber;

void DTSeriesRealNumber::Register() const
{
    Register(Name());
}

void DTSeriesRealNumber::Register(string name) const
{
    DTRegisteredSeries<DTSeriesRealNumber,map<string,DTSeriesRealNumber>::const_iterator>(DTGlobalSeriesRealNumber,name,"DTSeriesRealNumber",this);
}

void DTSeriesRealNumber::pregistered(void)
{
    DTPrintRegisteredSeries<DTSeriesRealNumber,map<string,DTSeriesRealNumber>::const_iterator>(DTGlobalSeriesRealNumber);
}

bool DTSeriesRealNumber::HasBeenRegistered(string name)
{
    return DTHasRegisteredSeries<DTSeriesRealNumber,map<string,DTSeriesRealNumber>::const_iterator>(DTGlobalSeriesRealNumber,name);
}

DTSeriesRealNumber DTSeriesRealNumber::ByName(string name)
{
    return DTFindRegisteredSeries<DTSeriesRealNumber,map<string,DTSeriesRealNumber>::const_iterator>(DTGlobalSeriesRealNumber,name,"DTSeriesRealNumber");
}

void DTSeriesRealNumber::Add(double v,double time)
{
    if (SharedSave(time)==false) return;

    DataFile().Save(v,baseName());
    AddToValueRange(DTRegion1D(v,v));
    DataFile().Flush();
}

double DTSeriesRealNumber::Get(double time) const
{
    int k = IndexForTime(time);
    if (k<0) return 0.0;
    return DataFile().ReadNumber(baseName(k));
}

void Read(const DTDataStorage &input,const std::string &name,DTSeriesRealNumber &toReturn)
{
    string variableName;
    DTMutablePointer<DTDataStorage> storage = DTSeries::ReferencedDataFile(input,name,variableName);
    toReturn = DTSeriesRealNumber(*storage,variableName);
}
