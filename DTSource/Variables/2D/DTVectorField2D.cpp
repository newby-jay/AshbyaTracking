// Part of DTSource. Copyright 2004-2017. David Adalsteinsson.
// see http://www.visualdatatools.com/DTSource/license.html for more information.

#include "DTVectorField2D.h"

#include "DTError.h"
#include "DTDoubleArrayOperators.h"

DTVectorField2D::DTVectorField2D(const DTMesh2DGrid gr,const DTDoubleArray &Xin,const DTDoubleArray &Yin)
{
    // Check that the sizes are compatible.
    if (Xin.m()!=Yin.m() || Xin.n()!=Yin.n() || Xin.o()!=Yin.o() || Xin.o()>1) {
        DTErrorMessage("DTVectorField2D(Grid,X,Y)","Incompatible sizes.");
        return;
    }
    if ((gr.m() && Xin.m()!=gr.m()) || (gr.n()!=0 && Xin.n()!=gr.n())) {
        DTErrorMessage("DTVectorField2D(Grid,X,Y)","Incompatible sizes.");
        return;
    }

	grid = gr;
    if (gr.m()!=Xin.m() || gr.n()!=Xin.n()) {
        grid = ChangeSize(gr,Xin.m(),Xin.n());
    }
	
    grid = ChangeSize(gr,Xin.m(),Xin.n());
    xComponent = Xin;
    yComponent = Yin;
}

void DTVectorField2D::pinfo(void) const
{
    grid.pinfo();
}

DTVectorField2D operator*(const DTVectorField2D &field,double c)
{
    return DTVectorField2D(field.Grid(),field.X()*c,field.Y()*c);
}

double MaximumLength(const DTVectorField2D &field)
{
    DTDoubleArray x = field.X();
    DTDoubleArray y = field.Y();
    double toReturn = 0, len;
    ssize_t ij,until = x.Length();
    const double *xD = x.Pointer();
    const double *yD = y.Pointer();
    for (ij=0;ij<until;ij++) {
        len = xD[ij]*xD[ij] + yD[ij]*yD[ij];
        if (len>toReturn) toReturn = len;
    }
    return sqrt(toReturn);
}

DTVectorField2D ApplyMask(const DTVectorField2D &field,const DTMask &mask)
{
	if (field.m()==mask.m() && field.n()==mask.n()) {
		return DTVectorField2D(ApplyMask(field.Grid(),mask),field.X(),field.Y());
	}
	else {
		DTErrorMessage("ApplyMask(DTVectorField2D,DTMask)","Incompatible mask size.");
		return field;
	}
}

DTVectorField2D RemoveMask(const DTVectorField2D &field)
{
    return DTVectorField2D(RemoveMask(field.Grid()),field.X(),field.Y());
}

void Read(const DTDataStorage &input,const std::string &name,DTVectorField2D &toReturn)
{
    DTMesh2DGrid grid;
    Read(input,name,grid);

    DTDoubleArray X,Y;
    Read(input,name+"_X",X);
    Read(input,name+"_Y",Y);

    toReturn = DTVectorField2D(grid,X,Y);
}

void Write(DTDataStorage &output,const std::string &name,const DTVectorField2D &saveThis)
{
    Write(output,name+"_X",saveThis.X());
    Write(output,name+"_Y",saveThis.Y());
    WriteNoSize(output,name,saveThis.Grid());
}

void WriteOne(DTDataStorage &output,const std::string &name,const DTVectorField2D &toWrite)
{
    Write(output,name,toWrite);
    Write(output,"Seq_"+name,"2D Vector Field");
    output.Flush();
}

