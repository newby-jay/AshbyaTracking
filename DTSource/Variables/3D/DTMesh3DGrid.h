// Part of DTSource. Copyright 2004-2017. David Adalsteinsson.
// see http://www.visualdatatools.com/DTSource/license.html for more information.

#ifndef DTMesh3DGrid_H
#define DTMesh3DGrid_H

#include "DTDataStorage.h"
#include "DTPoint3D.h"
#include "DTMask.h"

struct DTRegion3D;

class DTMesh3DGrid {
public:
    DTMesh3DGrid() : isEmpty(true), mv(0), nv(0), ov(0), origin(0,0,0), _dx(1.0), _dy(1.0), _dz(1.0), _maskDefined(false), mask(), _mask() {};
    DTMesh3DGrid(ssize_t m,ssize_t n,ssize_t o);
    DTMesh3DGrid(DTPoint3D origin,double dx,double dy,double dz,ssize_t m,ssize_t n,ssize_t o);
    DTMesh3DGrid(DTPoint3D origin,double dx,double dy,double dz,const DTMask &);
    DTMesh3DGrid(DTPoint3D origin,double dx,double dy,double dz,const DTMask3D &);
    
    ssize_t m(void) const {return mv;}
    ssize_t n(void) const {return nv;}
    ssize_t o(void) const {return ov;}
    
    DTPoint3D ConvertCoordinateToGrid(const DTPoint3D &P) const {return DTPoint3D((P.x-origin.x)/_dx,(P.y-origin.y)/_dy,(P.z-origin.z)/_dz);}
	DTPoint3D ConvertGridToCoordinate(const DTPoint3D &P) const {return DTPoint3D(P.x*_dx+origin.x,P.y*_dy+origin.y,P.z*_dz+origin.z);}
    
    bool IsEmpty(void) const {return isEmpty;}
    DTPoint3D Origin(void) const {return origin;}
    double dx(void) const {return _dx;}
    double dy(void) const {return _dy;}
    double dz(void) const {return _dz;}

    bool IsStandard(void) const {return (origin.x==0.0 && origin.y==0.0 && origin.z==0.0 &&
                                         _dx==1.0 && _dy==1.0 && _dz==1.0);}
    bool MaskDefined(void) const {return _maskDefined;}
    bool Mask3DDefined(void) const {return (_maskDefined && !_mask.IsEmpty());}
    ssize_t NumberOfPointsInMask(void) const;
    DTMask Mask(void) const;
    DTMask3D Mask3D(void) const;

    void pinfo(void) const;

private:
    bool isEmpty;
    ssize_t mv,nv,ov;
    DTPoint3D origin;
    double _dx,_dy,_dz;
    bool _maskDefined;
    DTMask mask;
    DTMask3D _mask;
};

extern bool operator==(const DTMesh3DGrid &,const DTMesh3DGrid &);
extern bool operator!=(const DTMesh3DGrid &,const DTMesh3DGrid &);

extern bool SameExceptForMask(const DTMesh3DGrid &A,const DTMesh3DGrid &B);

extern DTRegion3D BoundingBox(const DTMesh3DGrid &v);

extern DTMesh3DGrid GridInsideBox(const DTRegion3D &,double h);
extern DTMesh3DGrid ApplyMask(const DTMesh3DGrid &Grid,const DTMask &);
extern DTMesh3DGrid ApplyMask(const DTMesh3DGrid &Grid,const DTMask3D &);
extern DTMesh3DGrid RemoveMask(const DTMesh3DGrid &Grid);
extern DTMesh3DGrid ChangeSize(const DTMesh3DGrid &Grid,ssize_t m,ssize_t n,ssize_t o);
extern DTMesh3DGrid Crop(const DTMesh3DGrid &,const DTRegion3D &);

extern DTMesh3DGrid KRegion(const DTMesh3DGrid &,const DTRange &);
								 
// Reading and writing
extern bool Read(const DTDataStorage &input,const std::string &name,DTMesh3DGrid &toReturn);
extern void Write(DTDataStorage &output,const std::string &name,const DTMesh3DGrid &theVar);
extern void WriteOne(DTDataStorage &output,const std::string &name,const DTMesh3DGrid &toWrite); // One time value, self documenting.
extern void WriteNoSize(DTDataStorage &output,const std::string &name,const DTMesh3DGrid &theVar);

#endif
