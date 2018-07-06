// Part of DTSource. Copyright 2004-2017. David Adalsteinsson.
// see http://www.visualdatatools.com/DTSource/license.html for more information.

#include "DTVectorCollection2D.h"

#include "DTError.h"
#include "DTRegion2D.h"

DTVectorCollection2D::DTVectorCollection2D(const DTPointCollection2D &pointsIn,const DTDoubleArray &vectorsIn)
{
    if (pointsIn.IsEmpty() && vectorsIn.IsEmpty())
        return;
    
    if (vectorsIn.m()!=2 || vectorsIn.o()!=1 || pointsIn.NumberOfPoints()!=vectorsIn.n()) {
        DTErrorMessage("DTVectorCollection2D(Points,Vectors)","Invalid size for vector array.");
        return;
    }
    
    points = pointsIn;
    vectors = vectorsIn;
}

void DTVectorCollection2D::pinfo(void) const
{
    points.pinfo();
}

DTMutableVectorCollection2D::DTMutableVectorCollection2D(const DTPointCollection2D &pointsIn,const DTMutableDoubleArray &vectorsIn)
{
    if (pointsIn.IsEmpty() && vectorsIn.IsEmpty())
        return;
    
    if (vectorsIn.m()!=2 || vectorsIn.o()!=1 || pointsIn.NumberOfPoints()!=vectorsIn.n()) {
        DTErrorMessage("DTVectorCollection2D(Points,Vectors)","Invalid size for vector array.");
        return;
    }
    
    points = pointsIn;
    vectors = vectorsIn;
    mutableVectors = vectorsIn;
}

DTRegion2D BoundingBox(const DTVectorCollection2D &v)
{
    return BoundingBox(v.Points());
}

void Read(const DTDataStorage &input,const std::string &name,DTVectorCollection2D &toReturn)
{
    DTPointCollection2D points;
    Read(input,name,points);
    DTDoubleArray V;
    Read(input,name+"_dir",V);

    toReturn = DTVectorCollection2D(points,V);
}

void Write(DTDataStorage &output,const std::string &name,const DTVectorCollection2D &variable)
{
    Write(output,name+"_dir",variable.Vectors());
    Write(output,name,variable.Points());
}

void WriteOne(DTDataStorage &output,const std::string &name,const DTVectorCollection2D &toWrite)
{
    Write(output,name,toWrite);
    Write(output,"Seq_"+name,"2D Vector Collection");
    output.Flush();
}

void Read(const DTDataStorage &input,const std::string &name,DTVectorCollection2D &toReturn,DTPointCollection2D_SaveInfo &shared)
{
    DTPointCollection2D points;
    Read(input,name,points,shared);
    DTDoubleArray Vec;
    Read(input,name+"_dir",Vec);
    
    toReturn = DTVectorCollection2D(points,Vec);
}

void Write(DTDataStorage &output,const std::string &name,const DTVectorCollection2D &variable,DTPointCollection2D_SaveInfo &shared)
{
    Write(output,name+"_dir",variable.Vectors());
    Write(output,name,variable.Points(),shared);
}

