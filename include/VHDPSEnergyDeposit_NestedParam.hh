#ifndef VHDPSEnergyDeposit_NestedParam_h
#define VHDPSEnergyDeposit_NestedParam_h 1

#include "G4PSEnergyDeposit.hh"

//G4PSEnergyDeposit is the derived class of G4VPrimitiveScorer
class VHDPSEnergyDeposit_NestedParam : public G4PSEnergyDeposit
{
   public: // with description
      VHDPSEnergyDeposit_NestedParam(G4String name,G4int nx,G4int ny, G4int nz);
      virtual ~VHDPSEnergyDeposit_NestedParam();

  protected: // with description
      G4int GetIndex(G4Step*);

  private:
      G4int fNx, fNy, fNz, fNxNy;
};
#endif

