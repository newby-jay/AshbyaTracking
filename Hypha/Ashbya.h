//
//  Ashbya.h
//  filterMesh
//
//  Created by Jay Newby on 8/28/17.
//
//

#ifndef Ashbya_h
#define Ashbya_h

#include "DTTetrahedralGrid3D.h"
#include "DTSurface3D.h"
#include "DTPoint3D.h"
#include "DTDoubleArrayOperators.h"

#include <list>



class Ashbya {
public:
    Ashbya() : tgrid(), sgrid(), surfaceToVolume(), volumes(), tetraSegments(), pointSegments() {}
    Ashbya(const DTTetrahedralGrid3D &Tgrid);
    Ashbya(const DTTetrahedralGrid3D &Tgrid, const DTDoubleArray &Volumes);
    Ashbya(const DTTetrahedralGrid3D &T, const DTSurface3D &S,
           const DTDoubleArray &V, const DTIntArray &StoV);
    Ashbya(const DTTetrahedralGrid3D &T, const DTSurface3D &S,
           const DTDoubleArray &V, const DTIntArray &StoV,
           const DTIntArray &segs, const DTIntArray &pSegs);
    DTTetrahedralGrid3D Tgrid(void) const;
    DTSurface3D Sgrid(void) const;
    DTIntArray SurfaceToVolume(void) const;
    DTIntArray TetraSegments(void) const;
    DTIntArray PointSegments(void) const;
    DTDoubleArray Volumes(void) const;
    int NumberOfSegments(void) const;
    Ashbya addTetraSegments(const DTIntArray &segs);
    Ashbya addPointSegments(const DTIntArray &pSegs);
    void _addVolumes(void);
    void _addSurfaceGrid(void);
private:
    DTTetrahedralGrid3D tgrid;
    DTSurface3D sgrid;
    DTIntArray surfaceToVolume;
    DTDoubleArray volumes;
    DTIntArray tetraSegments;
    DTIntArray pointSegments;
};
//Ashbya operator+=(const Ashbya &A,const Ashbya &B);
Ashbya segmentHyphae(const Ashbya &ash);
Ashbya filterSegmentsByVolume(const Ashbya &ash, double minSegmentVolume);
Ashbya removeTetra(const Ashbya &ash, const DTIntArray &mask);
DTDoubleArray getVolumes(const DTTetrahedralGrid3D &tgrid);
Ashbya removeOrphanPoints(const Ashbya &ash);
Ashbya initialFilterByLength(const Ashbya &ash, double maxEdgeLength);
Ashbya erosionFilterByLength(const Ashbya &ash, double maxEdgeLength);
Ashbya removeOrphanTetras(const Ashbya &ash);
Ashbya renumberSegments(const Ashbya &ash);
#endif /* Ashbya_h */
