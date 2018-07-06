//
//  Ashbya.cpp
//  filterMesh
//
//  Created by Jay Newby on 8/28/17.
//
//
#include "Ashbya.h"
#include <math.h>
#include <queue>

Ashbya::Ashbya(const DTTetrahedralGrid3D &Tgrid) {
    tgrid = Tgrid;
    _addVolumes();
    _addSurfaceGrid();
    DTMutableIntArray pSegs(Tgrid.Points().NumberOfPoints());
    pSegs = 0;
    pointSegments = pSegs;
    
    DTMutableIntArray tSegs(Tgrid.Offsets().n());
    tSegs = 0;
    tetraSegments = tSegs;
}

Ashbya::Ashbya(const DTTetrahedralGrid3D &Tgrid, const DTDoubleArray &Volumes) {
    tgrid = Tgrid;
    volumes = Volumes;
    _addSurfaceGrid();
    
    DTMutableIntArray pSegs(Tgrid.Points().NumberOfPoints());
    pSegs = 0;
    pointSegments = pSegs;
    
    DTMutableIntArray tSegs(Tgrid.Offsets().n());
    tSegs = 0;
    tetraSegments = tSegs;
}

Ashbya::Ashbya(const DTTetrahedralGrid3D &Tgrid, const DTSurface3D &S,
               const DTDoubleArray &V, const DTIntArray &StoV) {
    sgrid = S;
    tgrid = Tgrid;
    surfaceToVolume = StoV;
    volumes = V;
    
    DTMutableIntArray pSegs(Tgrid.Points().NumberOfPoints());
    pSegs = 0;
    pointSegments = pSegs;
    
    DTMutableIntArray tSegs(Tgrid.Offsets().n());
    tSegs = 0;
    tetraSegments = tSegs;
}

Ashbya::Ashbya(const DTTetrahedralGrid3D &T, const DTSurface3D &S,
               const DTDoubleArray &V, const DTIntArray &StoV,
               const DTIntArray &segs, const DTIntArray &pSegs) {
    sgrid = S;
    tgrid = T;
    surfaceToVolume = StoV;
    volumes = V;
    tetraSegments = segs;
    pointSegments = pSegs;
}

DTTetrahedralGrid3D Ashbya::Tgrid(void) const
{
    return tgrid;
}

DTSurface3D Ashbya::Sgrid(void) const
{
    return sgrid;
}

DTIntArray Ashbya::SurfaceToVolume(void) const
{
    return surfaceToVolume;
}

DTDoubleArray Ashbya::Volumes(void) const
{
    return volumes;
}

DTIntArray Ashbya::TetraSegments(void) const
{
    return tetraSegments;
}

DTIntArray Ashbya::PointSegments(void) const
{
    return pointSegments;
}

void Ashbya::_addVolumes(void) {
    volumes = getVolumes(tgrid);
}

void Ashbya::_addSurfaceGrid(void)
{
    DTPointCollection3D points = tgrid.Points();
    DTIntArray tetrahedra = tgrid.Offsets();
    DTIntArray neighbors = tgrid.NeighboringTetrahedra();
    size_t Ntets = neighbors.n();
    int Ntriangles = 0;
    for (int n=0; n<Ntets; n++) {
        for (int m=0; m<4; m++) {
            if (neighbors(m, n)==-1) Ntriangles++;
        }
    }
    DTMutableIntArray triangles(3, Ntriangles);
    DTMutableIntArray StoVol(Ntriangles);
    DTMutableFloatArray surfaceNormals(3, Ntriangles);
    ///// 0=ABC, 1=ADB, 2=ACD, 3=BDC
    int edgeMap[12] = {0, 1, 2, 0, 3, 1, 0, 2, 3, 1, 3, 2};
    for (int ntet=0, ntri=0; ntet<Ntets; ntet++) {
        for (int mtet=0; mtet<4; mtet++) {
            if (neighbors(mtet, ntet) != -1) continue;
            StoVol(ntri) = ntet;
            int npoint1 = tetrahedra(edgeMap[3*mtet + 0], ntet);
            int npoint2 = tetrahedra(edgeMap[3*mtet + 1], ntet);
            int npoint3 = tetrahedra(edgeMap[3*mtet + 2], ntet);
            DTPoint3D nrml = Cross(points(npoint1) - points(npoint3),
                                   points(npoint2) - points(npoint3));
            nrml = nrml/Norm(nrml);
            surfaceNormals(0, ntri) = nrml.x;
            surfaceNormals(1, ntri) = nrml.y;
            surfaceNormals(2, ntri) = nrml.z;
            triangles(2, ntri) = npoint1;
            triangles(1, ntri) = npoint2;
            triangles(0, ntri) = npoint3;
            ntri++;
        }
    }
    //DTSurface3D Sgrid(triangles, points, surfaceNormals);
    DTSurface3D Sgrid(triangles, points);
    sgrid = AddNextTriangleInformation(Sgrid);
    surfaceToVolume = StoVol;
}

