// Part of DTSource. Copyright 2004-2017. David Adalsteinsson.
// see http://www.visualdatatools.com/DTSource/license.html for more information.

#ifndef DTPointCollection2D_H
#define DTPointCollection2D_H

#include "DTDoubleArray.h"
#include "DTIntArray.h"
#include "DTPoint2D.h"

#include <string>
using namespace std;

class DTDataStorage;
struct DTRange;
class DTMutablePointCollection2D;
struct DTRegion2D;

// Create a new point collection as either
//     DTPointCollection2D(array)
// or
//     DTPointCollection2D(array,vector)
// the array needs to be allocated as (2,N), where N = number of points.

// If you don't know beforehand the size of the array, use the
// IncreaseSize(...) and TruncateSize(...) functions to resize the array.

// The array is layed out as:
// array(0,j) = x coordinate of point j, array(1,j) = y coordinate of point j.

// To extract a single entry, you can use
// DTPointCollection Pts = ...
// int i,numOfPoints = Pts.NumberOfPoints();
// for (i=0; i<numOfPoints; i++) {
//     DTPoint2D p = Pts(i);
//     ....
// }
//
// More efficient
// DTDoubleArray data = Pts.Data();
// int i,numOfPoints = data.n();
// DTPoint2D p;
// for (i=0; i<numOfPoints; i++) {
//     p.x = data(0,i);
//     p.y = data(1,i);
//     ....
// }

class DTPointCollection2D {
public:
    DTPointCollection2D() : _coordinates(), _pointNumbers() {}

    explicit DTPointCollection2D(const DTDoubleArray &input);
    DTPointCollection2D(const DTDoubleArray &input,const DTIntArray &ptN);
    
    bool IsEmpty(void) const {return _coordinates.IsEmpty();}
    ssize_t NumberOfPoints(void) const {return _coordinates.n();}
    DTDoubleArray Data(void) const {return _coordinates;}
    DTIntArray PointNumbers(void) const {return _pointNumbers;}
    bool PointNumbersSpecified(void) const {return _pointNumbers.NotEmpty();}

    DTMutablePointCollection2D Copy(void) const;
    
    // Extract a single point.
    DTPoint2D operator()(int) const;
    
    void pinfo(void) const;
    void pall(void) const;
    
protected:
    DTDoubleArray _coordinates; // 2xN.
    DTIntArray _pointNumbers; // Optional data.  Used in DataTank to keep track of the original position in a list.
};

// Can modify the value of the points, but not the number of points.
// If you want to accumulate a list of points use a DTMutableDoubleArray object and then
// construct a point colection from that.
class DTMutablePointCollection2D : public DTPointCollection2D {
public:
    DTMutablePointCollection2D() : DTPointCollection2D() {}
    explicit DTMutablePointCollection2D(const DTMutableDoubleArray &input);
    DTMutablePointCollection2D(const DTMutableDoubleArray &input,const DTIntArray &);
    
    DTMutableDoubleArray Data(void) {return _mutableCoordinates;}
    DTDoubleArray Data(void) const {return _coordinates;}

    void Overwrite(const DTPointCollection2D &,const DTRange &);
    
    void operator-=(DTPoint2D); // Subtract a point from every location
    void operator+=(const DTPointCollection2D &);
    void operator-=(const DTPointCollection2D &);

private:
    DTMutableDoubleArray _mutableCoordinates;
};


extern bool operator==(const DTPointCollection2D &,const DTPointCollection2D &);
extern bool operator!=(const DTPointCollection2D &,const DTPointCollection2D &);

extern DTMutablePointCollection2D operator-(const DTPointCollection2D &,const DTPointCollection2D &);
extern DTMutablePointCollection2D operator-(const DTPointCollection2D &,const DTPoint2D &);
extern DTMutablePointCollection2D ExtractIndices(const DTPointCollection2D &,const DTRange &);

extern DTRegion2D BoundingBox(const DTPointCollection2D &);

// Reading and writing
extern void Read(const DTDataStorage &input,const std::string &name,DTPointCollection2D &toReturn);
extern void Write(DTDataStorage &output,const std::string &name,const DTPointCollection2D &theVar);
extern void WriteOne(DTDataStorage &output,const std::string &name,const DTPointCollection2D &toWrite); // One time value, self documenting.

// A utility class to enable grid sharing.
struct DTPointCollection2D_SaveInfo {
    DTPointCollection2D points;
    std::string name;
};

// Pass in as the third argument to Read and Write to enable the the read/write routines
// to avoid having to read or write identical grids.  This is used when reading DTTrianglarMesh2D and DTTriangularVectorField2D
extern void Read(const DTDataStorage &input,const std::string &name,DTPointCollection2D &toReturn,DTPointCollection2D_SaveInfo &);
extern void Write(DTDataStorage &output,const std::string &name,const DTPointCollection2D &toWrite,DTPointCollection2D_SaveInfo &);

#endif
