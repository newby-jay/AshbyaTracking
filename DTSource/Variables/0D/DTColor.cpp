// Part of DTSource. Copyright 2004-2017. David Adalsteinsson.
// see http://www.visualdatatools.com/DTSource/license.html for more information.

#include "DTColor.h"

#include "DTFloatArray.h"
#include "DTError.h"

void DTColorRGBA::pinfo(void) const
{
    std::cerr << "(" << r << ", " << g << ", " << b << ", " << a << ")\n" << std::flush;
}

void Read(const DTDataStorage &input,const std::string &name,DTColorRGBA &toReturn)
{
    DTFloatArray fromFile;
    Read(input,name,fromFile);
    if (fromFile.Length()!=5) {
        DTErrorMessage("Read(File,Name,Color)","Invalid array length");
        toReturn = DTColorRGBA();
        return;
    }

    toReturn = DTColorRGBA(fromFile(1),fromFile(2),fromFile(3),fromFile(4));
}

void Write(DTDataStorage &output,const std::string &name,const DTColorRGBA &theColor)
{
    DTMutableFloatArray toSave(5);
    toSave(0) = 1.0;
    toSave(1) = theColor.r;
    toSave(2) = theColor.g;
    toSave(3) = theColor.b;
    toSave(4) = theColor.a;

    Write(output,name,toSave);
}

void DTColorCMYKA::pinfo(void) const
{
    std::cerr << "(" << c << ", " << m << ", " << y << ", " << k << ", " << a << ")\n" << std::flush;
}

void Read(const DTDataStorage &input,const std::string &name,DTColorCMYKA &toReturn)
{
    DTFloatArray fromFile;
    Read(input,name,fromFile);
    if (fromFile.Length()!=6) {
        DTErrorMessage("Read(File,Name,Color)","Invalid array length");
        toReturn = DTColorCMYKA();
        return;
    }
    
    toReturn = DTColorCMYKA(fromFile(1),fromFile(2),fromFile(3),fromFile(4),fromFile(5));
}

void Write(DTDataStorage &output,const std::string &name,const DTColorCMYKA &theColor)
{
    DTMutableFloatArray toSave(6);
    toSave(0) = 2.0;
    toSave(1) = theColor.c;
    toSave(2) = theColor.m;
    toSave(3) = theColor.y;
    toSave(4) = theColor.k;
    toSave(5) = theColor.a;
    
    Write(output,name,toSave);
}

DTColorRGBA DTColor::rgb(void) const
{
    if (type==DTColor::RGB) {
        return _rgb;
    }
    else {
        DTErrorMessage("DTColor::rgb","Not stored as an RGB color");
        return DTColorRGBA();
    }
}

DTColorCMYKA DTColor::cmyk(void) const
{
    if (type==DTColor::CMYK) {
        return _cmyk;
    }
    else {
        DTErrorMessage("DTColor::cmyk","Not stored as an CMYK color");
        return DTColorCMYKA();
    }
}

DTColor DTColor::CreateRGB(float r,float g,float b,float alpha)
{
    DTColorRGBA toReturn(r,g,b,alpha);
    return DTColor(toReturn);
}

void DTColor::pinfo(void) const
{
    switch (type) {
        case DTColor::Empty:
            std::cerr << "Empty\n";
            break;
        case DTColor::RGB:
            std::cerr << "RGB : ";
            _rgb.pinfo();
            break;
        case DTColor::CMYK:
            std::cerr << "CMYK : ";
            _cmyk.pinfo();
            break;
    }
}

bool operator==(const DTColor &A,const DTColor &B)
{
    if (A.Type()!=B.Type())
        return false;
    if (A.Type()==DTColor::Empty) {
        return false;
    }
    if (A.Type()==DTColor::RGB) {
        return (A.rgb()==B.rgb());
    }
    if (A.Type()==DTColor::CMYK) {
        return (A.cmyk()==B.cmyk());
    }
    
    return false;
}

bool operator!=(const DTColor &A,const DTColor &B)
{
    if (A.Type()!=B.Type())
        return true;
    if (A.Type()!=DTColor::Empty) {
        return true;
    }
    
    if (A.Type()==DTColor::RGB) {
        return (A.rgb()!=B.rgb());
    }
    if (A.Type()==DTColor::CMYK) {
        return (A.cmyk()!=B.cmyk());
    }
    
    return false;
}

// Reading and writing
void Read(const DTDataStorage &input,const std::string &name,DTColor &toReturn)
{
    DTFloatArray arrayForm = input.ReadFloatArray(name);
    if (arrayForm.Length()==0) {
        toReturn = DTColor();
    }
    else if (arrayForm(0)==1.0) {
        DTColorRGBA rgb;
        Read(input,name,rgb);
        toReturn = DTColor(rgb);
    }
    else if (arrayForm(0)==2.0) {
        DTColorCMYKA cmyk;
        Read(input,name,cmyk);
        toReturn = DTColor(cmyk);
    }
    else {
        toReturn = DTColor();
    }
}

void Write(DTDataStorage &output,const std::string &name,const DTColor &theColor)
{
    switch (theColor.Type()) {
        case DTColor::Empty:
            Write(output,name,DTFloatArray());
            break;
        case DTColor::RGB:
            Write(output,name,theColor.rgb());
            break;
        case DTColor::CMYK:
            Write(output,name,theColor.cmyk());
            break;
    }
}

void WriteOne(DTDataStorage &output,const std::string &name,const DTColor &toWrite)
{
    Write(output,name,toWrite);
    Write(output,"Seq_"+name,"Color");
    output.Flush();
}

