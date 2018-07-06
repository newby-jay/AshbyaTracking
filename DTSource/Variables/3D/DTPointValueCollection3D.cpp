// Part of DTSource. Copyright 2004-2017. David Adalsteinsson.
// see http://www.visualdatatools.com/DTSource/license.html for more information.

#include "DTPointValueCollection3D.h"

#include "DTError.h"
#include "DTDataStorage.h"
#include "DTRegion1D.h"
#include "DTRegion3D.h"

DTPointValueCollection3D::DTPointValueCollection3D(const DTPointCollection3D &ptsIn,const DTDoubleArray &valIn)
{
    if (ptsIn.IsEmpty() && valIn.IsEmpty()) return;

    if (valIn.Length()!=ptsIn.NumberOfPoints() || valIn.Length()!=valIn.m()) {
        DTErrorMessage("DTPointValueCollection3D(Points,Values)","Invalid size of value array.");
        return;
    }

    points = ptsIn;
    values = valIn;
}

void DTPointValueCollection3D::pinfo(void) const
{
    points.pinfo();
}

void DTPointValueCollection3D::pall(void) const
{
	size_t i,howMany = points.NumberOfPoints();
	DTPoint3D P;
	for (i=0;i<howMany;i++) {
		P = points(i);
		cerr << i << " : " << P.x << ", " << P.y << ", " << P.z << " : " << values(i) << std::endl;
	}
}

DTPointValueCollection3D ConvertToDouble(const DTPointValueCollection3D &P)
{
    return DTPointValueCollection3D(ConvertToDouble(P.Points()),P.Values());
}

DTRegion3D BoundingBox(const DTPointValueCollection3D &coll)
{
    return BoundingBox(coll.Points());
}

DTRegion1D ValueRange(const DTPointValueCollection3D &coll)
{
    return ValueRange(coll.Values());
}

DTPointValueCollection3D ExtractPoints(const DTPointValueCollection3D &points,const DTIntArray &indices)
{
    return DTPointValueCollection3D(DTPointCollection3D(ExtractColumns(points.Points().Data(),indices)),
                                    ExtractIndices(points.Values(),indices));
}

void Read(const DTDataStorage &input,const std::string &name,DTPointValueCollection3D &toReturn)
{
    DTPointCollection3D pts;
    Read(input,name,pts);
    DTDoubleArray val;
    Read(input,name+"_V",val);
    toReturn = DTPointValueCollection3D(pts,val);
}

void Write(DTDataStorage &output,const std::string &name,const DTPointValueCollection3D &theVar)
{
    Write(output,name+"_V",theVar.Values());
    Write(output,name,theVar.Points());
}

void WriteOne(DTDataStorage &output,const std::string &name,const DTPointValueCollection3D &toWrite)
{
    Write(output,name,toWrite);
    Write(output,"Seq_"+name,"3D Point Value Collection");
    output.Flush();
}

void Read(const DTDataStorage &input,const std::string &name,DTPointValueCollection3D &toReturn,DTPointCollection3D_SaveInfo &shared)
{
    DTPointCollection3D pts;
    Read(input,name,pts,shared);
    DTDoubleArray val;
    Read(input,name+"_V",val);
    toReturn = DTPointValueCollection3D(pts,val);
}

void Write(DTDataStorage &output,const std::string &name,const DTPointValueCollection3D &theVar,DTPointCollection3D_SaveInfo &shared)
{
    Write(output,name+"_V",theVar.Values());
    Write(output,name,theVar.Points(),shared);
}

void WriteFloat(DTDataStorage &output,const std::string &name,const DTPointValueCollection3D &theVar,DTPointCollection3D_SaveInfo &shared)
{
    Write(output,name+"_V",theVar.Values());
    WriteFloat(output,name,theVar.Points(),shared);
}

