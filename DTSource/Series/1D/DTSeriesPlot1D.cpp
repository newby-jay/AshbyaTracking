// Part of DTSource. Copyright 2004-2017. David Adalsteinsson.
// see http://www.visualdatatools.com/DTSource/license.html for more information.

#include "DTSeriesPlot1D.h"

#include "DTPlot1D.h"
#include "DTSeriesTemplates.h"

map<string,DTSeriesPlot1D> DTGlobalSeriesPlot1D;

void DTSeriesPlot1D::Register() const
{
    Register(Name());
}

void DTSeriesPlot1D::Register(string name) const
{
    DTRegisteredSeries<DTSeriesPlot1D,map<string,DTSeriesPlot1D>::const_iterator>(DTGlobalSeriesPlot1D,name,"DTSeriesPlot1D",this);
}

void DTSeriesPlot1D::pregistered(void)
{
    DTPrintRegisteredSeries<DTSeriesPlot1D,map<string,DTSeriesPlot1D>::const_iterator>(DTGlobalSeriesPlot1D);
}

bool DTSeriesPlot1D::HasBeenRegistered(string name)
{
    return DTHasRegisteredSeries<DTSeriesPlot1D,map<string,DTSeriesPlot1D>::const_iterator>(DTGlobalSeriesPlot1D,name);
}

DTSeriesPlot1D DTSeriesPlot1D::ByName(string name)
{
    return DTFindRegisteredSeries<DTSeriesPlot1D,map<string,DTSeriesPlot1D>::const_iterator>(DTGlobalSeriesPlot1D,name,"DTSeriesPlot1D");
}

void DTSeriesPlot1D::Add(DTPlot1D v,double time)
{
    if (SharedSave(time)==false) return;
    Write(DataFile(),baseName(),v);
    AddToValueRange(ValueRange(v));
    DataFile().Flush();
}

DTPlot1D DTSeriesPlot1D::Get(double time) const
{
    int k = IndexForTime(time);
    if (k<0) return DTPlot1D();
    DTPlot1D toReturn;
    Read(DataFile(),baseName(k),toReturn);
    return toReturn;
}

void Read(const DTDataStorage &input,const std::string &name,DTSeriesPlot1D &toReturn)
{
    string variableName;
    DTMutablePointer<DTDataStorage> storage = DTSeries::ReferencedDataFile(input,name,variableName);
    toReturn = DTSeriesPlot1D(*storage,variableName);
}
