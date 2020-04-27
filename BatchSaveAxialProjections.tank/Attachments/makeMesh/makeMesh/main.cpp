// Include DTSource.h if you want to include all the headers.

#include "DTArguments.h"
#include "DTSaveError.h"

#include "DTDataFile.h"
#include "DTPointCollection3D.h"
#include "DTTetrahedralGrid3D.h"

// Common utilities
#include "DTDoubleArrayOperators.h"
#include "DTProgress.h"
#include "DTTimer.h"
#include "DTUtilities.h"
#include "DTDictionary.h"

#include <math.h>
#include <CGAL/Exact_predicates_inexact_constructions_kernel.h>
#include <CGAL/Delaunay_triangulation_3.h>
#include <CGAL/Triangulation_vertex_base_with_info_3.h>
#include <list>


DTTetrahedralGrid3D Computation(const DTPointCollection3D &points);


typedef CGAL::Exact_predicates_inexact_constructions_kernel K;
typedef CGAL::Triangulation_vertex_base_with_info_3<unsigned, K> Vb;
typedef CGAL::Triangulation_data_structure_3<Vb> Tds;
typedef CGAL::Delaunay_triangulation_3<K, Tds, CGAL::Fast_location> Delaunay;
typedef Delaunay::Point Point;
typedef Delaunay::Cell_handle Cell_handle;
typedef Delaunay::Facet Facet;


int main(int argc,const char *argv[])
{
    DTSetArguments(argc,argv);
    DTDataFile inputFile("Input.dtbin",DTFile::ReadOnly);
    // Read in the input variables.
    DTPointCollection3D points;
    Read(inputFile,"points",points);
    
    // The computation.
    DTTetrahedralGrid3D computed;
    clock_t t_before = clock();
    computed = Computation(points);
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


DTTetrahedralGrid3D Computation(const DTPointCollection3D &points)
{
    size_t Npoints = points.NumberOfPoints();
    
    
    
    std::list< std::pair<Point, unsigned> > listOfPoints;
    for (int i=0;i<Npoints;i++) {
        DTPoint3D p = points(i);
        listOfPoints.push_front(std::make_pair(Point(p.x, p.y, p.z), i));
    }
    Delaunay T(listOfPoints.begin(), listOfPoints.end());
    //Triangulation T(listOfPoints.begin(), listOfPoints.end());
    size_t Ncells = T.number_of_finite_cells();
    DTMutableIntArray offsets(4, Ncells);
    
    Delaunay::Finite_cells_iterator cit = T.finite_cells_begin();
    int jc=0;
    for (cit = T.finite_cells_begin(); cit != T.finite_cells_end(); ++cit) {
        for (int jv=0;jv<4;jv++) {
            offsets(jv, jc) = cit->vertex(jv)->info();
        }
        jc++;
    }
    

    DTTetrahedralGrid3D toReturn(offsets, points);
    return toReturn;
}
