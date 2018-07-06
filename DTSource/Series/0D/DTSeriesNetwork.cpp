// Part of DTSource. Copyright 2004-2017. David Adalsteinsson.
// see http://www.visualdatatools.com/DTSource/license.html for more information.

#include "DTSeriesNetwork.h"

#include "DTNetwork.h"
#include "DTSeriesTemplates.h"

map<string,DTSeriesNetwork> DTGlobalSeriesNetwork;

void DTSeriesNetwork::Register() const
{
    Register(Name());
}

void DTSeriesNetwork::Register(string name) const
{
    DTRegisteredSeries<DTSeriesNetwork,map<string,DTSeriesNetwork>::const_iterator>(DTGlobalSeriesNetwork,name,"DTSeriesNetwork",this);
}

void DTSeriesNetwork::pregistered(void)
{
    DTPrintRegisteredSeries<DTSeriesNetwork,map<string,DTSeriesNetwork>::const_iterator>(DTGlobalSeriesNetwork);
}

bool DTSeriesNetwork::HasBeenRegistered(string name)
{
    return DTHasRegisteredSeries<DTSeriesNetwork,map<string,DTSeriesNetwork>::const_iterator>(DTGlobalSeriesNetwork,name);
}

DTSeriesNetwork DTSeriesNetwork::ByName(string name)
{
    return DTFindRegisteredSeries<DTSeriesNetwork,map<string,DTSeriesNetwork>::const_iterator>(DTGlobalSeriesNetwork,name,"DTSeriesNetwork");
}

void DTSeriesNetwork::Add(DTNetwork v,double time)
{
    if (SharedSave(time)==false) return;
    
    Write(DataFile(),baseName(),v);
    DataFile().Flush();
}

DTNetwork DTSeriesNetwork::Get(double time) const
{
    int k = IndexForTime(time);
    if (k<0) return DTNetwork();
    DTNetwork toReturn;
    Read(DataFile(),baseName(k),toReturn);
    return toReturn;
}

void Read(const DTDataStorage &input,const std::string &name,DTSeriesNetwork &toReturn)
{
    string variableName;
    DTMutablePointer<DTDataStorage> storage = DTSeries::ReferencedDataFile(input,name,variableName);
    toReturn = DTSeriesNetwork(*storage,variableName);
}
