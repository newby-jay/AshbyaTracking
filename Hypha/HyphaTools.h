//
//  HyphaTools.h
//  meshSkel
//
//  Created by Jay Newby on 10/30/17.
//
//

#ifndef HyphaTools_h
#define HyphaTools_h
#define CGAL_EIGEN3_ENABLED


#include <CGAL/Surface_mesh.h>
//#include <CGAL/extract_mean_curvature_flow_skeleton.h>
#include <CGAL/Mean_curvature_flow_skeletonization.h>
#include <CGAL/Simple_cartesian.h>
#include <CGAL/boost/graph/graph_traits_Surface_mesh.h>
#include <CGAL/boost/graph/properties_Surface_mesh.h>
#include <CGAL/Polygon_mesh_processing/remesh.h>
#include <CGAL/Polygon_mesh_processing/border.h>
#include <CGAL/Polygon_mesh_processing/triangulate_hole.h>

#include <CGAL/Exact_predicates_inexact_constructions_kernel.h>
#include <CGAL/Delaunay_triangulation_3.h>
#include <CGAL/Triangulation_vertex_base_with_info_3.h>
#include <CGAL/Alpha_shape_3.h>

#include "DTDataFile.h"
#include "DTDoubleArray.h"
#include "DTSurface3D.h"
#include "DTTetrahedralGrid3D.h"
#include "Ashbya.h"
#include "Hypha.h"

#include <math.h>
#include <list>
#include <fstream>
#include <vector>


typedef CGAL::Exact_predicates_inexact_constructions_kernel K;
typedef CGAL::Triangulation_vertex_base_with_info_3<unsigned, K> Vbb;
typedef CGAL::Alpha_shape_vertex_base_3<K,Vbb>               Vb;
typedef CGAL::Alpha_shape_cell_base_3<K>            Fb;
typedef CGAL::Triangulation_data_structure_3<Vb, Fb> Tds;

typedef CGAL::Delaunay_triangulation_3<K, Tds, CGAL::Fast_location> Delaunay;
typedef Delaunay::Point Point;
typedef Delaunay::Cell_handle Cell_handle;
typedef Delaunay::Vertex_handle Vertex_handle;
//typedef Delaunay::Facet_handle Facet_handle;
typedef Delaunay::Facet Facet;

//typedef CGAL::Alpha_shape_vertex_base_3<K>          Vba;
//typedef CGAL::Triangulation_data_structure_3<Vba,Fb>  Tdsa;
typedef K::Point_3                            Point;
typedef CGAL::Alpha_shape_3<Delaunay>         Alpha_shape_3;
typedef Alpha_shape_3::Alpha_iterator         Alpha_iterator;
typedef Alpha_shape_3::NT                     NT;

typedef CGAL::Simple_cartesian<double>                        Kernel;
typedef Kernel::Point_3                                       Point3;
typedef CGAL::Surface_mesh<Point3>                            Triangle_mesh;
typedef boost::graph_traits<Triangle_mesh>::vertex_descriptor vertex_descriptor;
//typedef Triangle_mesh::Vertex_index                           vertex_descriptor;
typedef Triangle_mesh::Face_index                         Face_index;
typedef boost::graph_traits<Triangle_mesh>::halfedge_descriptor halfedge_descriptor;
//typedef boost::graph_traits<Triangle_mesh>::vertex_descriptor vertex_descriptor;
//typedef boost::graph_traits<Triangle_mesh>::face_descriptor face_descriptor;
typedef CGAL::Mean_curvature_flow_skeletonization<Triangle_mesh> Skeletonization;
typedef Skeletonization::Skeleton                             Skeleton;
typedef Skeleton::vertex_descriptor                           Skeleton_vertex;
typedef Skeleton::edge_descriptor                             Skeleton_edge;
namespace PMP = CGAL::Polygon_mesh_processing;




std::list<Hypha> collectHyphae(const DTDoubleArray &pointsWithSegments);
Hypha remeshHypha(const DTDoubleArray &segPoints, int seg);
Hypha remeshSurface(const Ashbya &ash, int seg);
Hypha makeSkeleton(const DTSurface3D &newSurf, const Triangle_mesh &tmesh,
                   const DTIntArray &vertexToPoint, int seg);
Hypha skeletonizeInit(const Hypha &H, int seg);
int getInd(int a, int b, int c);
HyphaGraph initHyphaGraph(const DTIntArray &G);
void alignSkel(int p0, int chan, const HyphaGraph &hg, DTMutableIntArray &G);
int alignBranch(const HyphaGraph &hg, DTMutableIntArray &G, int now, int nxt);
int travelBranch(const HyphaGraph &hg, int now, int direction);
double extractBranchArcLength(const HyphaGraph &hg, const DTPointCollection3D &skelPoints,
                              DTMutableDoubleArray &DS,
                              int now, int N, int direction);
int NbranchElements(const HyphaGraph &hg, int now, int direction);


#endif /* HyphaTools_h */