Ashbya Ashbya::addTetraSegments(const DTIntArray &segs)
{
    Ashbya toReturn(tgrid, sgrid, volumes, surfaceToVolume, segs, pointSegments);
    return toReturn;
}

Ashbya Ashbya::addPointSegments(const DTIntArray &pSegs)
{
    Ashbya toReturn(tgrid, sgrid, volumes, surfaceToVolume, tetraSegments, pSegs);
    return toReturn;
}

int Ashbya::NumberOfSegments(void) const
{
    int max = 0;
    size_t Ntets = tgrid.Offsets().n();
    for (int n=0; n<Ntets; n++) {
        if (tetraSegments(n)>max) max = tetraSegments(n);
    }
    return max;
}

DTDoubleArray getVolumes(const DTTetrahedralGrid3D &tgrid)
{
    DTPointCollection3D points = tgrid.Points();
    DTIntArray tetrahedra = tgrid.Offsets();
    DTIntArray neighbors = tgrid.NeighboringTetrahedra();
    size_t Ntets = neighbors.n();
    DTMutableDoubleArray vols(Ntets);
    for (int n=0; n<Ntets; n++) {
        DTPoint3D p1 = points(tetrahedra(0, n));
        DTPoint3D p2 = points(tetrahedra(1, n));
        DTPoint3D p3 = points(tetrahedra(2, n));
        vols(n) = Dot(p1, Cross(p2, p3))/6.;
    }
    return vols;
}

Ashbya segmentHyphae(const Ashbya &ash)
{
    DTTetrahedralGrid3D Tgrid = ash.Tgrid();
    DTDoubleArray volumes = ash.Volumes();
    DTPointCollection3D points = Tgrid.Points();
    DTIntArray tetrahedra = Tgrid.Offsets();
    DTIntArray neighbors = Tgrid.NeighboringTetrahedra();
    size_t Ntets = neighbors.n();
    ///////////////////////////////////////////////////////////////////
    //// Segment tetrahedra, assigning a unique integer to each segment
    int NsegedStart = 0;
    DTMutableIntArray segedTets(Ntets);
    segedTets = 0;
    int *isTetOpened = (int*) calloc(Ntets, sizeof(int));
    int currentSegNumber = 1;
    std::queue<int> openTets;
    while (NsegedStart<Ntets) {
        // get starting tet to begin new segment
        while ((NsegedStart<Ntets) && (segedTets(NsegedStart)>0)) {NsegedStart++;}
        // add first tet to the queue
        if (NsegedStart<Ntets) {openTets.push(NsegedStart);}
        // continue segmenting tetrahedra at the front of the queue, while adding unopened nearest neighbor tets to the end of the queue
        while (openTets.empty()==false) {
            int tet = openTets.front();
            openTets.pop();
            segedTets(tet) = currentSegNumber;
            for (int n=0; n<4; n++) {
                int p = neighbors(n, tet);
                int nextTet = p/4;
                if ((p>-1) && (isTetOpened[nextTet]==0)) {
                    isTetOpened[nextTet] = 1;
                    openTets.push(nextTet);
                }
            }
        }
        // once queue is empty, start a new segment
        if (NsegedStart<Ntets) currentSegNumber++;
    }
    //Ashbya toReturn = filterSegmentsByVolume(currentSegNumber, segedTets, Tgrid, volumes);
    Ashbya toReturn(ash.Tgrid(), ash.Sgrid(), ash.Volumes(), ash.SurfaceToVolume(), segedTets, ash.PointSegments());
    //toReturn.addSegments(segedTets);
    int max = 0;
    for (int n=0; n<Ntets; n++) {
        if (segedTets(n)>max) max = segedTets(n);
    }
    free(isTetOpened);
    return toReturn;
}

