// Part of DTSource. Copyright 2004-2017. David Adalsteinsson.
// see http://www.visualdatatools.com/DTSource/license.html for more information.

#include "DTVectorCollection3D.h"

#include "DTError.h"
#include "DTRegion3D.h"

DTVectorCollection3D::DTVectorCollection3D(const DTPointCollection3D &pointsIn,const DTDoubleArray &vectorsIn)
{
    if (pointsIn.IsEmpty() && vectorsIn.IsEmpty())
        return;
    
    if (vectorsIn.m()!=3 || vectorsIn.o()!=1 || pointsIn.NumberOfPoints()!=vectorsIn.n()) {
        DTErrorMessage("DTVectorCollection3D(Points,Vectors)","Invalid size for vector array.");
        return;
    }
    
    points = pointsIn;
    vectors = vectorsIn;
}

void DTVectorCollection3D::pinfo(void) const
{
    points.pinfo();
}

DTRegion3D BoundingBox(const DTVectorCollection3D &V)
{
    return BoundingBox(V.Points());
}

void Read(const DTDataStorage &input,const std::string &name,DTVectorCollection3D &toReturn)
{
    DTPointCollection3D points;
    Read(input,name,points);
    DTDoubleArray V;
    Read(input,name+"_dir",V);
    
    toReturn = DTVectorCollection3D(points,V);
}

void Write(DTDataStorage &output,const std::string &name,const DTVectorCollection3D &variable)
{
    Write(output,name+"_dir",variable.Vectors());
    Write(output,name,variable.Points());
}

void WriteOne(DTDataStorage &output,const std::string &name,const DTVectorCollection3D &toWrite)
{
    Write(output,name,toWrite);
    Write(output,"Seq_"+name,"3D Vector Collection");
    output.Flush();
}

void Read(const DTDataStorage &input,const std::string &name,DTVectorCollection3D &toReturn,DTPointCollection3D_SaveInfo &shared)
{
    DTPointCollection3D points;
    Read(input,name,points,shared);
    DTDoubleArray Vec;
    Read(input,name+"_dir",Vec);
    
    toReturn = DTVectorCollection3D(points,Vec);
}

void Write(DTDataStorage &output,const std::string &name,const DTVectorCollection3D &variable,DTPointCollection3D_SaveInfo &shared)
{
    Write(output,name+"_dir",variable.Vectors());
    Write(output,name,variable.Points(),shared);
}

