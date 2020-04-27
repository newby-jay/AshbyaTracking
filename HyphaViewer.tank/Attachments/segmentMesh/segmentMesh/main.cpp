// Include DTSource.h if you want to include all the headers.

#include "DTArguments.h"
#include "DTSaveError.h"

#include "DTDataFile.h"
#include "DTDoubleArray.h"
#include "DTTetrahedralGrid3D.h"

// Common utilities
#include "DTDoubleArrayOperators.h"
#include "DTProgress.h"
#include "DTTimer.h"
#include "DTUtilities.h"
#include "DTDictionary.h"

#include <math.h>
#include <queue>

DTDoubleArray Computation(const DTTetrahedralGrid3D &Tgrid,const DTDoubleArray &Volumes);

int main(int argc,const char *argv[])
{
    DTSetArguments(argc,argv);

    DTDataFile inputFile("Input.dtbin",DTFile::ReadOnly);
    // Read in the input variables.
    DTTetrahedralGrid3D Tgrid;
    Read(inputFile,"Tgrid",Tgrid);
    DTDoubleArray Volumes = inputFile.ReadDoubleArray("Volumes");

    // The computation.
    DTDoubleArray computed;
    clock_t t_before = clock();
    computed = Computation(Tgrid,Volumes);
    clock_t t_after = clock();
    double exec_time = double(t_after-t_before)/double(CLOCKS_PER_SEC);

    // Write the output.
    DTDataFile outputFile("Output.dtbin",DTFile::NewReadWrite);

    // Output from computation
    outputFile.Save(computed,"Var");
    outputFile.Save("Array","Seq_Var");

    // The execution time.
    outputFile.Save(exec_time,"ExecutionTime");
    outputFile.Save("Real Number","Seq_ExecutionTime");

    // The errors.
    DTSaveError(outputFile,"ExecutionErrors");
    outputFile.Save("StringList","Seq_ExecutionErrors");

    outputFile.SaveIndex();

    return 0;
}

DTDoubleArray Computation(const DTTetrahedralGrid3D &Tgrid, const DTDoubleArray &Volumes)
{
    DTPointCollection3D points = Tgrid.Points();
    DTIntArray tetrahedra = Tgrid.Offsets();
    DTIntArray neighbors = Tgrid.NeighboringTetrahedra();
    size_t Ntets = neighbors.n(), Npoints = points.NumberOfPoints();
    ///////////////////////////////////////////////////////////////////
    //// Segment tetrahedra, assigning a unique integer to each segment
    int NsegedStart = 0;
    int *segedTets = (int*) calloc(Ntets, sizeof(int));
    int *isTetOpened = (int*) calloc(Ntets, sizeof(int));
    int currentSegNumber = 1;
    std::queue<int> openTets;
    while (NsegedStart<Ntets) {
        // get starting tet to begin new segment
        while (segedTets[NsegedStart]>0) {NsegedStart++;}
        // add first tet to the queue
        if (NsegedStart<Ntets) {openTets.push(NsegedStart);}
        // continue segmenting tetrahedra at the front of the queue, while adding unopened nearest neighbor tets to the end of the queue
        while (openTets.empty()==false) {
            int tet = openTets.front();
            openTets.pop();
            segedTets[tet] = currentSegNumber;
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
    DTTetrahedralGrid3D newGrid = filterByVolume(currentSegNumber, segedTets, Tgrid, Volumes);
    /// need to compute filtered volumes
    
    
    DTMutableDoubleArray toReturn(Ntets);
    for (int n=0;n<Ntets;n++) {toReturn(n) = segedTets[n];}
    free(isTetOpened);
    free(segedTets);
    return toReturn;
}
DTTetrahedralGrid3D filterByVolume(int Nsegs, double *segedTets,
                              const DTTetrahedralGrid3D &Tgrid,
                              const DTDoubleArray &Volumes)
{
    DTPointCollection3D points = Tgrid.Points();
    DTIntArray tetrahedra = Tgrid.Offsets();
    DTIntArray neighbors = Tgrid.NeighboringTetrahedra();
    size_t Ntets = neighbors.n(), Npoints = points.NumberOfPoints();
    ////////////////////////////////////////////////////
    //// determine total volume of each segmented region
    double minSegmentVolume = 1000.;
    DTMutableDoubleArray segmentVolumes(Nsegs);
    segmentVolumes = 0.;
    for (int n=0; n<Ntets; n++) {segmentVolumes(segedTets[n]) += Volumes(n);}
    ////////////////////////////////////////////////////////
    //// filter segments that are less than minSegmentVolume
    DTMutableIntArray segmentMask(Nsegs);
    for (int r=0; r<Nsegs; r++) {
        segmentMask(r) = (segmentVolumes(r)<minSegmentVolume) ? 0:1;
    }
    int nNew = 0;
    for (int n=0; n<Ntets; n++) {
        if (segmentMask(segedTets[n])==1) {nNew++;}
    }
    int NtetsNew = nNew;
    DTMutableIntArray newTets(4, NtetsNew);
    DTMutableIntArray newSegedTets(NtetsNew);
    DTMutableDoubleArray newVolumes(NtetsNew);
    nNew = 0;
    for (int n=0; n<Ntets; n++) {
        if (segmentMask(segedTets[n])==1) {
            newSegedTets(nNew) = segedTets[n];
            newVolumes(nNew) = Volumes(n);
            for (int m=0;m<4;m++) {newTets(m, nNew) = tetrahedra(m, n);}
            nNew++;
        }
    }
    DTTetrahedralGrid3D newGrid = AddNextTetrahedra(DTTetrahedralGrid3D(newTets, points));
    return newGrid, newSegedTets, newVolumes;
}
DTDoubleArray filterBySurface(double *segedTets, const DTTetrahedralGrid3D &Tgrid, const DTDoubleArray &Volumes)
{
    DTPointCollection3D points = Tgrid.Points();
    DTIntArray tetrahedra = Tgrid.Offsets();
    DTIntArray neighbors = Tgrid.NeighboringTetrahedra();
    size_t Ntets = neighbors.n(), Npoints = points.NumberOfPoints();
    ////////////////////////////////////////////////////
    //// compute number of tetrahedra each point belongs to
    DTMutableIntArray tetsPerPoint(Npoints);
    tetsPerPoint = 0;
    DTMutableIntArray isSurface(Ntets);
    isSurface = 0;
    for (int n=0; n<Ntets; n++) {
        for (int m=0; m<4; m++) {
            tetsPerPoint(tetrahedra(m, n)) += 1;
            if (neighbors(m, n)==-1) {isSurface(n)=1;}
        }
    }
    /////// points that already have zero tetrahedra are discarded, but no new orphans are generated
    
    //// TODO:
    // construct 2D surface data structure, with neighbors, that poionts to the 3D grid
    // get surface normals
    // compute curvature along surfaces
    // sort tetrahedra by volume
    // filter based on volume and negative curvature (need to come up with a threshold for stopping)
    

    
}


