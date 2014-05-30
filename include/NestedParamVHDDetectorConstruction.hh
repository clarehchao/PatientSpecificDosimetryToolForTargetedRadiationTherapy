#ifndef NestedParamVHDDetectorConstruction_h
#define NestedParamVHDDetectorConstruction_h 1

#include "globals.hh"
#include "VHDDetectorConstruction.hh"
#include "VHDNestedPhantomParameterisation.hh"

class NestedParamVHDDetectorConstruction : public VHDDetectorConstruction
{
public:

  NestedParamVHDDetectorConstruction();
  virtual ~NestedParamVHDDetectorConstruction();

private:

  virtual void ConstructPhantom();
  VHDNestedPhantomParameterisation* param;

};

#endif

