// Part of DTSource. Copyright 2004-2017. David Adalsteinsson.
// see http://www.visualdatatools.com/DTSource/license.html for more information.

#ifndef DTMesh3D_H
#define DTMesh3D_H

#include "DTFloatArray.h"
#include "DTDoubleArray.h"
#include "DTDataStorage.h"
#include "DTMesh3DGrid.h"

struct DTRegion1D;

// Two classes here, DTMesh3D which has a full grid and DTMesh3DBanded which only stores values at specific values.
// There are both const and mutable versions of these classes.

class DTMesh3D {
public:
    DTMesh3D() {};
    DTMesh3D(const DTMesh3DGrid &grid,const DTFloatArray &input);
    DTMesh3D(const DTMesh3DGrid &grid,const DTDoubleArray &input);

    bool IsEmpty(void) const {return (_floatData.IsEmpty() && _doubleData.IsEmpty());}
    
    bool FloatPrecision(void) const {return (_floatData.NotEmpty());}
    bool DoublePrecision(void) const {return (_doubleData.NotEmpty());}
    
    const DTDoubleArray &DoubleData(void) const;
    const DTFloatArray &FloatData(void) const;
    DTMesh3DGrid Grid(void) const {return _grid;}
    
    ssize_t m(void) const {return _grid.m();}
    ssize_t n(void) const {return _grid.n();}
    ssize_t o(void) const {return _grid.o();}

    void pinfo(void) const;
    
protected:
    DTMesh3DGrid _grid;
    DTFloatArray _floatData;
    DTDoubleArray _doubleData;
};

extern DTMesh3D ApplyMask(const DTMesh3D &,const DTMask &);
extern DTMesh3D RemoveMask(const DTMesh3D &);

extern bool CompatibleMeshes(const DTMesh3D &,const DTMesh3D &);
extern bool operator==(const DTMesh3D &,const DTMesh3D &);
extern bool operator!=(const DTMesh3D &,const DTMesh3D &);

extern DTMesh3D ConvertToFloat(DTMesh3D);
extern DTMesh3D ConvertToDouble(DTMesh3D);

extern DTMesh3D Crop(const DTMesh3D &,const DTRegion3D &);
extern DTRegion3D BoundingBox(const DTMesh3D &);
extern DTRegion1D ValueRange(const DTMesh3D &);
extern double Minimum(const DTMesh3D &);
extern double Maximum(const DTMesh3D &);

extern DTMesh3D operator-(const DTMesh3D &,const DTMesh3D &);

// Reading and writing
extern void Read(const DTDataStorage &input,const std::string &name,DTMesh3D &toReturn);
extern void Write(DTDataStorage &output,const std::string &name,const DTMesh3D &theMesh);
extern void WriteFloat(DTDataStorage &output,const std::string &name,const DTMesh3D &theMesh);
extern void WriteOne(DTDataStorage &output,const std::string &name,const DTMesh3D &toWrite); // One time value, self documenting.

////////////////////////////////////////////////////////////////////////////
// Mutable version
////////////////////////////////////////////////////////////////////////////

class DTMutableMesh3D : public DTMesh3D
{
public:
    DTMutableMesh3D() : DTMesh3D() {}
    DTMutableMesh3D(const DTMesh3DGrid &grid); // Double array
    DTMutableMesh3D(const DTMesh3DGrid &grid,const DTMutableDoubleArray &input);
    DTMutableMesh3D(const DTMesh3DGrid &grid,const DTMutableFloatArray &input);

    DTMutableDoubleArray DoubleData(void) const;
    DTMutableFloatArray FloatData(void) const;

    void operator-=(const DTMesh3D &);
    void operator+=(const DTMesh3D &);
    void operator*=(const DTMesh3D &);
    void operator*=(const DTDoubleArray &);
    void operator/=(const DTMesh3D &);
    void operator*=(double);

    void operator=(double);
	void SetMaskedOutValues(double); // Only set values outside the mask.

private:
    DTMutableDoubleArray _mutableDoubleData;
    DTMutableFloatArray _mutableFloatData;
};

