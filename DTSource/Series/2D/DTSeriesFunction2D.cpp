// Part of DTSource. Copyright 2004-2017. David Adalsteinsson.
// see http://www.visualdatatools.com/DTSource/license.html for more information.

#include "DTSeriesFunction2D.h"

#include "DTFunction2D.h"
#include "DTSeriesTemplates.h"

map<string,DTSeriesFunction2D> DTGlobalSeriesFunction2D;

void DTSeriesFunction2D ::Register() const
{
    Register(Name());
}

void DTSeriesFunction2D ::Register(string name) const
{
    DTRegisteredSeries<DTSeriesFunction2D ,map<string,DTSeriesFunction2D >::const_iterator>(DTGlobalSeriesFunction2D,name,"DTSeriesFunction2D ",this);
}

void DTSeriesFunction2D ::pregistered(void)
{
    DTPrintRegisteredSeries<DTSeriesFunction2D ,map<string,DTSeriesFunction2D >::const_iterator>(DTGlobalSeriesFunction2D);
}

bool DTSeriesFunction2D ::HasBeenRegistered(string name)
{
    return DTHasRegisteredSeries<DTSeriesFunction2D ,map<string,DTSeriesFunction2D >::const_iterator>(DTGlobalSeriesFunction2D,name);
}

DTSeriesFunction2D  DTSeriesFunction2D ::ByName(string name)
{
    return DTFindRegisteredSeries<DTSeriesFunction2D ,map<string,DTSeriesFunction2D >::const_iterator>(DTGlobalSeriesFunction2D,name,"DTSeriesFunction2D ");
}

void DTSeriesFunction2D::Add(DTFunction2D v,double time)
{
    if (SharedSave(time)==false) return;
    Write(DataFile(),baseName(),v);
    DataFile().Flush();
}

DTFunction2D DTSeriesFunction2D::Get(double time) const
{
    int k = IndexForTime(time);
    if (k<0) return DTFunction2D();
    DTFunction2D toReturn;
    Read(DataFile(),baseName(k),toReturn);
    return toReturn;
}

void Read(const DTDataStorage &input,const std::string &name,DTSeriesFunction2D &toReturn)
{
    string variableName;
    DTMutablePointer<DTDataStorage> storage = DTSeries::ReferencedDataFile(input,name,variableName);
    toReturn = DTSeriesFunction2D(*storage,variableName);
}
