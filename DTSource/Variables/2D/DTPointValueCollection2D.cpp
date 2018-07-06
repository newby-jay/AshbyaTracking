// Part of DTSource. Copyright 2004-2017. David Adalsteinsson.
// see http://www.visualdatatools.com/DTSource/license.html for more information.

#include "DTPointValueCollection2D.h"

#include "DTError.h"
#include "DTDataStorage.h"
#include "DTRegion1D.h"
#include "DTRegion2D.h"

DTPointValueCollection2D::DTPointValueCollection2D(const DTPointCollection2D &ptsIn,const DTDoubleArray &valIn)
: points(), values()
{
    if (ptsIn.IsEmpty() && valIn.IsEmpty()) return;
    
    if (valIn.Length()!=ptsIn.NumberOfPoints() || valIn.Length()!=valIn.m()) {
        DTErrorMessage("DTPointValueCollection2D(Points,Values)","Invalid size of value array.");
        return;
    }
    
    points = ptsIn;
    values = valIn;
}

void DTPointValueCollection2D::pinfo(void) const
{
    points.pinfo();
}

void DTPointValueCollection2D::pall(void) const
{
    DTDoubleArray pts = Points().Data();
    ssize_t howMany = pts.n();
    if (howMany==0) std::cerr << "Empty\n";
    for (int j=0;j<howMany;j++) {
        std::cerr << pts(0,j) << ", " << pts(1,j) << " - " << values(j) << std::endl;
    }
}

DTPoint2DValue DTPointValueCollection2D::operator()(int i) const
{
    return DTPoint2DValue(points(i),values(i));
}

bool operator==(const DTPointValueCollection2D &A,const DTPointValueCollection2D &B)
{
	return (A.Points()==B.Points() && A.Values()==B.Values());
}

bool operator!=(const DTPointValueCollection2D &A,const DTPointValueCollection2D &B)
{
	return (A.Points()!=B.Points() || A.Values()!=B.Values());
}

DTRegion2D BoundingBox(const DTPointValueCollection2D &coll)
{
    return BoundingBox(coll.Points());
}

DTRegion1D ValueRange(const DTPointValueCollection2D &coll)
{
    return ValueRange(coll.Values());
}

void Read(const DTDataStorage &input,const std::string &name,DTPointValueCollection2D &toReturn)
{
    DTPointCollection2D pts;
    Read(input,name,pts);
    DTDoubleArray val;
    Read(input,name+"_V",val);
    toReturn = DTPointValueCollection2D(pts,val);
}

void Write(DTDataStorage &output,const std::string &name,const DTPointValueCollection2D &theVar)
{
    Write(output,name+"_V",theVar.Values());
    Write(output,name,theVar.Points());
}

void WriteOne(DTDataStorage &output,const std::string &name,const DTPointValueCollection2D &toWrite)
{
    Write(output,name,toWrite);
    Write(output,"Seq_"+name,"2D Point Value Collection");
    output.Flush();
}

void Read(const DTDataStorage &input,const std::string &name,DTPointValueCollection2D &toReturn,DTPointCollection2D_SaveInfo &shared)
{
    DTPointCollection2D pts;
    Read(input,name,pts,shared);
    DTDoubleArray val;
    Read(input,name+"_V",val);
    toReturn = DTPointValueCollection2D(pts,val);
}

void Write(DTDataStorage &output,const std::string &name,const DTPointValueCollection2D &theVar,DTPointCollection2D_SaveInfo &shared)
{
    Write(output,name+"_V",theVar.Values());
    Write(output,name,theVar.Points(),shared);
}

