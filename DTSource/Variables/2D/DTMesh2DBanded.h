// Part of DTSource. Copyright 2004-2017. David Adalsteinsson.
// see http://www.visualdatatools.com/DTSource/license.html for more information.

#ifndef DTMesh2DBanded_H
#define DTMesh2DBanded_H

// Differences between DTMesh2DBanded and DTMesh2D
// DTMesh2DBanded only stores values inside the given mask, undefined outside.
// DTMesh2DBanded only stores double precision numbers
// DTMesh2DBanded comes in mutable/const versions.
// DTMesh2DBanded requires a mask to be defined for the grid.

#include "DTMesh2DGrid.h"
#include "DTDoubleArray.h"

class DTMutableMesh2D;
class DTMesh2D;
struct DTRegion1D;
struct DTRegion2D;

class DTMesh2DBanded {
public:
    DTMesh2DBanded() : grid(), data() {};
    DTMesh2DBanded(const DTMesh2DGrid &,const DTDoubleArray &);

    bool IsEmpty(void) const {return (data.IsEmpty());}
    DTMesh2DGrid Grid(void) const {return grid;}
    DTDoubleArray Data(void) const {return data;}
    
    size_t m(void) const {return grid.m();}
    size_t n(void) const {return grid.n();}
    
    void pinfo(void) const;
    
protected:
    DTMesh2DGrid grid;
    DTDoubleArray data;
};

class DTMutableMesh2DBanded : public DTMesh2DBanded {
public:
    DTMutableMesh2DBanded() : mutableData() {}
    DTMutableMesh2DBanded(const DTMesh2DGrid &);
    DTMutableMesh2DBanded(const DTMesh2DGrid &,const DTMutableDoubleArray &);

    void operator=(double);
    void operator*=(double);
    void operator-=(const DTMesh2DBanded &);
    void operator+=(const DTMesh2DBanded &);
    void operator*=(const DTMesh2DBanded &);
    
    DTDoubleArray Data(void) const {return data;}
    DTMutableDoubleArray Data(void) {return mutableData;}

private:
    DTMutableDoubleArray mutableData;
};

extern void operator-=(DTMutableMesh2D &,const DTMesh2DBanded &);
extern void operator+=(DTMutableMesh2D &,const DTMesh2DBanded &);
extern void operator*=(DTMutableMesh2DBanded &,const DTMesh2D &);

extern DTMutableMesh2D ConvertToFull(const DTMesh2DBanded &);
extern DTMutableMesh2DBanded ConvertToBanded(const DTMesh2D &);
extern void CopyValues(DTMutableMesh2DBanded &A,const DTMesh2D &B);

extern DTRegion2D BoundingBox(const DTMesh2DBanded &);
extern DTRegion1D ValueRange(const DTMesh2DBanded &);

// Reading and writing
extern void Read(const DTDataStorage &input,const std::string &name,DTMesh2DBanded &toReturn);
extern void Write(DTDataStorage &output,const std::string &name,const DTMesh2DBanded &theMesh);
extern void WriteOne(DTDataStorage &output,const std::string &name,const DTMesh2DBanded &toWrite); // One time value, self documenting.

#endif
