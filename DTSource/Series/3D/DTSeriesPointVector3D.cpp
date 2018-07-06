// Part of DTSource. Copyright 2004-2017. David Adalsteinsson.
// see http://www.visualdatatools.com/DTSource/license.html for more information.

#include "DTSeriesPointVector3D.h"

#include "DTPointVector3D.h"
#include "DTSeriesTemplates.h"

map<string,DTSeriesPointVector3D> DTGlobalSeriesPointVector3D;

void DTSeriesPointVector3D::Register() const
{
    Register(Name());
}

void DTSeriesPointVector3D::Register(string name) const
{
    DTRegisteredSeries<DTSeriesPointVector3D,map<string,DTSeriesPointVector3D>::const_iterator>(DTGlobalSeriesPointVector3D,name,"DTSeriesPointVector3D",this);
}

void DTSeriesPointVector3D::pregistered(void)
{
    DTPrintRegisteredSeries<DTSeriesPointVector3D,map<string,DTSeriesPointVector3D>::const_iterator>(DTGlobalSeriesPointVector3D);
}

bool DTSeriesPointVector3D::HasBeenRegistered(string name)
{
    return DTHasRegisteredSeries<DTSeriesPointVector3D,map<string,DTSeriesPointVector3D>::const_iterator>(DTGlobalSeriesPointVector3D,name);
}

DTSeriesPointVector3D DTSeriesPointVector3D::ByName(string name)
{
    return DTFindRegisteredSeries<DTSeriesPointVector3D,map<string,DTSeriesPointVector3D>::const_iterator>(DTGlobalSeriesPointVector3D,name,"DTSeriesPointVector3D");
}

void DTSeriesPointVector3D::Add(DTPointVector3D v,double time)
{
    if (SharedSave(time)==false) return;
    
    Write(DataFile(),baseName(),v);
    AddToBBox(DTRegion3D(v.point,v.point));
    DataFile().Flush();
}

DTPointVector3D DTSeriesPointVector3D::Get(double time) const
{
    int k = IndexForTime(time);
    if (k<0) return DTPointVector3D();
    DTPointVector3D toReturn;
    Read(DataFile(),baseName(k),toReturn);
    return toReturn;
}

DTPointVector3D DTSeriesPointVector3D::operator()(int index) const
{
    if (index<0 || index>=HowManySaved()) {
        DTErrorOutOfRange("DTSeriesPointVector3D(i)",index,HowManySaved());
        return DTPointVector3D();
    }
    DTPointVector3D toReturn;
    Read(DataFile(),baseName(index),toReturn);
    return toReturn;
}

void Read(const DTDataStorage &input,const std::string &name,DTSeriesPointVector3D &toReturn)
{
    string variableName;
    DTMutablePointer<DTDataStorage> storage = DTSeries::ReferencedDataFile(input,name,variableName);
    toReturn = DTSeriesPointVector3D(*storage,variableName);
}