Ashbya filterSegmentsByVolume(const Ashbya &ash, double minSegmentVolume)
{
    DTTetrahedralGrid3D Tgrid = ash.Tgrid();
    int Nsegs = ash.NumberOfSegments();
    DTDoubleArray Volumes = ash.Volumes();
    DTIntArray segedTets = ash.TetraSegments();
    DTPointCollection3D points = Tgrid.Points();
    DTIntArray tetrahedra = Tgrid.Offsets();
    DTIntArray neighbors = Tgrid.NeighboringTetrahedra();
    size_t Ntets = neighbors.n();
    ////////////////////////////////////////////////////
    //// determine total volume of each segmented region
    DTMutableDoubleArray segmentVolumes(Nsegs);
    segmentVolumes = 0.;
    for (int n=0; n<Ntets; n++) {segmentVolumes(segedTets(n)-1) += Volumes(n);}
    ////////////////////////////////////////////////////////
    //// filter segments that are less than minSegmentVolume
    DTMutableIntArray segmentMask(Nsegs);
    for (int r=0; r<Nsegs; r++) {
        segmentMask(r) = (segmentVolumes(r)<minSegmentVolume) ? 0:1;
    }
    int nNew = 0;
    DTMutableIntArray mask(Ntets);
    mask = 0;
    for (int n=0; n<Ntets; n++) {
        if (segmentMask(segedTets(n)-1)==1) {
            mask(n) = 1;
            nNew++;
        }
    }
    Ashbya toReturn = removeTetra(ash, mask);
    return toReturn;
}

Ashbya removeOrphanPoints(const Ashbya &ash)
{
    DTTetrahedralGrid3D Tgrid = ash.Tgrid();
    DTPointCollection3D points = Tgrid.Points();
    size_t Npoints = points.NumberOfPoints();
    DTIntArray tetrahedra = Tgrid.Offsets();
    size_t Ntets = tetrahedra.n();
    DTIntArray segedPoints = ash.PointSegments();
    
    ///// determine what points and tetra to filter out
    DTMutableIntArray keepPoints(Npoints);
    keepPoints = 0;
    for (int n=0; n<Ntets; n++) {
        for (int m=0;m<4;m++) {
            keepPoints(tetrahedra(m, n)) = 1;
        }
    }
    //// count number of new points
    int NnewPoints = 0;
    DTMutableIntArray oldToNew(Npoints);
    oldToNew = 0;
    for (int n=0; n<Npoints; n++) {
        if (keepPoints(n)==1) NnewPoints++;
    }
    // collect new points into DTPointCollection3D
    // mapping between old points to new points
    DTDoubleArray pointData = ConvertToDouble(points).DoubleData();
    DTMutableDoubleArray newPointData(3, NnewPoints);
    DTMutableIntArray newPointSegs(NnewPoints);
    DTMutableIntArray pointNumbers(NnewPoints);
    for (int n=0, ns=0; n<Npoints; n++) {
        if (keepPoints(n)==0) continue;
        oldToNew(n) = ns;
        for (int m=0; m<3; m++) {
            newPointData(m, ns) = pointData(m, n);
        }
        newPointSegs(ns) = segedPoints(n);
        pointNumbers(ns) = n;
        ns++;
    }
    DTPointCollection3D newPoints(newPointData, pointNumbers);
    // renumber tetrahedra array using mapping
    DTMutableIntArray NewTetrahedra(4, Ntets);
    for (int n=0; n<Ntets; n++) {
        for (int m=0; m<4; m++) {
            NewTetrahedra(m, n) = oldToNew(tetrahedra(m, n));
        }
    }
    DTTetrahedralGrid3D newGrid = AddNextTetrahedra(DTTetrahedralGrid3D(NewTetrahedra, newPoints));
    Ashbya toReturn(newGrid, ash.Volumes());
    return toReturn.addTetraSegments(ash.TetraSegments()).addPointSegments(newPointSegs);
}

