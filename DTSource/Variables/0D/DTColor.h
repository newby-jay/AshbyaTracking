// Part of DTSource. Copyright 2004-2017. David Adalsteinsson.
// see http://www.visualdatatools.com/DTSource/license.html for more information.

#ifndef DTColor_H
#define DTColor_H

#include "DTDataStorage.h"

struct DTColorRGBA
{
    DTColorRGBA() : r(0.0), g(0.0), b(0.0), a(1.0) {}
    DTColorRGBA(float rv,float gv,float bv) : r(rv), g(gv), b(bv), a(1.0) {}
    DTColorRGBA(float rv,float gv,float bv,float av) : r(rv), g(gv), b(bv), a(av) {}
    
    void pinfo(void) const;
    
    bool operator==(const DTColorRGBA &B) {return (r==B.r && g==B.g && b==B.b && a==B.a);}
    bool operator!=(const DTColorRGBA &B) {return (r!=B.r || g!=B.g || b!=B.b || a!=B.a);}
        
    float r,g,b,a;
};

extern void Read(const DTDataStorage &input,const std::string &name,DTColorRGBA &toReturn);
extern void Write(DTDataStorage &output,const std::string &name,const DTColorRGBA &theColor);

// *********

struct DTColorCMYKA
{
    DTColorCMYKA() : c(0.0), m(0.0), y(0.0), k(0.0), a(1.0) {}
    DTColorCMYKA(float cv,float mv,float yv,float kv) : c(cv), m(mv), y(yv), k(kv) {}
    DTColorCMYKA(float cv,float mv,float yv,float kv,float av) : c(cv), m(mv), y(yv), k(kv), a(av) {}
    
    void pinfo(void) const;
    
    bool operator==(const DTColorCMYKA &B) {return (c==B.c && m==B.m && y==B.y && k==B.k && k==B.k);}
    bool operator!=(const DTColorCMYKA &B) {return (c!=B.c || m!=B.m || y!=B.y || k!=B.k || k!=B.k);}
    
    float c,m,y,k,a;
};

extern void Read(const DTDataStorage &input,const std::string &name,DTColorCMYKA &toReturn);
extern void Write(DTDataStorage &output,const std::string &name,const DTColorCMYKA &theColor);

// *********

// Interchange color.
class DTColor {
public:
    DTColor() : type(DTColor::Empty) {};
    DTColor(DTColorRGBA c) : type(DTColor::RGB), _rgb(c) {}
    DTColor(DTColorCMYKA c) : type(DTColor::CMYK), _cmyk(c) {}
    
    enum ColorType {Empty,RGB,CMYK};
    
    ColorType Type(void) const {return type;}
    DTColorRGBA rgb(void) const;
    DTColorCMYKA cmyk(void) const;
    
    static DTColor CreateRGB(float r,float g,float b,float alpha=1.0f);
    
    void pinfo(void) const;
private:
    ColorType type;
    DTColorRGBA _rgb;
    DTColorCMYKA _cmyk;
};

bool operator==(const DTColor &,const DTColor &);
bool operator!=(const DTColor &,const DTColor &);

// Reading and writing
extern void Read(const DTDataStorage &input,const std::string &name,DTColor &toReturn);
extern void Write(DTDataStorage &output,const std::string &name,const DTColor &theColor);
extern void WriteOne(DTDataStorage &output,const std::string &name,const DTColor &toWrite); // One time value, self documenting.

#endif
