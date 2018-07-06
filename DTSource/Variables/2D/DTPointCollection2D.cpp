// Part of DTSource. Copyright 2004-2017. David Adalsteinsson.
// see http://www.visualdatatools.com/DTSource/license.html for more information.

#include "DTPointCollection2D.h"

#include "DTError.h"
#include "DTDataStorage.h"
#include "DTRegion2D.h"
#include "DTDoubleArrayOperators.h"
#include "DTUtilities.h"

#include <cstring>

DTPointCollection2D::DTPointCollection2D(const DTDoubleArray &input)
: _coordinates(), _pointNumbers()
{
    if (input.IsEmpty()) return;

    if (input.m()!=2 || input.o()!=1) {
        DTErrorMessage("DTPointCollection2D(Array)","Invalid size of array.");
        return;
    }

    _coordinates = input;
}

DTPointCollection2D::DTPointCollection2D(const DTDoubleArray &input,const DTIntArray &ptN)
: _coordinates(), _pointNumbers()
{
    if (input.IsEmpty()) return;
    
    if (input.m()!=2 || input.o()!=1 || ptN.m()!=ptN.Length() || ptN.Length()!=input.n()) {
        DTErrorMessage("DTPointCollection2D(Array,Point Numbers)","Invalid size of array.");
        return;
    }
    
    _coordinates = input;
    _pointNumbers = ptN;
}

DTPoint2D DTPointCollection2D::operator()(int i) const
{
    return DTPoint2D(_coordinates(0,i),_coordinates(1,i));
}

DTMutablePointCollection2D DTPointCollection2D::Copy(void) const
{
    if (_pointNumbers.NotEmpty())
        return DTMutablePointCollection2D(_coordinates.Copy(),_pointNumbers);
    else
        return DTMutablePointCollection2D(_coordinates.Copy());
}

void DTPointCollection2D::pinfo(void) const
{
    if (_coordinates.IsEmpty())
        std::cerr << "No points\n";
    else if (_coordinates.n()==1)
        std::cerr << "1 point\n";
    else
        std::cerr << _coordinates.n() << " points\n";
    std::cerr << flush;
}

void DTPointCollection2D::pall(void) const
{
    _coordinates.pall();
}

#pragma mark Mutable version

DTMutablePointCollection2D::DTMutablePointCollection2D(const DTMutableDoubleArray &input)
{
    if (input.IsEmpty()) return;
    
    if (input.m()!=2 || input.o()!=1) {
        DTErrorMessage("DTPointCollection2D(Array)","Invalid size of array.");
        return;
    }

    _coordinates = input;
    _mutableCoordinates = input;
}

DTMutablePointCollection2D::DTMutablePointCollection2D(const DTMutableDoubleArray &input,const DTIntArray &ptN)
{
    if (input.IsEmpty()) return;
    
    if (input.m()!=2 || input.o()!=1 || ptN.m()!=ptN.Length() || ptN.Length()!=input.n()) {
        DTErrorMessage("DTPointCollection2D(Array,Point Numbers)","Invalid size of array.");
        return;
    }
    
    _coordinates = input;
    _mutableCoordinates = input;
    _pointNumbers = ptN;
}

void DTMutablePointCollection2D::operator-=(DTPoint2D P)
{
    size_t i,howMany = _mutableCoordinates.n();
    for (i=0;i<howMany;i++) {
        _mutableCoordinates(0,i) -= P.x;
        _mutableCoordinates(1,i) -= P.y;
    }
}

void DTMutablePointCollection2D::operator+=(const DTPointCollection2D &P)
{
    if (NumberOfPoints()!=P.NumberOfPoints()) {
        DTErrorMessage("MutablePointCollection2D+=PointCollection2D","Sizes don't match");
    }
    else {
        _mutableCoordinates += P.Data();
    }
}

void DTMutablePointCollection2D::operator-=(const DTPointCollection2D &P)
{
    if (NumberOfPoints()!=P.NumberOfPoints()) {
        DTErrorMessage("MutablePointCollection2D-=PointCollection2D","Sizes don't match");
    }
    else {
        _mutableCoordinates -= P.Data();
    }
}

