// Part of DTSource. Copyright 2006. David Adalsteinsson.
// see http://www.visualdatatools.com/DTSource/license.html for more information.

#ifndef DTCombinedMesh2D_H
#define DTCombinedMesh2D_H

#include "DTList.h"
#include "DTTriangularMesh2D.h"
#include "DTStructuredMesh2D.h"
#include "DTMesh2D.h"

class DTCombinedMesh2D {
public:
    DTCombinedMesh2D() {}
    DTCombinedMesh2D(const DTList<DTMesh2D> &);
    DTCombinedMesh2D(const DTList<DTStructuredMesh2D> &);
    DTCombinedMesh2D(const DTList<DTTriangularMesh2D> &);
    DTCombinedMesh2D(const DTList<DTMesh2D> &,const DTList<DTStructuredMesh2D> &,const DTList<DTTriangularMesh2D> &);
    
    bool IsEmpty(void) const {return (uniform.IsEmpty() && structured.IsEmpty() && triangular.IsEmpty());}
    DTList<DTMesh2D> Uniform(void) const {return uniform;}
    DTList<DTStructuredMesh2D> Structured(void) const {return structured;}
    DTList<DTTriangularMesh2D> Triangular(void) const {return triangular;}
    
    void pinfo(void) const;

private:
    DTList<DTMesh2D> uniform;
    DTList<DTStructuredMesh2D> structured;
    DTList<DTTriangularMesh2D> triangular;
};

// Reading and writing
extern void Read(const DTDataStorage &input,const std::string &name,DTCombinedMesh2D &toReturn);
extern void Write(DTDataStorage &output,const std::string &name,const DTCombinedMesh2D &thePath);
extern void WriteOne(DTDataStorage &output,const std::string &name,const DTCombinedMesh2D &toWrite); // One time value, self documenting.

#endif
