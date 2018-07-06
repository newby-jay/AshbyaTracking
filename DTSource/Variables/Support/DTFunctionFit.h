// Part of DTSource. Copyright 2012. David Adalsteinsson.
// see http://www.visualdatatools.com/DTSource/license.html for more information.


#ifndef DTFunctionFit_h
#define DTFunctionFit_h

class DTFunction;
class DTDictionary;
class DTMutableDictionary;
class DTDoubleArray;
class DTMutableDoubleArray;

// On output, guesses contains the result of the least squares fit.
extern DTFunction FunctionFit(const DTFunction &arbitrary,const DTDoubleArray &values,const DTDictionary &knownConstants,DTMutableDictionary &guesses);

// Example:
/*
// Fitting y = a + b*exp(-c*x), where x and y are vectors and a,b are constants.
DTFunction a = DTFunction::Constant("a");
DTFunction b = DTFunction::Constant("b");
DTFunction c = DTFunction::Constant("c");
DTFunction x = DTFunction::Constant("x");
DTFunction foo = a + b*exp(-c*x);

// Create initial data
DTMutableDoubleArray xval(20), yval(20);
for (int i=0;i<20;i++) {
    xval(i) = 0.1*i;
    yval(i) = 1 - 3*exp(-2*xval(i));
}

// The x is known and a,b,c are unknown guesses.  You can have multiple known arguments, and they can be arrays or single numbers.
DTMutableDictionary knownConstants;
knownConstants("x") = xval;
DTMutableDictionary guesses;
guesses("a") = 1.5;
guesses("b") = -2;
guesses("c") = 3;

DTFunction fit = FunctionFit(foo,yval,knownConstants,guesses);
// The guesses dictionary contains the result
 
*/

// More general optimization driver, expected to fill the returnArray with the current solution.
// This allows you drive a much larger routine than just an expression.
// Use the knownConstants to hand arguments to the underlying function (i.e. a solver)
// typedef bool (*DTFitFunction)(const DTDictionary &knownConstants,const DTDictionary &optimizeConstants,DTMutableDoubleArray &returnArray);
typedef bool (*DTFitFunction)(const DTDictionary &constants,DTMutableDoubleArray &returnArray);
extern DTDictionary FunctionFit(DTFitFunction foo,const DTDoubleArray &values,const DTDictionary &knownConstants,const DTDictionary &optimizeConstants);

// Example code
/*
bool foo(const DTDictionary &constants,DTMutableDoubleArray &returnArray)
{
    DTDoubleArray x = constants("x");
    int howMany = x.Length();
    int i;
    double a = constants("a");
    double b = constants("b");
    double c = constants("c");
    
    for (i=0;i<howMany;i++) {
        returnArray(i) = a + b*exp(-c*x(i));
    }
    
    return true;
}

void test(void)
{
    DTMutableDoubleArray xval(20), yval(20);
    for (int i=0;i<20;i++) {
        xval(i) = 0.1*i;
        yval(i) = 1 - 3*exp(-2*xval(i));
    }
    DTMutableDictionary knownConstants;
    knownConstants("x") = xval;
    DTMutableDictionary guesses;
    guesses("a") = 1.5;
    guesses("b") = -2;
    guesses("c") = 3;
    
    DTDictionary returned = FunctionFit(foo,yval,knownConstants,guesses);
    returned.pinfo();
}
  */

#endif
