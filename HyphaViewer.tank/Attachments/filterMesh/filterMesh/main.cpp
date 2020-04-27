// Include DTSource.h if you want to include all the headers.

#include "DTArguments.h"
#include "DTSaveError.h"

#include "DTDataFile.h"
#include "DTTetrahedralGrid3D.h"
#include "DTSurface3D.h"
#include "DTPoint3D.h"

// Common utilities
#include "DTDoubleArrayOperators.h"
#include "DTProgress.h"
#include "DTTimer.h"
#include "DTUtilities.h"
#include "DTDictionary.h"

#include <math.h>
#include <queue>




class Ashbya {
public:
    Ashbya() : tgrid(), sgrid(), surfaceToVolume(), volumes(), segments() {}
    Ashbya(const DTTetrahedralGrid3D &Tgrid, const DTDoubleArray &Volumes);
    Ashbya(const DTTetrahedralGrid3D &Tgrid, const DTDoubleArray &Volumes, const DTIntArray &segs);
    Ashbya(const DTTetrahedralGrid3D &Tgrid, const DTSurface3D &Sgrid, const DTDoubleArray &Volumes, const DTIntArray &StoV);
    //Ashbya(const DTTetrahedralGrid3D &Tgrid, const DTSurface3D &Sgrid);
    //Ashbya(const DTTetrahedralGrid3D &Tgrid, const DTSurface3D &Sgrid,
    //       const DTDoubleArray &volumes);
    DTTetrahedralGrid3D Tgrid(void) const;
    DTSurface3D Sgrid(void) const;
    DTIntArray SurfaceToVolume(void) const;
    DTIntArray Segments(void) const;
    DTDoubleArray Volumes(void) const;
    int NumberOfSegments(void) const;
    void addSegments(const DTIntArray &segs);
private:
    DTTetrahedralGrid3D tgrid;
    DTSurface3D sgrid;
    DTIntArray surfaceToVolume;
    DTDoubleArray volumes;
    DTIntArray segments;
};
Ashbya segmentHyphae(const Ashbya &ash);
Ashbya filterSegmentsByVolume(const Ashbya &ash, double minSegmentVolume);
Ashbya removeTetra(const Ashbya &ash, const DTIntArray &mask, int NtetsNew);
/////////////////////////// end header /////////////////////////////////
////////////////////////////////////////////////////////////////////////
Ashbya::Ashbya(const DTTetrahedralGrid3D &Tgrid, const DTDoubleArray &Volumes) {
    DTPointCollection3D points = Tgrid.Points();
    DTIntArray tetrahedra = Tgrid.Offsets();
    DTIntArray neighbors = Tgrid.NeighboringTetrahedra();
    size_t Ntets = neighbors.n();
    int Ntriangles = 0;
    for (int n=0; n<Ntets; n++) {
        for (int m=0; m<4; m++) {
            if (neighbors(m, n)==-1) {Ntriangles ++;}
        }
    }
    //0=ABC, 1=ADB, 2=ACD, 3=BDC
    //{{0, 1, 2}, {0, 3, 2}, {0, 2, 3}, {1, 3, 2}}
    DTMutableIntArray triangles(3, Ntriangles);
    DTMutableIntArray SurfaceToVolume(Ntriangles);
    DTMutableFloatArray surfaceNormals(3, Ntriangles);
    int edgeMap[12] = {0, 1, 2, 0, 3, 2, 0, 2, 3, 1, 3, 2};
    for (int ntet=0, ntri=0; ntet<Ntets; ntet++) {
        for (int mtet=0; mtet<4; mtet++) {
            if (neighbors(mtet, ntet)==-1) {
                SurfaceToVolume(ntri) = ntet;
                int npoint1 = tetrahedra(edgeMap[3*mtet + 0], ntet);
                int npoint2 = tetrahedra(edgeMap[3*mtet + 1], ntet);
                int npoint3 = tetrahedra(edgeMap[3*mtet + 2], ntet);
                triangles(0, ntri) = npoint1;
                triangles(1, ntri) = npoint2;
                triangles(2, ntri) = npoint3;
                DTPoint3D nrml = Cross(points(npoint1) - points(npoint3),
                                       points(npoint2) - points(npoint3));
                nrml = nrml/Norm(nrml);
                surfaceNormals(0, ntri) = nrml.x;
                surfaceNormals(1, ntri) = nrml.y;
                surfaceNormals(2, ntri) = nrml.z;
                ntri ++;
            }
        }
    }
    DTSurface3D Sgrid(triangles, points, surfaceNormals);
    sgrid = AddNextTriangleInformation(Sgrid);
    tgrid = Tgrid;
    surfaceToVolume = surfaceToVolume;
    volumes = Volumes;
}
Ashbya::Ashbya(const DTTetrahedralGrid3D &Tgrid, const DTDoubleArray &Volumes, const DTIntArray &segs) {
    DTPointCollection3D points = Tgrid.Points();
    DTIntArray tetrahedra = Tgrid.Offsets();
    DTIntArray neighbors = Tgrid.NeighboringTetrahedra();
    size_t Ntets = neighbors.n();
    int Ntriangles = 0;
    for (int n=0; n<Ntets; n++) {
        for (int m=0; m<4; m++) {
            if (neighbors(m, n)==-1) {Ntriangles ++;}
        }
    }
    // construct surface grid, w pointer triangle -> tetrahedra
    // calculate outward normal for each triangle
    //      n = +- (p2 - p1)X(p3-p1) = u X v
    //          = u2*v3 - u3*v2, u3*v1 - u1*v3, u1*v2 - u2*v1
    //      sign should be oriented opposite the remaining point on the tetrahedra
    //      (p4 - p1) dot n < 0
    //0=ABC, 1=ADB, 2=ACD, 3=BDC
    //{{0, 1, 2}, {0, 3, 2}, {0, 2, 3}, {1, 3, 2}}
    DTMutableIntArray triangles(3, Ntriangles);
    DTMutableIntArray SurfaceToVolume(Ntriangles);
    DTMutableFloatArray surfaceNormals(3, Ntriangles);
    int edgeMap[12] = {0, 1, 2, 0, 3, 2, 0, 2, 3, 1, 3, 2};
    for (int ntet=0, ntri=0; ntet<Ntets; ntet++) {
        for (int mtet=0; mtet<4; mtet++) {
            if (neighbors(mtet, ntet)==-1) {
                SurfaceToVolume(ntri) = ntet;
                int npoint1 = tetrahedra(edgeMap[3*mtet + 0], ntet);
                int npoint2 = tetrahedra(edgeMap[3*mtet + 1], ntet);
                int npoint3 = tetrahedra(edgeMap[3*mtet + 2], ntet);
                triangles(0, ntri) = npoint1;
                triangles(1, ntri) = npoint2;
                triangles(2, ntri) = npoint3;
                DTPoint3D nrml = Cross(points(npoint1) - points(npoint3),
                                       points(npoint2) - points(npoint3));
                nrml = nrml/Norm(nrml);
                surfaceNormals(0, ntri) = nrml.x;
                surfaceNormals(1, ntri) = nrml.y;
                surfaceNormals(2, ntri) = nrml.z;
                ntri ++;
            }
        }
    }
    DTSurface3D Sgrid(triangles, points, surfaceNormals);
    sgrid = AddNextTriangleInformation(Sgrid);
    tgrid = Tgrid;
    surfaceToVolume = surfaceToVolume;
    volumes = Volumes;
    segments = segs;
}
Ashbya::Ashbya(const DTTetrahedralGrid3D &T, const DTSurface3D &S,
               const DTDoubleArray &V, const DTIntArray &StoV) {
    sgrid = S;
    tgrid = T;
    surfaceToVolume = StoV;
    volumes = V;
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
DTIntArray Ashbya::Segments(void) const
{
    return segments;
}
void Ashbya::addSegments(const DTIntArray &segs)
{
    segments = segs;
}
int Ashbya::NumberOfSegments(void) const
{
    int max = 0;
    size_t Ntets = tgrid.Offsets().n();
    for (int n=0; n<Ntets; n++) {
        if (segments(n)>max) {max = segments(n);}
    }
    return max;
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
    int *isTetOpened = (int*) calloc(Ntets, sizeof(int));
    int currentSegNumber = 1;
    std::queue<int> openTets;
    while (NsegedStart<Ntets) {
        // get starting tet to begin new segment
        while (segedTets(NsegedStart)>0) {NsegedStart++;}
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
        currentSegNumber++;
    }
    //Ashbya toReturn = filterSegmentsByVolume(currentSegNumber, segedTets, Tgrid, volumes);
    Ashbya toReturn(ash.Tgrid(), ash.Sgrid(), ash.Volumes(), ash.SurfaceToVolume());
    toReturn.addSegments(segedTets);
    free(isTetOpened);
    return toReturn;
}
Ashbya filterSegmentsByVolume(const Ashbya &ash, double minSegmentVolume)
{
    DTTetrahedralGrid3D Tgrid = ash.Tgrid();
    int Nsegs = ash.NumberOfSegments();
    DTDoubleArray Volumes = ash.Volumes();
    DTIntArray segedTets = ash.Segments();
    DTPointCollection3D points = Tgrid.Points();
    DTIntArray tetrahedra = Tgrid.Offsets();
    DTIntArray neighbors = Tgrid.NeighboringTetrahedra();
    size_t Ntets = neighbors.n();
    ////////////////////////////////////////////////////
    //// determine total volume of each segmented region
    DTMutableDoubleArray segmentVolumes(Nsegs);
    segmentVolumes = 0.;
    for (int n=0; n<Ntets; n++) {segmentVolumes(segedTets(n)) += Volumes(n);}
    ////////////////////////////////////////////////////////
    //// filter segments that are less than minSegmentVolume
    DTMutableIntArray segmentMask(Nsegs);
    for (int r=0; r<Nsegs; r++) {
        segmentMask(r) = (segmentVolumes(r)<minSegmentVolume) ? 0:1;
    }
    int nNew = 0;
    for (int n=0; n<Ntets; n++) {
        if (segmentMask(segedTets(n))==1) {nNew++;}
    }
    Ashbya toReturn = removeTetra(ash, segmentMask, nNew);
    return toReturn;
}
Ashbya removeTetra(const Ashbya &ash, const DTIntArray &mask, int NtetsNew) {
    DTTetrahedralGrid3D Tgrid = ash.Tgrid();
    DTIntArray tetrahedra = Tgrid.Offsets();
    size_t Ntets = tetrahedra.n();
    DTDoubleArray Volumes = ash.Volumes();
    DTIntArray segedTets = ash.Segments();
    
    DTMutableIntArray newTets(4, NtetsNew);
    DTMutableIntArray newSegedTets(NtetsNew);
    DTMutableDoubleArray newVolumes(NtetsNew);
    int nNew = 0;
    for (int n=0; n<Ntets; n++) {
        if (mask(segedTets(n))==1) {
            newSegedTets(nNew) = segedTets(n);
            newVolumes(nNew) = Volumes(n);
            for (int m=0;m<4;m++) {newTets(m, nNew) = tetrahedra(m, n);}
            nNew++;
        }
    }
    DTTetrahedralGrid3D newGrid = AddNextTetrahedra(DTTetrahedralGrid3D(newTets, Tgrid.Points()));
    Ashbya toReturn(newGrid, newVolumes, newSegedTets);
    return toReturn;
}


//////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////
DTTetrahedralGrid3D Computation(const DTTetrahedralGrid3D &Tgrid, const DTDoubleArray &volumes, double maxEdgeLength);
Ashbya filterByLength(const Ashbya &ash, double maxEdgeLength);
//DTTetrahedralGrid3D getNextTetras(const DTTetrahedralGrid3D &Tgrid);
//////////////////////////////////////////////////////////////////////////////////////
int main(int argc,const char *argv[])
{
    DTSetArguments(argc,argv);
    
    DTDataFile inputFile("Input.dtbin",DTFile::ReadOnly);
    // Read in the input variables.
    DTTetrahedralGrid3D Tgrid;
    Read(inputFile,"Tgrid",Tgrid);
    DTDoubleArray volumes = inputFile.ReadDoubleArray("volumes");
    //double maxVolume = inputFile.ReadNumber("maxVolume");
    double maxEdgeLength = inputFile.ReadNumber("maxEdgeLength");
    
    // The computation.
    DTTetrahedralGrid3D computed;
    clock_t t_before = clock();
    computed = Computation(Tgrid,volumes,maxEdgeLength);
    clock_t t_after = clock();
    double exec_time = double(t_after-t_before)/double(CLOCKS_PER_SEC);
    
    // Write the output.
    DTDataFile outputFile("Output.dtbin",DTFile::NewReadWrite);
    
    // Output from computation
    Write(outputFile,"Var",computed);
    outputFile.Save("TetrahedralGrid3D","Seq_Var");
    
    // The execution time.
    outputFile.Save(exec_time,"ExecutionTime");
    outputFile.Save("Real Number","Seq_ExecutionTime");
    
    // The errors.
    DTSaveError(outputFile,"ExecutionErrors");
    outputFile.Save("StringList","Seq_ExecutionErrors");
    
    outputFile.SaveIndex();
    
    return 0;
}
DTTetrahedralGrid3D Computation(const DTTetrahedralGrid3D &Tgrid, const DTDoubleArray &volumes, double maxEdgeLength)
{
    double minSegmentVolume = 1000.;
    
    Ashbya ash(Tgrid, volumes);
    ash = segmentHyphae(ash);
    ash = filterSegmentsByVolume(ash, minSegmentVolume);
    Ashbya newAsh = filterByLength(ash, maxEdgeLength);
    DTTetrahedralGrid3D toReturn = newAsh.Tgrid();
    return toReturn;
}
Ashbya filterByLength(const Ashbya &ash, double maxEdgeLength)
{
    DTTetrahedralGrid3D Tgrid = ash.Tgrid();
    DTIntArray tetrahedra = Tgrid.Offsets();
    size_t Ntets = tetrahedra.n();
    DTPointCollection3D points = Tgrid.Points();
    size_t Npoints = points.NumberOfPoints();
    DTIntArray neighbors = Tgrid.NeighboringTetrahedra();
    DTMutableIntArray newNeighbors = neighbors.Copy();
    //DTIntArray StoV = ash.SurfaceToVolume();
    //size_t Ntri = StoV.n();
    
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
    for (int i=0, allFiltered=0; ((allFiltered==0) && (i<100)); i++) {
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
            }
            Nfiltered += 1 - filterMask(n);
            //// update surface mask and number of tetra per point
            for (int m=0; m<4; m++) {
                tetsPerPoint(tetrahedra(m, n)) -= 1;
                int p = newNeighbors(m, n);
                if (p<0) continue;
                //// if p>=0 then neighbors(m, n) = 4*u + v and neighbors(u, v) = 4*n + m
                int nextTetra = p/4;
                isSurface(nextTetra) = 1;
                newNeighbors(p%4, nextTetra) = -1;
            }
        }
        Nfiltered += filterCounter;
        if (filterCounter==0) allFiltered = 1;
    }
    int NnewCells = Ntets - Nfiltered;
    Ashbya toReturn = removeTetra(ash, filterMask, NnewCells);
    return toReturn;
}







