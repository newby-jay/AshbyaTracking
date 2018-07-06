// Include DTSource.h if you want to include all the headers.


#include "HyphaTools.h"
#include "Hypha.h"
#include "Ashbya.h"


// Common utilities
#include "DTArguments.h"
#include "DTSaveError.h"

#include "DTDataFile.h"
#include "DTDoubleArray.h"
#include "DTSurface3D.h"
#include "DTTetrahedralGrid3D.h"
#include "DTSeriesGroup.h"
#include "DTDoubleArrayOperators.h"
#include "DTProgress.h"
#include "DTTimer.h"
#include "DTUtilities.h"
#include "DTDictionary.h"


#include <math.h>
#include <list>
#include <fstream>
#include <vector>


//////////////////////////////////////////////////////////////////////////////
//    SurfaceMesh
//////////////////////////////////////////////////////////////////////////////

struct SurfaceMesh {
    DTDoubleArray points;
    DTIntArray triangles;
    
    void pinfo(void) const;
    void pinfoIndent(string) const;
};

void SurfaceMesh::pinfo(void) const
{
    pinfoIndent("");
}

void SurfaceMesh::pinfoIndent(string pad) const
{
    cerr << pad << "points = "; points.pinfo();
    cerr << pad << "triangles = "; triangles.pinfo();
}

extern void Read(DTDataStorage &,string name,SurfaceMesh &);

void Read(DTDataStorage &input,string name,SurfaceMesh &var)
{
    Read(input,name+"_points",var.points);
    Read(input,name+"_triangles",var.triangles);
}

//////////////////////////////////////////////////////////////////////////////
//    End of structure definitions.
//////////////////////////////////////////////////////////////////////////////


string Computation(const string &fileBase,DTFile hfile);

int main(int argc,const char *argv[])
{
    DTSetArguments(argc,argv);
    
    DTDataFile inputFile("Input.dtbin",DTFile::ReadOnly);
    // Read in the input variables.
    string fileBase = inputFile.ReadString("fileBase");
    DTFile hfile("hfile",DTFile::ReadOnly);
    
    // The computation.
    string computed;
    clock_t t_before = clock();
    computed = Computation(fileBase,hfile);
    clock_t t_after = clock();
    double exec_time = double(t_after-t_before)/double(CLOCKS_PER_SEC);
    
    // Write the output.
    DTDataFile outputFile("Output.dtbin",DTFile::NewReadWrite);
    
    // Output from computation
    outputFile.Save(computed,"Var");
    outputFile.Save("String","Seq_Var");
    
    // The execution time.
    outputFile.Save(exec_time,"ExecutionTime");
    outputFile.Save("Real Number","Seq_ExecutionTime");
    
    // The errors.
    DTSaveError(outputFile,"ExecutionErrors");
    outputFile.Save("StringList","Seq_ExecutionErrors");
    
    outputFile.SaveIndex();
    
    return 0;
}

string Computation(const string &basePath,DTFile hfile)
{
    DTDataFile inputFile(hfile);
    inputFile.SaveIndex();
    //DTSeriesGroup<Hypha> S2(inputFile, "Var");
    DTSeriesGroup<SurfaceMesh> S(inputFile, "Var");
    int Nsegs = S.HowManySaved();
    std::list<Hypha> hyphaList;
    

    for (int s=0; s<Nsegs;s++) {
        // Hypha h = S2.Get(s);
        SurfaceMesh mesh = S.Get(s);
        Hypha h;
        DTPointCollection3D dtpoints(mesh.points);
        h.surface = DTSurface3D(mesh.triangles, dtpoints);
        hyphaList.push_back(skeletonizeInit(h, s+1));
    }


    DTDataFile outputFile(basePath + "-hyphaeList-withSkeletons.dtbin", DTFile::NewReadWrite);
    DTSeriesGroup<Hypha> Sout(outputFile, "Var");
    for (int n=0; n<Nsegs; n++) {
        Hypha H = hyphaList.front();
        Sout.Add(H, n);
        hyphaList.pop_front();
    }
    outputFile.SaveIndex();
    string toReturn = basePath + "-hyphaeList-withSkeletons.dtbin";
    return toReturn;
}
