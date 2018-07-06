// Part of DTSource. Copyright 2004-2017. David Adalsteinsson.
// see http://www.visualdatatools.com/DTSource/license.html for more information.

#include "DTSeriesPointValueCollection3D.h"

#include "DTPointValueCollection3D.h"
#include "DTRegion3D.h"
#include "DTSeriesTemplates.h"

map<string,DTSeriesPointValueCollection3D> DTGlobalSeriesPointValueCollection3D;

void DTSeriesPointValueCollection3D::Register() const
{
    Register(Name());
}

void DTSeriesPointValueCollection3D::Register(string name) const
{
    DTRegisteredSeries<DTSeriesPointValueCollection3D,map<string,DTSeriesPointValueCollection3D>::const_iterator>(DTGlobalSeriesPointValueCollection3D,name,"DTSeriesPointValueCollection3D",this);
}

void DTSeriesPointValueCollection3D::pregistered(void)
{
    DTPrintRegisteredSeries<DTSeriesPointValueCollection3D,map<string,DTSeriesPointValueCollection3D>::const_iterator>(DTGlobalSeriesPointValueCollection3D);
}

bool DTSeriesPointValueCollection3D::HasBeenRegistered(string name)
{
    return DTHasRegisteredSeries<DTSeriesPointValueCollection3D,map<string,DTSeriesPointValueCollection3D>::const_iterator>(DTGlobalSeriesPointValueCollection3D,name);
}

DTSeriesPointValueCollection3D DTSeriesPointValueCollection3D::ByName(string name)
{
    return DTFindRegisteredSeries<DTSeriesPointValueCollection3D,map<string,DTSeriesPointValueCollection3D>::const_iterator>(DTGlobalSeriesPointValueCollection3D,name,"DTSeriesPointValueCollection3D");
}

void DTSeriesPointValueCollection3D::Add(DTPointValueCollection3D v,double time)
{
    if (SharedSave(time)==false) return;
    AddToBBox(BoundingBox(v));
    if (_saveDoubleAsFloat)
        WriteFloat(DataFile(),baseName(),v,saveInfo);
    else
        Write(DataFile(),baseName(),v,saveInfo);
    DataFile().Flush();
}

DTPointValueCollection3D DTSeriesPointValueCollection3D::Get(double time) const
{
    int k = IndexForTime(time);
    if (k<0) return DTPointValueCollection3D();
    DTPointValueCollection3D toReturn;
    Read(DataFile(),baseName(k),toReturn,saveInfo);
    return toReturn;
}

DTPointValueCollection3D DTSeriesPointValueCollection3D::operator()(int index) const
{
    if (index<0 || index>=HowManySaved()) {
        DTErrorOutOfRange("DTPointValueCollection3D(i)",index,HowManySaved());
        return DTPointValueCollection3D();
    }
    DTPointValueCollection3D toReturn;
    Read(DataFile(),baseName(index),toReturn);
    return toReturn;
}

void Read(const DTDataStorage &input,const std::string &name,DTSeriesPointValueCollection3D &toReturn)
{
    string variableName;
    DTMutablePointer<DTDataStorage> storage = DTSeries::ReferencedDataFile(input,name,variableName);
    toReturn = DTSeriesPointValueCollection3D(*storage,variableName);
}
