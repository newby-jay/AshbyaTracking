// Part of DTSource. Copyright 2004-2017. David Adalsteinsson.
// see http://www.visualdatatools.com/DTSource/license.html for more information.

#include "DTSeriesFunction1D.h"

#include "DTFunction1D.h"
#include "DTSeriesTemplates.h"

map<string,DTSeriesFunction1D> DTGlobalSeriesFunction1D;

void DTSeriesFunction1D::Register() const
{
    Register(Name());
}

void DTSeriesFunction1D::Register(string name) const
{
    DTRegisteredSeries<DTSeriesFunction1D,map<string,DTSeriesFunction1D>::const_iterator>(DTGlobalSeriesFunction1D,name,"DTSeriesFunction1D",this);
}

void DTSeriesFunction1D::pregistered(void)
{
    DTPrintRegisteredSeries<DTSeriesFunction1D,map<string,DTSeriesFunction1D>::const_iterator>(DTGlobalSeriesFunction1D);
}

bool DTSeriesFunction1D::HasBeenRegistered(string name)
{
    return DTHasRegisteredSeries<DTSeriesFunction1D,map<string,DTSeriesFunction1D>::const_iterator>(DTGlobalSeriesFunction1D,name);
}

DTSeriesFunction1D DTSeriesFunction1D::ByName(string name)
{
    return DTFindRegisteredSeries<DTSeriesFunction1D,map<string,DTSeriesFunction1D>::const_iterator>(DTGlobalSeriesFunction1D,name,"DTSeriesFunction1D");
}

void DTSeriesFunction1D::Add(DTFunction1D v,double time)
{
    if (SharedSave(time)==false) return;
    Write(DataFile(),baseName(),v);
    DataFile().Flush();
}

DTFunction1D DTSeriesFunction1D::Get(double time) const
{
    int k = IndexForTime(time);
    if (k<0) return DTFunction1D();
    DTFunction1D toReturn;
    Read(DataFile(),baseName(k),toReturn);
    return toReturn;
}

void Read(const DTDataStorage &input,const std::string &name,DTSeriesFunction1D &toReturn)
{
    string variableName;
    DTMutablePointer<DTDataStorage> storage = DTSeries::ReferencedDataFile(input,name,variableName);
    toReturn = DTSeriesFunction1D(*storage,input.ReadString(name));
}
