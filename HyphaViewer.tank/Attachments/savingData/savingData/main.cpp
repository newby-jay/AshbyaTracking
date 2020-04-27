// Include DTSource.h if you want to include all the headers.

#include "DTArguments.h"
#include "DTSaveError.h"

#include "DTDataFile.h"
#include "DTDoubleArray.h"
#include "DTPath3D.h"
#include "DTSurfaceValues3D.h"
#include "DTBitmap2D.h"
// Include DTSource.h if you want to include all the headers.

#include "DTArguments.h"
#include "DTSaveError.h"

#include "DTSurfaceValues3D.h"
#include "DTDataFile.h"
#include "DTPath3D.h"
#include "DTSeriesSurfaceValues3D.h"
#include "DTSeriesPointCollection3D.h"
#include "DTSeriesBitmap2D.h"

// Common utilities
#include "DTDoubleArrayOperators.h"
#include "DTProgress.h"
#include "DTTimer.h"
#include "DTUtilities.h"
#include "DTDictionary.h"

#include <math.h>

string Computation(const DTSeriesSurfaceValues3D &diffusivity,const DTSeriesSurfaceValues3D &viscosity,
                   const DTSeriesSurfaceValues3D &density,const DTSeriesBitmap2D &maxProj,
                   const DTPath3D &GEMS_paths,
                   int Ntvals, const string &fn,double NF,double Ny,
                   double Nx,double Nz,double zs,double dt,double dxy,const DTSeriesPointCollection3D &gems,
                   const DTSeriesPointCollection3D &nuclei);

int main(int argc,const char *argv[])
{
    DTSetArguments(argc,argv);
    
    DTDataFile inputFile("Input.dtbin",DTFile::ReadOnly);
    // Read in the input variables.
    DTDataFile diffusivityFile("diffusivity.dtbin",DTFile::ReadOnly);
    DTSeriesSurfaceValues3D diffusivity(diffusivityFile, "Var");
    //Read(inputFile,"diffusivity",diffusivity);
    DTDataFile viscosityFile("viscosity.dtbin",DTFile::ReadOnly);
    DTSeriesSurfaceValues3D viscosity(viscosityFile, "Var");
    //Read(inputFile,"viscosity",viscosity);
    DTDataFile densityFile("density.dtbin",DTFile::ReadOnly);
    DTSeriesSurfaceValues3D density(densityFile, "Var");
    //Read(inputFile,"density",density);
    DTDataFile mpFile("maxProj.dtbin",DTFile::ReadOnly);
    DTSeriesBitmap2D maxProj(mpFile, "Var");
    DTDataFile gemsFile("gems.dtbin",DTFile::ReadOnly);
    DTSeriesPointCollection3D gems(gemsFile, "Var");
    DTDataFile nucFile("nuclei.dtbin",DTFile::ReadOnly);
    DTSeriesPointCollection3D nuclei(nucFile, "Var");
    
    DTPath3D GEMS_paths;
    Read(inputFile,"GEMS_paths",GEMS_paths);
    string fn = inputFile.ReadString("fn");
    double NF = inputFile.ReadNumber("NF");
    double Ny = inputFile.ReadNumber("Ny");
    double Nx = inputFile.ReadNumber("Nx");
    double Nz = inputFile.ReadNumber("Nz");
    double zs = inputFile.ReadNumber("zs");
    double dt = inputFile.ReadNumber("dt");
    double dxy = inputFile.ReadNumber("dxy");
    int Ntvals = diffusivity.HowManySaved();
    // The computation.
    string computed;
    clock_t t_before = clock();
    computed = Computation(diffusivity,viscosity,density,maxProj,GEMS_paths,Ntvals,fn,NF,Ny,Nx,Nz,zs,dt,dxy,gems,nuclei);
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

string Computation(const DTSeriesSurfaceValues3D &diffusivity,const DTSeriesSurfaceValues3D &viscosity,
                   const DTSeriesSurfaceValues3D &density, const DTSeriesBitmap2D &maxProj,
                   const DTPath3D &GEMS_paths,
                   int Ntvals, const string &fn,double NF,double Ny,
                   double Nx,double Nz,double zs,double dt,double dxy,const DTSeriesPointCollection3D &gems,
                   const DTSeriesPointCollection3D &nuclei)
{

    DTDataFile seriesOutput(fn, DTFile::NewReadWrite);
    
    seriesOutput.Save(NF,"NF");
    seriesOutput.Save("Real Number","Seq_NF");
    Write(seriesOutput,"",DTDoubleArray());
    seriesOutput.Save(Ny,"Ny");
    seriesOutput.Save("Real Number","Seq_Ny");
    seriesOutput.Save(Nx,"Nx");
    seriesOutput.Save("Real Number","Seq_Nx");
    seriesOutput.Save(Nz,"Nz");
    seriesOutput.Save("Real Number","Seq_Nz");
    seriesOutput.Save(zs,"zs");
    seriesOutput.Save("Real Number","Seq_zs");
    seriesOutput.Save(dt,"dt");
    seriesOutput.Save("Real Number","Seq_dt");
    seriesOutput.Save(dxy,"dxy");
    seriesOutput.Save("Real Number","Seq_dxy");
    Write(seriesOutput, "GEMS_paths", GEMS_paths);
    seriesOutput.Save("Path3D", "Seq_GEMS_paths");
    //DTSeriesGroup<timeVars> Sout(seriesOutput, "Var");
    DTSeriesSurfaceValues3D diffusivityOut(seriesOutput, "diffusivity");
    DTSeriesSurfaceValues3D viscosityOut(seriesOutput, "viscosity");
    DTSeriesSurfaceValues3D densityOut(seriesOutput, "density");
    DTSeriesBitmap2D maxProjOut(seriesOutput, "maxProj");
    DTSeriesPointCollection3D gemsOut(seriesOutput, "gems");
    DTSeriesPointCollection3D nucleiOut(seriesOutput, "nuclei");
    for (int n=1;n<=Ntvals;n++) {
        //DTSeriesSurfaceValues3D
        diffusivityOut.Add(diffusivity.Get(n), n);
        viscosityOut.Add(viscosity.Get(n), n);
        densityOut.Add(density.Get(n), n);
        maxProjOut.Add(maxProj.Get(n), n);
        gemsOut.Add(gems.Get(n), n);
        nucleiOut.Add(nuclei.Get(n), n);
    }
    seriesOutput.SaveIndex();
    // GEMS_paths.Data() - DTDoubleArray - Packed representation (see header file).
    // GEMS_paths.NumberOfLoops() - how many components there are.
    // GEMS_paths.NumberOfPoints() - Number of xy values.
    // GEMS_paths.ExtractLoop(int) - Get a single component.
    return fn;

}