Ashbya removeOrphanTetras(const Ashbya &ash)
{
    DTTetrahedralGrid3D Tgrid = ash.Tgrid();
    DTIntArray tetrahedra = Tgrid.Offsets();
    size_t Ntets = tetrahedra.n();
    DTIntArray neighbors = Tgrid.NeighboringTetrahedra();
    DTMutableIntArray filterMask(Ntets);
    filterMask = 1;
    for (int n=0; n<Ntets; n++) {
        int checkTetra=0;
        for (int m=0; m<4; m++) {
            if (neighbors(m, n)==-1) checkTetra++;
        }
        if (checkTetra>=3) filterMask(n) = 0;
    }
    Ashbya toReturn = removeTetra(ash, filterMask);
    return toReturn;
}

Ashbya removeTetra(const Ashbya &ash, const DTIntArray &mask) {
    /////// 1 is keep, 0 is eliminate
    int NtetsNew = 0;
    for (int n=0; n<mask.m(); n++) {NtetsNew+=mask(n);}
    DTTetrahedralGrid3D Tgrid = ash.Tgrid();
    DTIntArray tetrahedra = Tgrid.Offsets();
    size_t Ntets = tetrahedra.n();
    DTDoubleArray Volumes = ash.Volumes();
    DTIntArray segedTets = ash.TetraSegments();
    
    DTMutableIntArray newTets(4, NtetsNew);
    DTMutableIntArray newSegedTets(NtetsNew);
    DTMutableDoubleArray newVolumes(NtetsNew);
    ///// determine what points and tetra to filter out
    for (int n=0, ns=0; n<Ntets; n++) {
        if (mask(n)!=1) continue;
        newSegedTets(ns) = segedTets(n);
        newVolumes(ns) = Volumes(n);
        for (int m=0;m<4;m++) {
            newTets(m, ns) = tetrahedra(m, n);
        }
        ns++;
    }
    DTTetrahedralGrid3D newGrid = DTTetrahedralGrid3D(newTets, Tgrid.Points());
    newGrid = AddNextTetrahedra(newGrid);
    Ashbya toReturn(newGrid, newVolumes);
    return toReturn.addTetraSegments(newSegedTets).addPointSegments(ash.PointSegments());
}

Ashbya initialFilterByLength(const Ashbya &ash, double maxEdgeLength)
{
    DTTetrahedralGrid3D Tgrid = ash.Tgrid();
    DTIntArray tetrahedra = Tgrid.Offsets();
    size_t Ntets = tetrahedra.n();
    DTPointCollection3D points = Tgrid.Points();
    DTIntArray neighbors = Tgrid.NeighboringTetrahedra();
    DTMutableIntArray newNeighbors = neighbors.Copy();
    int Nfiltered = 0;
    DTMutableIntArray filterMask(Ntets);
    filterMask = 1;
    //// scan over surface for tetra to filter
    for (int n=0;n<Ntets;n++) {
        double dist = 0;
        for (int m=0;m<4;m++) {
            for (int j=0;j<m;j++) {
                double distNew = Distance(points(tetrahedra(m, n)), points(tetrahedra(j, n)));
                dist = (distNew>dist) ? distNew:dist;
            }
        }
        if (dist>maxEdgeLength) {
            filterMask(n) = 0;
            Nfiltered ++;
        }
    }
    Ashbya toReturn = removeTetra(ash, filterMask);
    return toReturn;
}

