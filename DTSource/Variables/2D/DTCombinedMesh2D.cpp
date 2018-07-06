// Part of DTSource. Copyright 2006. David Adalsteinsson.
// see http://www.visualdatatools.com/DTSource/license.html for more information.

#include "DTCombinedMesh2D.h"
#include "DTUtilities.h"

DTCombinedMesh2D::DTCombinedMesh2D(const DTList<DTMesh2D> &m)
{
    uniform = m;
}

DTCombinedMesh2D::DTCombinedMesh2D(const DTList<DTStructuredMesh2D> &m)
{
    structured = m;
}

DTCombinedMesh2D::DTCombinedMesh2D(const DTList<DTTriangularMesh2D> &m)
{
    triangular = m;
}

DTCombinedMesh2D::DTCombinedMesh2D(const DTList<DTMesh2D> &m1,const DTList<DTStructuredMesh2D> &m2,const DTList<DTTriangularMesh2D> &m3)
{
    uniform = m1;
    structured = m2;
    triangular = m3;
}

void DTCombinedMesh2D::pinfo(void) const
{
    if (IsEmpty()) {
        std::cerr << "Empty\n";
    }
    else {
        size_t i,len;
        len = int(uniform.Length());
        for (i=0;i<len;i++) {
            uniform(i).pinfo();
        }
        len = structured.Length();
        for (i=0;i<len;i++) {
            structured(i).pinfo();
        }
        len = triangular.Length();
        for (i=0;i<len;i++) {
            triangular(i).pinfo();
        }
    }
}
    
void Read(const DTDataStorage &input,const std::string &name,DTCombinedMesh2D &toReturn)
{
    int howManyMeshes = input.ReadInt(name);
    if (howManyMeshes<=0) {
        toReturn = DTCombinedMesh2D();
        return;
    }
    
    DTMutableList<DTMesh2D> uniform(howManyMeshes);
    int posUniform = 0;
    DTMutableList<DTStructuredMesh2D> structured(howManyMeshes);
    int posStructured = 0;
    DTMutableList<DTTriangularMesh2D> triangular(howManyMeshes);
    int posTriangular = 0;
    
    int i;
    std::string type;
    for (i=0;i<howManyMeshes;i++) {
        type = input.ReadString(name+"_"+DTInt2String(i)+"type");
        if (type=="Uniform") {
            Read(input,name+"_"+DTInt2String(i)+"data",uniform(posUniform));
            posUniform++;
        }
        else if (type=="Triangular") {
            Read(input,name+"_"+DTInt2String(i)+"data",triangular(posTriangular));
            posTriangular++;
        }
        else if (type=="Structured") {
            Read(input,name+"_"+DTInt2String(i)+"data",structured(posStructured));
            posStructured++;
        }
    }
    
    uniform = TruncateSize(uniform,posUniform);
    structured = TruncateSize(structured,posStructured);
    triangular = TruncateSize(triangular,posTriangular);
    
    toReturn = DTCombinedMesh2D(uniform,structured,triangular);
}

void Write(DTDataStorage &output,const std::string &name,const DTCombinedMesh2D &toWrite)
{
    int i,len;
    int howMany = 0;
    
    DTList<DTMesh2D> uniform = toWrite.Uniform();
    len = int(uniform.Length());
    for (i=0;i<len;i++) {
        output.Save("Uniform",name+"_"+DTInt2String(howMany)+"type");
        Write(output,name+"_"+DTInt2String(howMany)+"data",uniform(i));
        howMany++;
    }
    
    DTList<DTStructuredMesh2D> structured = toWrite.Structured();
    len = int(structured.Length());
    for (i=0;i<len;i++) {
        output.Save("Structured",name+"_"+DTInt2String(howMany)+"type");
        Write(output,name+"_"+DTInt2String(howMany)+"data",structured(i));
        howMany++;
    }
    
    DTList<DTTriangularMesh2D> triangular = toWrite.Triangular();
    len = int(triangular.Length());
    for (i=0;i<len;i++) {
        output.Save("Triangular",name+"_"+DTInt2String(howMany)+"type");
        Write(output,name+"_"+DTInt2String(howMany)+"data",triangular(i));
        howMany++;
    }
    
    output.Save(howMany,name);
}

void WriteOne(DTDataStorage &output,const std::string &name,const DTCombinedMesh2D &toWrite)
{
    Write(output,name,toWrite);
    Write(output,"Seq_"+name,"2D Combined Mesh");
    output.Flush();
}

