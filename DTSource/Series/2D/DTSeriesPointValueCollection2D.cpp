// Part of DTSource. Copyright 2004-2017. David Adalsteinsson.
// see http://www.visualdatatools.com/DTSource/license.html for more information.

#include "DTSeriesPointValueCollection2D.h"

#include "DTPointValueCollection2D.h"
#include "DTSeriesTemplates.h"

map<string,DTSeriesPointValueCollection2D> DTGlobalSeriesPointValueCollection2D;

void DTSeriesPointValueCollection2D::Register() const
{
    Register(Name());
}

void DTSeriesPointValueCollection2D::Register(string name) const
{
    DTRegisteredSeries<DTSeriesPointValueCollection2D,map<string,DTSeriesPointValueCollection2D>::const_iterator>(DTGlobalSeriesPointValueCollection2D,name,"DTSeriesPointValueCollection2D",this);
}

void DTSeriesPointValueCollection2D::pregistered(void)
{
    DTPrintRegisteredSeries<DTSeriesPointValueCollection2D,map<string,DTSeriesPointValueCollection2D>::const_iterator>(DTGlobalSeriesPointValueCollection2D);
}

bool DTSeriesPointValueCollection2D::HasBeenRegistered(string name)
{
    return DTHasRegisteredSeries<DTSeriesPointValueCollection2D,map<string,DTSeriesPointValueCollection2D>::const_iterator>(DTGlobalSeriesPointValueCollection2D,name);
}

DTSeriesPointValueCollection2D DTSeriesPointValueCollection2D::ByName(string name)
{
    return DTFindRegisteredSeries<DTSeriesPointValueCollection2D,map<string,DTSeriesPointValueCollection2D>::const_iterator>(DTGlobalSeriesPointValueCollection2D,name,"DTSeriesPointValueCollection2D");
}

void DTSeriesPointValueCollection2D::Add(DTPointValueCollection2D v,double time)
{
    if (SharedSave(time)==false) return;
    
    Write(DataFile(),baseName(),v,saveInfo);
    AddToBBox(BoundingBox(v));
    DataFile().Flush();
}

DTPointValueCollection2D DTSeriesPointValueCollection2D::Get(double time) const
{
    int k = IndexForTime(time);
    if (k<0) return DTPointValueCollection2D();
    DTPointValueCollection2D toReturn;
    Read(DataFile(),baseName(k),toReturn,saveInfo);
    return toReturn;
}

DTPointValueCollection2D DTSeriesPointValueCollection2D::operator()(int index) const
{
    if (index<0 || index>=HowManySaved()) {
        DTErrorOutOfRange("DTSeriesPointValueCollection2D(i)",index,HowManySaved());
        return DTPointValueCollection2D();
    }
    DTPointValueCollection2D toReturn;
    Read(DataFile(),baseName(index),toReturn);
    return toReturn;
}

void Read(const DTDataStorage &input,const std::string &name,DTSeriesPointValueCollection2D &toReturn)
{
    string variableName;
    DTMutablePointer<DTDataStorage> storage = DTSeries::ReferencedDataFile(input,name,variableName);
    toReturn = DTSeriesPointValueCollection2D(*storage,variableName);
}
