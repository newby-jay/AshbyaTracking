// Part of DTSource. Copyright 2004-2017. David Adalsteinsson.
// see http://www.visualdatatools.com/DTSource/license.html for more information.

#include "DTColorList.h"

#include "DTDataStorage.h"
#include "DTError.h"
#include "DTUCharArray.h"
#include <math.h>

DTColorList::DTColorList(const DTFloatArray &rgb)
{
    if (rgb.m()==0)
        return;
    
    if ((rgb.m()!=3 && rgb.m()!=4) || rgb.o()!=1) {
        DTErrorMessage("DTColorList(array)","Array can only have 3 or 4 rows");
        return;
    }
    
    rgbArray = rgb;
}

DTMutableUCharArray DTColorList::EightBitColors(void) const
{
    DTMutableUCharArray toReturn(rgbArray.m(),rgbArray.n());
    ssize_t i,mn = rgbArray.m()*rgbArray.n();
    float v;
    for (i=0; i<mn; i++) {
        v = rgbArray(i);
        toReturn(i) = round(v*255);
    }
    
    return toReturn;
}

void DTColorList::pinfo(void) const
{
    if (rgbArray.IsEmpty())
        std::cerr << "No colors\n";
    else if (rgbArray.n()==1)
        std::cerr << "1 color\n";
    else 
        std::cerr << rgbArray.n() << " colors\n";
    std::cerr << flush;
}

void Read(const DTDataStorage &input,const std::string &name,DTColorList &toReturn)
{
    DTFloatArray rgb;
    Read(input,name,rgb);

    toReturn = DTColorList(rgb);
}

void Write(DTDataStorage &output,const std::string &name,const DTColorList &theVar)
{
    Write(output,name,theVar.RGB());
}

void WriteOne(DTDataStorage &output,const std::string &name,const DTColorList &toWrite)
{
    Write(output,name,toWrite);
    Write(output,"Seq_"+name,"List of Colors");
    output.Flush();
}