//DTTetrahedralGrid3D getNextTetras(const DTTetrahedralGrid3D &Tgrid)
//{
//    //// way too slow, David gave us an NlogN version
//    DTPointCollection3D points = Tgrid.Points();
//    DTIntArray offsets = Tgrid.Offsets();
//    size_t Ncells = offsets.n();
//    
//    DTMutableIntArray nextTetra(4, Ncells);
//    for (int jc=0;jc<Ncells;jc++) {
//        for (int jv=0;jv<4;jv++) {
//            nextTetra(jv, jc) = -1;
//        }
//    }
//    int V[12] = {0, 1, 2, 0, 3, 2, 0, 2, 3, 1, 3, 2};
//    for (int jc=0;jc<Ncells;jc++) {
//        // offset = point index of A, B, C, D
//        for (int kc=jc+1;kc<Ncells;kc++) {
//            if ((nextTetra(0, jc)>=0) && (nextTetra(1, jc)>=0)
//                && (nextTetra(2, jc)>=0) && (nextTetra(3, jc)>=0)) break;
//            // side index: 0=ABC, 1=ADB, 2=ACD, 3=BDC
//            for (int n=0;n<4;n++) {
//                for (int m=0; ((m<4)&&(nextTetra(n, jc)==-1)); m++) {
//                    if ((offsets(V[3*n + 0], jc) == offsets(V[3*m + 0], kc)) &&
//                        (offsets(V[3*n + 1], jc) == offsets(V[3*m + 2], kc)) &&
//                        (offsets(V[3*n + 2], jc) == offsets(V[3*m + 1], kc))) {
//                        nextTetra(n, jc) = 4*kc + m;
//                        nextTetra(m, kc) = 4*jc + n;
//                    }
//                }
//            }
//            // nextTetra(jv, jc) = 4*(tetra index of neighbor) + (side index of neighbor)
//        }
//    }
//    DTTetrahedralGrid3D toReturn(offsets, points, nextTetra);
//    return toReturn;
//}