Ashbya renumberSegments(const Ashbya &ash)
{
    DTIntArray psegs = ash.PointSegments();
    DTIntArray tsegs = ash.TetraSegments();
    int Npoints = psegs.m();
    int Ntetra = tsegs.m();
    std::list<int> currentSegs;
    int maxSeg = 0;
    for (int n=0; n<Npoints; n++) {
        int haveIt = 0;
        for (int i : currentSegs) {
            if (psegs(n) == i) {
                haveIt = 1;
                break;
            }
        }
        if (haveIt == 0) currentSegs.push_back(psegs(n));
        if (psegs(n) > maxSeg) maxSeg = psegs(n);
    }
    int Nsegs = currentSegs.size();
    DTMutableIntArray segMap(maxSeg + 1);
    int inew = 1;
    for (int i : currentSegs) {
        segMap(i) = inew;
        inew ++;
    }
    DTMutableIntArray newPsegs(Npoints);
    for (int n=0; n<Npoints; n++) {
        newPsegs(n) = segMap(psegs(n));
    }
    DTMutableIntArray newTsegs(Ntetra);
    for (int n=0; n<Ntetra; n++) {
        newTsegs(n) = segMap(tsegs(n));
    }
    Ashbya toReturn(ash.Tgrid(), ash.Volumes());
    return toReturn.addTetraSegments(newTsegs).addPointSegments(newPsegs);
}

Ashbya erosionFilterByLength(const Ashbya &ash, double maxEdgeLength)
{
    DTTetrahedralGrid3D Tgrid = ash.Tgrid();
    DTIntArray tetrahedra = Tgrid.Offsets();
    size_t Ntets = tetrahedra.n();
    DTPointCollection3D points = Tgrid.Points();
    size_t Npoints = points.NumberOfPoints();
    DTIntArray neighbors = Tgrid.NeighboringTetrahedra();
    DTMutableIntArray newNeighbors = neighbors.Copy();
    ///// which tetra are surface and number of tetra each point is in
    DTMutableIntArray tetsPerPoint(Npoints);
    tetsPerPoint = 0;
    DTMutableIntArray isSurface(Ntets);
    isSurface = 0;
    for (int n=0; n<Ntets; n++) {
        for (int m=0; m<4; m++) {
            tetsPerPoint(tetrahedra(m, n)) += 1;
            if (neighbors(m, n)==-1) {isSurface(n) += 1;}
        }
    }
    int Nfiltered = 0;
    DTMutableIntArray filterMask(Ntets);
    filterMask = 1;
    for (int i=0, allFiltered=0; ((allFiltered==0) && (i<1000)); i++) {
        //// scan over surface for tetra to filter
        int filterCounter = 0;
        for (int n=0;n<Ntets;n++) {
            if ((isSurface(n)==0) || (filterMask(n)==0)) continue;
            int hasOrphan = 0;
            double dist = 0;
            for (int m=0;m<4;m++) {
                hasOrphan = (tetsPerPoint(tetrahedra(m, n))>1)? 0:1;
                for (int j=0;j<m;j++) {
                    double distNew = Distance(points(tetrahedra(m, n)), points(tetrahedra(j, n)));
                    dist = (distNew>dist) ? distNew:dist;
                }
            }
            if ((dist>maxEdgeLength) && (hasOrphan==0)) {
                filterMask(n) = 0;
                filterCounter ++;
                //// update surface mask and number of tetra per point
                for (int m=0; m<4; m++) {
                    tetsPerPoint(tetrahedra(m, n)) -= 1;
                    int p = newNeighbors(m, n);
                    if (p<0) continue;
                    //// if p>=0 then neighbors(m, n) = 4*u + v and neighbors(u, v) = 4*n + m
                    int nextTetra = p/4;
                    isSurface(nextTetra) = 1;
                    //newNeighbors(p%4, nextTetra) = -1;
                }
            }
        }
        Nfiltered += filterCounter;
        if (filterCounter==0) allFiltered = 1;
    }
    Ashbya toReturn = removeTetra(ash, filterMask);
    return toReturn;
}

