//
//  Hypha.h
//  meshSkel
//
//  Created by Jay Newby on 10/26/17.
//
//

#ifndef Hypha_h
#define Hypha_h


#include "DTDataFile.h"
#include "DTDoubleArray.h"
#include "DTSurface3D.h"
#include "DTTetrahedralGrid3D.h"

#include <math.h>
#include <list>
#include <fstream>
#include <vector>

struct HyphaGraph {
    int Ncaps; // total number of end caps
    int Nbranches; // total number of branch points
    int NskelPoints; // total number of skeleton points
    DTIntArray skelGraph;// edges of full skeleton graph, shape = (Number of skeleton points, 3)
                         // 0: downstream skeleton point index,
                         // 1: upstream skeleton point index,
                         // 3: branching skeleton point index (for branch points only)
    DTIntArray z; // number of edges for each skeleton vertex (1: cap, 2: interior point, 3: branch point)
    DTIntArray caps; // vertex index for tips/caps (z=1)
    DTIntArray branches; // vertex index for branch points (z=3)
    DTIntArray branchGraph; // edges connecting caps and branches (top level graph)
    
    static void WriteStructure(DTDataStorage &,string);
};
extern void Read(DTDataStorage &,string name,HyphaGraph &);
extern void Write(DTDataStorage &,string name,const HyphaGraph &);

struct Tracks {
    DTPointCollection3D points; // point index -> x, y, z
    DTIntArray pointToParticle; // point index -> particle/track index
    DTIntArray particleToPoints; // particle/track index -> track point start offset, number of track points
    DTDoubleArray radius; // point index -> radius
    DTIntArray time; // point index -> time
    DTIntArray skelAndSurfInd; // point index -> closest skeleton point index, closest surface point index
    DTDoubleArray minDistToSurf; // point index -> minimum distance to surface
    static void WriteStructure(DTDataStorage &,string);
};
extern void Read(DTDataStorage &,string name,Tracks &);
extern void Write(DTDataStorage &,string name,const Tracks &);

struct Hypha {
    DTSurface3D surface; // triangular surface mesh
    DTIntArray surfToSkeleton; // surface point index -> skeleton point index
    DTIntArray skelToSurf; // offset + n -> surface point index
    DTIntArray skelToSurfIndex; // skeleton point index -> (offset, size) for surface mapping
                                // shape = (number of skeleton points, 2)
    DTPointCollection3D skeleton; // skeleton points: index -> (x,y,z) position
    Tracks GEMS;
    Tracks nuclei;
    HyphaGraph hyphaGraph;
    DTDoubleArray DS; // arclength for each skeleton edge
    DTDoubleArray branchLengths; // arc lengths of cap to cap or cap to branch segments
    double maxRadius; // maximum skeleton point to connected surface point disntance (upper bound on hypha cross sectional radius)
    static void WriteStructure(DTDataStorage &,string);
};
extern void Read(DTDataStorage &,string name,Hypha &);
extern void Write(DTDataStorage &,string name,const Hypha &);
////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////
std::list<Hypha> addTracks(const std::list<Hypha> &H, const DTDoubleArray &paths, int GEMSorNuclei);
DTIntArray segmentPoints(const std::list<Hypha> &H, const DTDoubleArray &points);
std::pair<DTIntArray, DTDoubleArray> GEMSdistanceFromNuclei(const Hypha &h, double maxDist);
DTDoubleArray incrementValueSet(const Tracks &tracks, const DTIntArray &pointNumbers, const DTDoubleArray &values);
std::list<int> localSkelPoints(const Hypha &h, int skelPoint, int range);
std::list<int> localSurfacePoints(const Hypha &h, int skelPoint, int range);
DTDoubleArray diffusivitySurfaceMap(const Hypha &h, int window,int seg,int NF, double sigma, int range);
DTDoubleArray densitySurfaceMap(const Hypha &h, int window,int seg,int NF, double sigma, int range);
struct MedialAxisValues {
    DTDoubleArray radius;
    DTDoubleArray density;
    DTDoubleArray diffusivity;
};
MedialAxisValues densityDiffusivityProjectedOnMedialAxis(const Hypha &h, const DTIntArray &skelToOrdered, int window, int NF, double sigma, int range);
std::pair<std::list<DTDoubleArray>, DTIntArray> getOrderedMedialAxis(const Hypha &h);
#endif /* Hypha_h */
