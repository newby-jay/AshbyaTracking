// Include DTSource.h if you want to include all the headers.
#define CGAL_EIGEN3_ENABLED


#include <CGAL/Surface_mesh.h>
//#include <CGAL/extract_mean_curvature_flow_skeleton.h>
#include <CGAL/Mean_curvature_flow_skeletonization.h>
#include <CGAL/Simple_cartesian.h>
#include <CGAL/boost/graph/graph_traits_Surface_mesh.h>
#include <CGAL/boost/graph/properties_Surface_mesh.h>
#include <CGAL/Polygon_mesh_processing/remesh.h>
//#include <CGAL/Polygon_mesh_processing/border.h>
//#include <CGAL/Polygon_mesh_processing/triangulate_hole.h>

#include <CGAL/Exact_predicates_inexact_constructions_kernel.h>
#include <CGAL/Delaunay_triangulation_3.h>
#include <CGAL/Triangulation_vertex_base_with_info_3.h>
#include <CGAL/Alpha_shape_3.h>

#include "DTArguments.h"
#include "DTSaveError.h"

#include "DTDataFile.h"
#include "DTTetrahedralGrid3D.h"

// Common utilities
#include "DTDoubleArrayOperators.h"
#include "DTProgress.h"
#include "DTTimer.h"
#include "DTUtilities.h"
#include "DTDictionary.h"

#include <math.h>
#include <list>
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

DTTetrahedralGrid3D Computation(const DTTetrahedralGrid3D &Tgrid);

int main(int argc,const char *argv[])
{
    DTSetArguments(argc,argv);

    DTDataFile inputFile("Input.dtbin",DTFile::ReadOnly);
    // Read in the input variables.
    DTTetrahedralGrid3D Tgrid;
    Read(inputFile,"Tgrid",Tgrid);

    // The computation.
    DTTetrahedralGrid3D computed;
    clock_t t_before = clock();
    computed = Computation(Tgrid);
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

DTTetrahedralGrid3D Computation(const DTTetrahedralGrid3D &Tgrid)
{
    // Tgrid.Points() - DTFloatArray - 3xN underlying points
    // Tgrid.Offsets() - DTIntArray - 4xM array
    // Tgrid.NeighboringTetrahedra() - DTIntArray 4xM array/empty if not set
    // Tgrid.PointNumbers() - DTIntArray - number of points before cropping.
    DTPointCollection3D points = Tgrid.Points();
    size_t Npoints = points.NumberOfPoints();
    std::list< std::pair<Point, unsigned> > listOfPoints;
    for (int n=0;n<Npoints;n++) {
        DTPoint3D point = points(n);
        listOfPoints.push_front(std::make_pair(Point(point.x,
                                                     point.y,
                                                     point.z), n));
    }
    Delaunay T(listOfPoints.begin(), listOfPoints.end());
    
    Alpha_shape_3 as(T);
    // find optimal alpha values
//    Alpha_shape_3::NT alpha_solid = as.find_alpha_solid();
//    Alpha_iterator opt = as.find_optimal_alpha(1);
//    int alphaOpt = *opt;
//    int alpha = (alphaOpt > 300) ? alphaOpt + 50 : 300;
    as.set_alpha(500);
    std::list<Cell_handle> cells;
    as.get_alpha_shape_cells(std::back_inserter(cells),
                             Alpha_shape_3::INTERIOR);
    size_t Ncells = cells.size();
    DTMutableIntArray offsets(4, Ncells);
    //    Delaunay::Finite_cells_iterator cit = T.finite_cells_begin();
    int n=0;
    for (Cell_handle cit : cells) {
        for (int m=0; m<4; m++) {
            offsets(m, n) = cit->vertex(m)->info();
        }
        n++;
    }
    DTTetrahedralGrid3D toReturn(offsets, points);
    return toReturn;
}
