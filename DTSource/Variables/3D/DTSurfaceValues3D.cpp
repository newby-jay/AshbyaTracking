// Part of DTSource. Copyright 2004-2017. David Adalsteinsson.
// see http://www.visualdatatools.com/DTSource/license.html for more information.

#include "DTSurfaceValues3D.h"

#include "DTError.h"
#include "DTRegion1D.h"
#include "DTRegion3D.h"
#include "DTFloatArrayOperators.h"
#include "DTDoubleArrayOperators.h"

DTSurfaceValues3D::DTSurfaceValues3D(const DTSurface3D &surf,const DTFloatArray &val)
{
    if (surf.Points().NumberOfPoints()!=val.Length()) {
        DTErrorMessage("DTSurfaceValues3D(Surface,Values)","Number of values has to match the number of points.");
        return;
    }
    if (val.m()!=val.Length()) {
        DTErrorMessage("DTSurfaceValues3D(Surface,Values)","The value array has to be a list.");
        return;
    }

    surface = surf;
    _floatData = val;
}

DTSurfaceValues3D::DTSurfaceValues3D(const DTSurface3D &surf,const DTDoubleArray &val)
{
    if (surf.Points().NumberOfPoints()!=val.Length()) {
        DTErrorMessage("DTSurfaceValues3D(Surface,Values)","Number of values has to match the number of points.");
        return;
    }
    if (val.m()!=val.Length()) {
        DTErrorMessage("DTSurfaceValues3D(Surface,Values)","The value array has to be a list.");
        return;
    }
    
    surface = surf;
    _doubleData = val;
}

const DTDoubleArray &DTSurfaceValues3D::DoubleData(void) const
{
    if (_floatData.NotEmpty()) {
        DTErrorMessage("DTMesh3D::DoubleData","Array saved as float.");
    }
    return _doubleData;
}

const DTFloatArray &DTSurfaceValues3D::FloatData(void) const
{
    if (_doubleData.NotEmpty()) {
        DTErrorMessage("DTMesh3D::FloatData","Array saved as double.");
    }
    return _floatData;
}

void DTSurfaceValues3D::pinfo(void) const
{
    surface.pinfo();
}

bool operator==(const DTSurfaceValues3D &A,const DTSurfaceValues3D &B)
{
    if (A.Surface()!=B.Surface()) return false;
    if (A.FloatPrecision()!=B.FloatPrecision()) return false;
    if (A.FloatPrecision()) {
        return (A.FloatData()==B.FloatData());
    }
    else {
        return (A.DoubleData()==B.DoubleData());
    }
}

bool operator!=(const DTSurfaceValues3D &A,const DTSurfaceValues3D &B)
{
    if (A.Surface()!=B.Surface()) return true;
    if (A.FloatPrecision()!=B.FloatPrecision()) return true;
    if (A.FloatPrecision()) {
        return (A.FloatData()!=B.FloatData());
    }
    else {
        return (A.DoubleData()!=B.DoubleData());
    }
}

DTRegion1D ValueRange(const DTSurfaceValues3D &A)
{
    if (A.FloatPrecision())
        return ValueRange(A.FloatData());
    else
        return ValueRange(A.DoubleData());
}

DTRegion3D BoundingBox(const DTSurfaceValues3D &A)
{
    return BoundingBox(A.Surface());
}

