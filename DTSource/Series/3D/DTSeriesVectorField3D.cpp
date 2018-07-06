// Part of DTSource. Copyright 2006. David Adalsteinsson.
// see http://www.visualdatatools.com/DTSource/license.html for more information.

#include "DTSeriesVectorField3D.h"

#include "DTVectorField3D.h"
#include "DTSeriesTemplates.h"
#include "DTFloatArrayOperators.h"

map<string,DTSeriesVectorField3D> DTGlobalSeriesVectorField3D;

void DTSeriesVectorField3D::Register() const
{
    Register(Name());
}

void DTSeriesVectorField3D::Register(string name) const
{
    DTRegisteredSeries<DTSeriesVectorField3D,map<string,DTSeriesVectorField3D>::const_iterator>(DTGlobalSeriesVectorField3D,name,"DTSeriesVectorField3D",this);
}

void DTSeriesVectorField3D::pregistered(void)
{
    DTPrintRegisteredSeries<DTSeriesVectorField3D,map<string,DTSeriesVectorField3D>::const_iterator>(DTGlobalSeriesVectorField3D);
}

bool DTSeriesVectorField3D::HasBeenRegistered(string name)
{
    return DTHasRegisteredSeries<DTSeriesVectorField3D,map<string,DTSeriesVectorField3D>::const_iterator>(DTGlobalSeriesVectorField3D,name);
}

DTSeriesVectorField3D DTSeriesVectorField3D::ByName(string name)
{
    return DTFindRegisteredSeries<DTSeriesVectorField3D,map<string,DTSeriesVectorField3D>::const_iterator>(DTGlobalSeriesVectorField3D,name,"DTSeriesVectorField3D");
}

void DTSeriesVectorField3D::Add(DTVectorField3D v,double time)
{
    if (SharedSave(time)==false) return;
    
    Write(DataFile(),baseName(),v);
    
    AddToBBox(BoundingBox(v));
    DataFile().Flush();
}

DTVectorField3D DTSeriesVectorField3D::Get(double time) const
{
    int k = IndexForTime(time);
    if (k<0) return DTVectorField3D();
    DTVectorField3D toReturn;
    if (k==k1) {
        toReturn = v1;
    }
    else if (k==k2) {
        toReturn = v2;
    }
    else {
        Read(DataFile(),baseName(k),toReturn);
    }
    return toReturn;
}

DTVectorField3D DTSeriesVectorField3D::GetInterpolated(double time) const
{
    if (HowManySaved()==0) return DTVectorField3D();
    
    int before = IndexForTime(time);
    double timeBefore = TimeNumber(before);
    double timeAfter = timeBefore;
    DTVectorField3D toReturn;
    
    if (before<HowManySaved()-1) {
        timeAfter = TimeNumber(before+1);
    }
    if (timeBefore==timeAfter || DTSeries::TimeConsideredTheSame(time,timeBefore)) {
        // Read in only that time value.  First check if it is already cached.
        if (timeBefore==t1) {
            return v1;
        }
        else if (timeBefore==t2) {
            return v2;
        }
        else {
            Read(DataFile(),baseName(before),toReturn);
            // See which where to cache it.
            if (timeBefore>t2) {
                v1 = v2;
                t1 = t2;
                k1 = k2;
                v2 = toReturn;
                t2 = timeBefore;
                k2 = before;
            }
            else if (timeBefore<t1) {
                v2 = v1;
                t2 = t1;
                k2 = k1;
                v1 = toReturn;
                t1 = timeBefore;
                k1 = before;
            }
            else {
                // Between the two values.  Assume you are increasing time.
                v1 = toReturn;
                t1 = timeBefore;
                k1 = before;
            }
        }
    }
    else {
        // Need to read in value before and value after.
        if (timeBefore!=t1 || timeAfter!=t2) {
            if (timeAfter==t1) {
                t2 = t1;
                v2 = v1;
                k2 = k1;
                t1 = timeBefore;
                Read(DataFile(),baseName(before),v1);
                k1 = before;
            }
            else if (timeBefore==t2) {
                t1 = t2;
                v1 = v2;
                k1 = k2;
                t2 = timeAfter;
                Read(DataFile(),baseName(before+1),v2);
                k2 = before+1;
            }
            else {
                t1 = timeBefore;
                Read(DataFile(),baseName(before),v1);
                k1 = before;
                t2 = timeAfter;
                Read(DataFile(),baseName(before+1),v2);
                k2 = before+1;
            }
        }
        // Interpolate between time values
        if (v1.Grid()!=v2.Grid()) {
            DTErrorMessage("DTSeriesVectorField3D::GetInterpolated(time)","Need to have the same grid in order to interpolate.");
        }
        else {
            float fraction = float((timeAfter-time)/(timeAfter-timeBefore));
            DTFloatArray X = fraction*v1.X() + (1-fraction)*v2.X();
            DTFloatArray Y = fraction*v1.Y() + (1-fraction)*v2.Y();
            DTFloatArray Z = fraction*v1.Z() + (1-fraction)*v2.Z();
            toReturn = DTVectorField3D(v1.Grid(),X,Y,Z);
        }
    }
    
    return toReturn;
}

DTVectorField3D DTSeriesVectorField3D::operator()(int index) const
{
    if (index<0 || index>=HowManySaved()) {
        DTErrorOutOfRange("DTSeriesVectorField3D(i)",index,HowManySaved());
        return DTVectorField3D();
    }
    DTVectorField3D toReturn;
    Read(DataFile(),baseName(index),toReturn);
    return toReturn;
}

void Read(const DTDataStorage &input,const std::string &name,DTSeriesVectorField3D &toReturn)
{
    string variableName;
    DTMutablePointer<DTDataStorage> storage = DTSeries::ReferencedDataFile(input,name,variableName);
    toReturn = DTSeriesVectorField3D(*storage,variableName);
}