extern DTMutableMesh3D Copy(const DTMesh3D &);

extern DTMutableMesh3D ConvertToFloat(const DTMutableMesh3D &);
extern DTMutableMesh3D ConvertToDouble(const DTMutableMesh3D &);

extern DTMutableMesh3D ApplyMask(const DTMutableMesh3D &mesh,const DTMask &mask);
extern DTMutableMesh3D RemoveMask(const DTMutableMesh3D &);


////////////////////////////////////////////////////////////////////////////
// Below this is the banded version - DTMesh3DBanded
////////////////////////////////////////////////////////////////////////////

class DTMutableMesh3DBanded;

class DTMesh3DBanded {
public:
    DTMesh3DBanded() : _grid(), _doubleData(), _floatData() {};
    DTMesh3DBanded(const DTMesh3DGrid &,const DTDoubleArray &);
    DTMesh3DBanded(const DTMesh3DGrid &,const DTFloatArray &);

    bool IsEmpty(void) const {return (_doubleData.IsEmpty() && _floatData.IsEmpty());}

    DTMutableMesh3DBanded Copy(void) const;
    
    bool FloatPrecision(void) const {return (_floatData.NotEmpty());}
    bool DoublePrecision(void) const {return (_doubleData.NotEmpty());}

    DTMesh3DGrid Grid(void) const {return _grid;}
    const DTFloatArray &FloatData(void) const;
    const DTDoubleArray &DoubleData(void) const;

    ssize_t m(void) const {return _grid.m();}
    ssize_t n(void) const {return _grid.n();}
    ssize_t o(void) const {return _grid.o();}

    void pinfo(void) const;

protected:
    DTMesh3DGrid _grid;
    DTFloatArray _floatData;
    DTDoubleArray _doubleData;
};

extern DTMesh3DBanded Crop(const DTMesh3DBanded &,const DTRegion3D &);

class DTMutableMesh3DBanded : public DTMesh3DBanded {
public:
    DTMutableMesh3DBanded() : _mutableFloatData(){}
    DTMutableMesh3DBanded(const DTMesh3DGrid &);
    DTMutableMesh3DBanded(const DTMesh3DGrid &,const DTMutableFloatArray &);
    DTMutableMesh3DBanded(const DTMesh3DGrid &,const DTMutableDoubleArray &);

    void operator=(double);
    void operator*=(double);
    void operator-=(const DTMesh3DBanded &);
    void operator+=(const DTMesh3DBanded &);
    void operator*=(const DTMesh3DBanded &);

    const DTFloatArray &FloatData(void) const;
    DTMutableFloatArray FloatData(void);
    const DTDoubleArray &DoubleData(void) const;
    DTMutableDoubleArray DoubleData(void);

private:
    DTMutableFloatArray _mutableFloatData;
    DTMutableDoubleArray _mutableDoubleData;
};

extern void operator-=(DTMutableMesh3D &,const DTMesh3DBanded &);
extern void operator+=(DTMutableMesh3D &,const DTMesh3DBanded &);
extern void operator*=(DTMutableMesh3DBanded &,const DTMesh3D &);

extern DTMesh3D ConvertToFull(const DTMesh3DBanded &A);
extern DTMutableMesh3DBanded ConvertToBanded(const DTMesh3D &); // Uses the mask
extern DTMutableMesh3DBanded ConvertToBanded(const DTMesh3D &,double outside); // Uses the mask
extern void CopyValues(DTMutableMesh3DBanded &A,const DTMesh3D &B);

extern DTRegion3D BoundingBox(const DTMesh3DBanded &);
extern DTRegion1D ValueRange(const DTMesh3DBanded &);

// Reading and writing
extern void Read(const DTDataStorage &input,const std::string &name,DTMesh3DBanded &toReturn);
extern void Write(DTDataStorage &output,const std::string &name,const DTMesh3DBanded &theMesh);
extern void WriteFloat(DTDataStorage &output,const std::string &name,const DTMesh3DBanded &theMesh);
extern void WriteOne(DTDataStorage &output,const std::string &name,const DTMesh3DBanded &toWrite); // One time value, self documenting.

#endif