DTSurfaceValues3D Combine(const DTList<DTSurfaceValues3D> &L)
{
    if (L.IsEmpty()) return DTSurfaceValues3D();
    
	ssize_t surfN,howManySurfaces = L.Length();
	int howManyPoints = 0;
	int howManyTriangles = 0;
	DTSurface3D S;
	bool hasNormals = true;
	for (surfN=0;surfN<howManySurfaces;surfN++) {
		S = L(surfN).Surface();
		howManyPoints += S.NumberOfPoints();
		howManyTriangles += S.NumberOfTriangles();
		if (S.NormalsDefined()==false) hasNormals = false;
	}
	
    if (L(0).FloatPrecision()) {
        DTMutableFloatArray newPoints(3,howManyPoints);
        DTMutableFloatArray newValues(howManyPoints);
        DTMutableFloatArray newNormals(3,hasNormals ? howManyPoints : 0);
        DTMutableIntArray newTriangles(3,howManyTriangles);
        int posInPoints = 0;
        int posInTriangles = 0;
        DTSurfaceValues3D SV;
        for (surfN=0;surfN<howManySurfaces;surfN++) {
            SV = L(surfN);
            S = SV.Surface();
            MemoryCopy(newPoints,3*posInPoints,S.Points().Data());
            MemoryCopy(newValues,posInPoints,SV.FloatData());
            CopyValuesIntoAndAdd(newTriangles,3*posInTriangles,S.Connections(),posInPoints);
            if (hasNormals)
                MemoryCopy(newNormals,3*posInPoints,S.Normals());
            posInPoints += S.NumberOfPoints();
            posInTriangles += S.NumberOfTriangles();
        }
        
        DTSurface3D returnSurf;
        if (hasNormals)
            returnSurf = DTSurface3D(newTriangles,DTPointCollection3D(newPoints),newNormals);
        else
            returnSurf = DTSurface3D(newTriangles,DTPointCollection3D(newPoints));
        
        return DTSurfaceValues3D(returnSurf,newValues);
    }
    else {
        DTMutableFloatArray newPoints(3,howManyPoints);
        DTMutableDoubleArray newValues(howManyPoints);
        DTMutableFloatArray newNormals(3,hasNormals ? howManyPoints : 0);
        DTMutableIntArray newTriangles(3,howManyTriangles);
        int posInPoints = 0;
        int posInTriangles = 0;
        DTSurfaceValues3D SV;
        for (surfN=0;surfN<howManySurfaces;surfN++) {
            SV = L(surfN);
            S = SV.Surface();
            MemoryCopy(newPoints,3*posInPoints,S.Points().Data());
            MemoryCopy(newValues,posInPoints,SV.DoubleData());
            CopyValuesIntoAndAdd(newTriangles,3*posInTriangles,S.Connections(),posInPoints);
            if (hasNormals)
                MemoryCopy(newNormals,3*posInPoints,S.Normals());
            posInPoints += S.NumberOfPoints();
            posInTriangles += S.NumberOfTriangles();
        }
        
        DTSurface3D returnSurf;
        if (hasNormals)
            returnSurf = DTSurface3D(newTriangles,DTPointCollection3D(newPoints),newNormals);
        else
            returnSurf = DTSurface3D(newTriangles,DTPointCollection3D(newPoints));
        
        return DTSurfaceValues3D(returnSurf,newValues);
    }
}

DTSurfaceValues3D operator-(const DTSurfaceValues3D &A,const DTSurfaceValues3D &B)
{
	if (A.Surface()!=B.Surface()) {
		DTErrorMessage("DTSurfaceValues3D-DTSurfaceValues3D","Need to be defined for the same surface");
		return DTSurfaceValues3D();
	}
	
    if (A.FloatPrecision() && B.FloatPrecision()) {
        return DTSurfaceValues3D(A.Surface(),A.FloatData()-B.FloatData());
    }
    else if (A.DoublePrecision() && B.DoublePrecision()) {
        return DTSurfaceValues3D(A.Surface(),A.DoubleData()-B.DoubleData());
    }
    else {
        DTErrorMessage("DTSurfaceValues3D-DTSurfaceValues3D","One is float, the other is double");
        return DTSurfaceValues3D();
    }
}

void Read(const DTDataStorage &input,const std::string &name,DTSurfaceValues3D &toReturn)
{
    DTSurface3D theSurface;
    Read(input,name,theSurface);
    DTFloatArray theValues;
    Read(input,name+"_V",theValues);

    toReturn = DTSurfaceValues3D(theSurface,theValues);
}

void Write(DTDataStorage &output,const std::string &name,const DTSurfaceValues3D &toWrite)
{
    if (toWrite.FloatPrecision()) {
        Write(output,name+"_V",toWrite.FloatData());
    }
    else {
        Write(output,name+"_V",toWrite.DoubleData());
    }
    Write(output,name,toWrite.Surface());
}

void WriteOne(DTDataStorage &output,const std::string &name,const DTSurfaceValues3D &toWrite)
{
    Write(output,name,toWrite);
    Write(output,"Seq_"+name,"3D Surface Values");
    output.Flush();
}

void Read(const DTDataStorage &input,const std::string &name,DTSurfaceValues3D &toReturn,DTSurface3D_SaveInfo &shared)
{
    DTSurface3D theSurface;
    Read(input,name,theSurface,shared);
    DTFloatArray theValues;
    Read(input,name+"_V",theValues);
    
    toReturn = DTSurfaceValues3D(theSurface,theValues);
}

void Write(DTDataStorage &output,const std::string &name,const DTSurfaceValues3D &toWrite,DTSurface3D_SaveInfo &shared)
{
    if (toWrite.FloatPrecision()) {
        Write(output,name+"_V",toWrite.FloatData());
    }
    else {
        Write(output,name+"_V",toWrite.DoubleData());
    }
    Write(output,name,toWrite.Surface(),shared);
}

#pragma mark mutable

DTMutableSurfaceValues3D::DTMutableSurfaceValues3D(const DTSurface3D &S)
{
	surface = S;
	mutableValues = DTMutableFloatArray(S.NumberOfPoints());
	_floatData = mutableValues;
}

DTMutableSurfaceValues3D::DTMutableSurfaceValues3D(const DTSurface3D &S,DTMutableFloatArray &V)
{
	if (S.NumberOfPoints()!=V.Length() || V.Length()!=V.m()) {
		DTErrorMessage("DTMutableSurfaceValues3D(surface,values)","Length of value array incorrect.");
        return;
	}
	surface = S;
	mutableValues = V;
	_floatData = mutableValues;
}