void DTMutablePointCollection2D::Overwrite(const DTPointCollection2D &P,const DTRange &r)
{
    if (r.length!=P.NumberOfPoints()) {
        DTErrorMessage("PointCollection::Overwite(PointCollection2D,Range)", "Range has a different length than the point collection");
        return;
    }
    if (r.start+r.length>=NumberOfPoints()) {
        DTErrorMessage("PointCollection::Overwite(PointCollection2D,Range)", "Range is out of bounds");
        return;
    }
    std::memcpy(_mutableCoordinates.Pointer()+2*r.start, P.Data().Pointer(), sizeof(double)*2*r.length);
}

#pragma mark Functions

bool operator==(const DTPointCollection2D &A,const DTPointCollection2D &B)
{
    return (A.Data()==B.Data());
}

bool operator!=(const DTPointCollection2D &A,const DTPointCollection2D &B)
{
    return (A.Data()!=B.Data());
}

DTMutablePointCollection2D operator-(const DTPointCollection2D &A,const DTPointCollection2D &B)
{
    if (A.NumberOfPoints()!=B.NumberOfPoints()) {
        DTErrorMessage("PointCollection2D-PointCollection2D","Incompatible lengths");
        return DTMutablePointCollection2D();
    }
    return DTMutablePointCollection2D(A.Data()-B.Data());
}

DTMutablePointCollection2D operator-(const DTPointCollection2D &A,const DTPoint2D &b)
{
    size_t i,howMany = A.NumberOfPoints();
    DTMutableDoubleArray toReturn(2,howMany);
    DTDoubleArray Aa = A.Data();
    for (i=0;i<howMany;i++) {
        toReturn(0,i) = Aa(0,i) - b.x;
        toReturn(1,i) = Aa(1,i) - b.y;
    }
    return DTMutablePointCollection2D(toReturn);
}

DTRegion2D BoundingBox(const DTPointCollection2D &coll)
{
    return BoundingBox2D(coll.Data());
}

DTMutablePointCollection2D ExtractIndices(const DTPointCollection2D &A,const DTRange &r)
{
    if (r.start+r.length>A.NumberOfPoints()) {
        DTErrorMessage("ExtractIndices(PointCollection,Range)","Range is out of bounds");
        return DTMutablePointCollection2D();
    }
    if (A.PointNumbersSpecified())
        return DTMutablePointCollection2D(ExtractColumns(A.Data(),r),ExtractIndices(A.PointNumbers(),r));
    else
        return DTMutablePointCollection2D(ExtractColumns(A.Data(),r));
}

#pragma mark I/O Routines

void Read(const DTDataStorage &input,const std::string &name,DTPointCollection2D &toReturn)
{
    std::string theName = input.ResolveName(name);

    // The main array stores the loop structure
    DTDoubleArray points = input.ReadDoubleArray(theName);
    if (input.Contains(theName+"_ptN")) {
        DTIntArray ptN = input.ReadIntArray(theName+"_ptN");
        toReturn = DTPointCollection2D(points,ptN);
    }
    else {
        toReturn = DTPointCollection2D(points);
    }
}

void Write(DTDataStorage &output,const std::string &name,const DTPointCollection2D &theVar)
{
    Write(output,name+"_bbox2D",BoundingBox(theVar));

    DTIntArray ptN = theVar.PointNumbers();
    if (ptN.IsEmpty()==false) {
        Write(output,name+"_ptN",ptN);
    }
    Write(output,name,theVar.Data());
}

void WriteOne(DTDataStorage &output,const std::string &name,const DTPointCollection2D &toWrite)
{
    Write(output,name,toWrite);
    Write(output,"Seq_"+name,"2D Point Collection");
    output.Flush();
}

void Read(const DTDataStorage &input,const std::string &name,DTPointCollection2D &toReturn,DTPointCollection2D_SaveInfo &SaveInfo)
{
    std::string theName = input.ResolveName(name);
    if (SaveInfo.name==theName) {
        toReturn = SaveInfo.points; // Already read this grid in.
        return;
    }
    
    Read(input,theName,toReturn);
    
    SaveInfo.points = toReturn;
    SaveInfo.name = theName;
}

void Write(DTDataStorage &output,const std::string &name,const DTPointCollection2D &toWrite,DTPointCollection2D_SaveInfo &SaveInfo)
{
    if (SaveInfo.name.length() && SaveInfo.points==toWrite) {
        // Just save the reference.
        Write(output,name,SaveInfo.name);
    }
    else {
        Write(output,name,toWrite);
        SaveInfo.points = toWrite;
        SaveInfo.name = name;
    }
}

