// Part of DTSource. Copyright 2004-2017. David Adalsteinsson.
// see http://www.visualdatatools.com/DTSource/license.html for more information.

#ifndef DTTriangularGrid2D_H
#define DTTriangularGrid2D_H

#include "DTDoubleArray.h"
#include "DTIntArray.h"
#include "DTDataStorage.h"
#include "DTPath2D.h"
#include "DTPointer.h"

struct DTRegion2D;

struct DTTriangularGridOptionalData {
    DTIntArray pointNumbers;
    DTIntArray nextTriangles;
    DTPath2D domainBoundary;
};

class DTTriangularGrid2D {
public:
    DTTriangularGrid2D();
    DTTriangularGrid2D(const DTIntArray &Connections,const DTDoubleArray &Points);
    // The next constructor is only used by the Add** functions.  Do not call this directly, since it doesn't check input.
    DTTriangularGrid2D(const DTIntArray &,const DTDoubleArray &,const DTIntArray &,const DTIntArray &,const DTPath2D &);
    
    int IsEmpty() const {return connections.IsEmpty();}
    int NotEmpty() const {return connections.NotEmpty();}
    
    ssize_t NumberOfPoints(void) const {return points.n();}
    ssize_t NumberOfTriangles(void) const {return connections.n();}
    
    // Data that will always exist.
    DTIntArray Connections(void) const {return connections;}
    DTDoubleArray Points(void) const {return points;}
    
    // Optional data.  Should check for it's existence.
    bool NextTrianglesDefined(void) const {return (!(optional->nextTriangles.IsEmpty()));}
    DTIntArray NextTriangles(void) const;
    
    bool BoundaryDefined(void) const {return (optional->domainBoundary.NotEmpty());}
    DTPath2D DomainBoundary(void) const;
    
    bool PointNumbersSpecified(void) const {return (optional->pointNumbers.NotEmpty() && NotEmpty());}
    DTIntArray PointNumbers(void) const;
    
    // Debug information.
    void pinfo(void) const;

private:
    bool SetConnectionsAndPoints(const DTIntArray &,const DTDoubleArray &,const char *);
    DTIntArray connections;
    DTDoubleArray points;

	DTPointer<DTTriangularGridOptionalData> optional;
};

// Add or change entries in a grid.  Will not copy any arrays.
extern DTTriangularGrid2D AddDomain(const DTTriangularGrid2D &,const DTPath2D &);
extern DTTriangularGrid2D AddPointNumbers(const DTTriangularGrid2D &,const DTIntArray &);
extern DTTriangularGrid2D AddNextTriangleInformation(const DTTriangularGrid2D &,const DTIntArray &);
extern DTTriangularGrid2D AddNextTriangleInformation(const DTTriangularGrid2D &); // Free if already defined.

// Actions.  Intended to help use DTTriangularGrid2D inside a solver.
extern DTIntArray EdgeSegments(const DTTriangularGrid2D &); // 2xM array of edges.  Offsets into the point array. Need NextTriangle information.

extern DTDoubleArray TriangleAreas(const DTTriangularGrid2D &); // The area of each triangle.

extern bool operator==(const DTTriangularGrid2D &,const DTTriangularGrid2D &);
extern bool operator!=(const DTTriangularGrid2D &,const DTTriangularGrid2D &);

extern DTRegion2D BoundingBox(const DTTriangularGrid2D &);

// Reading and writing
extern void Read(const DTDataStorage &input,const std::string &name,DTTriangularGrid2D &toReturn);
extern void Write(DTDataStorage &output,const std::string &name,const DTTriangularGrid2D &toWrite);
extern void WriteOne(DTDataStorage &output,const std::string &name,const DTTriangularGrid2D &toWrite); // One time value, self documenting.

// A utility class to enable grid sharing.
struct DTTriangularGrid2D_SaveInfo {
    DTTriangularGrid2D grid;
    std::string name;
};

// Pass in as the third argument to Read and Write to enable the the read/write routines
// to avoid having to read or write identical grids.  This is used when reading DTTrianglarMesh2D and DTTriangularVectorField2D
extern void Read(const DTDataStorage &input,const std::string &name,DTTriangularGrid2D &toReturn,DTTriangularGrid2D_SaveInfo &);
extern void Write(DTDataStorage &output,const std::string &name,const DTTriangularGrid2D &toWrite,DTTriangularGrid2D_SaveInfo &);

#endif