//Ashbya operator+=(const Ashbya &A,const Ashbya &B)
//{
//    DTTetrahedralGrid3D Agrid = A.Tgrid(), Bgrid = B.Tgrid();
//    DTPointCollection3D Apoints = Agrid.Points(), Bpoints = Bgrid.Points();
//    int NApoints = Apoints.NumberOfPoints(), NBpoints = Bpoints.NumberOfPoints();
//    DTIntArray Atetrahedra = Agrid.Offsets(), Btetrahedra = Bgrid.Offsets();
//    int NAtets = Atetrahedra.n(), NBtets = Btetrahedra.n();
//    DTDoubleArray APdata = Apoints.DoubleData(), BPdata = Bpoints.DoubleData();
//    //////// combine point sets
//    DTMutableDoubleArray outPdata(3, NApoints+NBpoints);
//    for (int n=0; n<NApoints; n++) {
//        for (int m=0; m<3; m++) {
//            outPdata(m, n) = APdata(m, n);
//        }
//    }
//    for (int n=0; n<NBpoints; n++) {
//        for (int m=0; m<3; m++) {
//            outPdata(m, NApoints + n) = BPdata(m, n);
//        }
//    }
//    DTPointCollection3D pointsOut(outPdata);
//    ////// combine offsets
//    DTMutableIntArray outTetrahedra(4, NAtets+NBtets);
//    for (int n=0; n<NAtets; n++) {
//        for (int m=0; m<4; m++) {
//            outTetrahedra(m, n) = Atetrahedra(m, n);
//        }
//    }
//    for (int n=0; n<NBtets; n++) {
//        for (int m=0; m<4; m++) {
//            outTetrahedra(m, NAtets + n) = NAtets + BPdata(m, n);
//        }
//    }
//    DTTetrahedralGrid3D tgridOut(outTetrahedra, pointsOut);
//    Ashbya toReturn(tgridOut);
//    return toReturn;
//}
//////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////
//DTSurface3D operator+(const DTSurface3D &A,const DTSurface3D &B)
//{
//    DTMutableList<DTSurface3D> L(2);
//    L(0) = A;
//    L(1) = B;
//    return Combine(L);
//}
//
//DTSurface3D Combine(const DTList<DTSurface3D> &L)
//{
//    int surfN;
//    ssize_t howManySurfaces = L.Length();
//    int howManyPoints = 0;
//    int howManyTriangles = 0;
//    DTSurface3D S;
//    bool hasNormals = true;
//    for (surfN=0;surfN<howManySurfaces;surfN++) {
//        S = L(surfN);
//        howManyPoints += S.NumberOfPoints();
//        howManyTriangles += S.NumberOfTriangles();
//        if (S.NormalsDefined()==false) hasNormals = false;
//    }
//    
//    DTMutableFloatArray newPoints(3,howManyPoints);
//    DTMutableFloatArray newNormals(3,hasNormals ? howManyPoints : 0);
//    DTMutableIntArray newTriangles(3,howManyTriangles);
//    int posInPoints = 0;
//    int posInTriangles = 0;
//    for (surfN=0;surfN<howManySurfaces;surfN++) {
//        S = L(surfN);
//        MemoryCopy(newPoints,3*posInPoints,S.Points().FloatData());
//        CopyValuesIntoAndAdd(newTriangles,3*posInTriangles,S.Connections(),posInPoints);
//        if (hasNormals)
//            MemoryCopy(newNormals,3*posInPoints,S.Normals());
//        posInPoints += S.NumberOfPoints();
//        posInTriangles += S.NumberOfTriangles();
//    }
//    
//    if (hasNormals)
//        return DTSurface3D(newTriangles,DTPointCollection3D(newPoints),newNormals);
//    else
//        return DTSurface3D(newTriangles,DTPointCollection3D(newPoints));
//}
