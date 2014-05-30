 //
// ********************************************************************
// * License and Disclaimer                                           *
// *                                                                  *
// * The  Geant4 software  is  copyright of the Copyright Holders  of *
// * the Geant4 Collaboration.  It is provided  under  the terms  and *
// * conditions of the Geant4 Software License,  included in the file *
// * LICENSE and available at  http://cern.ch/geant4/license .  These *
// * include a list of copyright holders.                             *
// *                                                                  *
// * Neither the authors of this software system, nor their employing *
// * institutes,nor the agencies providing financial support for this *
// * work  make  any representation or  warranty, express or implied, *
// * regarding  this  software system or assume any liability for its *
// * use.  Please see the license in the file  LICENSE  and URL above *
// * for the full disclaimer and the limitation of liability.         *
// *                                                                  *
// * This  code  implementation is the result of  the  scientific and *
// * technical work of the GEANT4 collaboration.                      *
// * By using,  copying,  modifying or  distributing the software (or *
// * any work based  on the software)  you  agree  to acknowledge its *
// * use  in  resulting  scientific  publications,  and indicate your *
// * acceptance of all terms of the Geant4 Software license.          *
// ********************************************************************

/**
 * @file   VHDMSDRunActionROOT.cc
 * @brief  Run action definition with ROOT usage
 *
 * @date   7th Aug 2013
 * @author Shih-ying Huang
 * @name   Geant4.9.6-p02
 */


#include "VHDMultiSDRunActionROOT.hh"
#include "VHDMultiSDRun.hh"
#include "G4RunManager.hh"
#include "VHDDetectorConstruction.hh"
#include "G4THitsMap.hh"
#include "G4UnitsTable.hh"


//=======================================================================
// VHDMultiSDRunActionROOT
//  a derived class of VHDMultiSDRunAction
//
//
//=======================================================================

// Constructor
VHDMultiSDRunActionROOT::VHDMultiSDRunActionROOT()
{;
}

// Destructor.
VHDMultiSDRunActionROOT::~VHDMultiSDRunActionROOT()
{
  G4cout << "Destroying VHDMultiSDRunActionROOT! " << G4endl;
}


void VHDMultiSDRunActionROOT::EndOfRunAction(const G4Run* aRun)
{

  //print out the total number of events during this run
  G4cout << "Number of Events in this run: " << aRun->GetNumberOfEvent() << G4endl;

  //- VHDMultiSDRun object.
  VHDMultiSDRun* MSDRun = (VHDMultiSDRun*)aRun;

  //--- Dump all socred quantities involved in VHDMultiSDRun to check for ouptput!
  //MSDRun->DumpAllScorer();
  //---


  const VHDDetectorConstruction* detector =(const VHDDetectorConstruction*)(G4RunManager::GetRunManager()->GetUserDetectorConstruction());
  fNx = detector->GetNX();
  fNy = detector->GetNY();
  fNz = detector->GetNZ();
  fNxNy = fNx*fNy;
  NEbin = detector->GetNEngbin();
  G4cout << "fNx, fNy, fNz = " << fNx << ", " << fNy << ", " << fNz << G4endl;
  G4cout << "NEbin = " << NEbin << G4endl;

  //---------------------------------------------
  // Dump accumulated quantities for this RUN.
  //  (Display only central region of x-y plane)
  //---------------------------------------------
  G4THitsMap<G4double>* totEdep = MSDRun->GetHitsMap("PhantomSD/totalEDep");
 
  std::vector<G4THitsMap<G4double>*> pCellFlux;
  char snamechar[50];
  for(G4int i = 0; i < NEbin; i++)
  {
	std::sprintf(snamechar,"PhantomSD/PhotonCellFlux%02d",i);
      	G4String sname(snamechar);
	G4THitsMap<G4double>* tmp = MSDRun->GetHitsMap(sname);
	if(tmp != NULL){
		pCellFlux.push_back(tmp);
	}
  }
  G4cout << "after reading the PhotonCellFlux G4THitsMap..." << G4endl;

  G4int ix,iy,iz,m;
  char fname1[700],fname2[700];
  
  //Save sparse data in .root files
  //write out Energy deposit sparse map for this run
  int posX,posY,posZ;
  float edep;
  TTree* EdepTree = new TTree("EdepTree","posX, posY, posZ, and Edep");
  EdepTree->Branch("posX",&posX,"posX/I");
  EdepTree->Branch("posY",&posY,"posY/I");
  EdepTree->Branch("posZ",&posZ,"posZ/I");
  EdepTree->Branch("eng",&edep,"eng/F");
  
  
  //write out fluence vs. energy sparse format for this run
  std::vector<float> fluence(28);   //initialize the vector to 28 ZEROs!
  char treename[200];
  char bname1[100];
  char bname2[100];
  std::vector<TTree*> TreeHolder;
  TTree* letree;
  
  //set the appropriate variables to read and save in the root Tree
  for(m=0; m<NEbin; m++){
  	std::sprintf(treename,"fluenceTree%02d",static_cast<int>(m+1));
  	letree = new TTree(treename,"posX, posY, posZ, and fluence per energy bin");
  	letree->Branch("posX",&posX,"posX/I");
  	letree->Branch("posY",&posY,"posY/I");
  	letree->Branch("posZ",&posZ,"posZ/I");
  	std::sprintf(bname1,"fluence%02d",static_cast<int>(m+1));
  	std::sprintf(bname2,"fluence%02d/F",static_cast<int>(m+1));
  	letree->Branch(bname1,&fluence[m],bname2);
	TreeHolder.push_back(letree);
  }
  
  for(iz = 0; iz < fNz; iz++){
	for(iy = 0; iy < fNy; iy++){
		for(ix = 0; ix < fNx; ix++){
			//set up the posX, posY, posZ
			posX = static_cast<int>(ix);
			posY = static_cast<int>(iy);
			posZ = static_cast<int>(iz);
			
			G4double* eh1 = (*totEdep)[CopyNo(ix,iy,iz)];
			if (eh1){  //write out (x,y,z) and Edep for voxels with energy deposit..
				
				edep = static_cast< float >(*eh1);
				EdepTree->Fill();

			}
			
			for(m=0; m< NEbin; m++){
				G4double* eh2 = (*pCellFlux[m])[CopyNo(ix,iy,iz)];
				if(eh2){
					fluence[m] = static_cast< float >(*eh2);   //unit of cm-2
					TreeHolder[m]->Fill();
				}
			}	
		}
	}
  }
  
  
  //save the .root file for Edep sparse data map
  sprintf(fname1,"%s/RootData/Edep.root",dirName);
  printf("write root file: %s\n",fname1);
  TFile* outfile1 = TFile::Open(fname1,"recreate");
  EdepTree->Write();	
  outfile1->Close();
  delete EdepTree;
  EdepTree = 0;
  
  //save the .root file for Edep sparse data map
  for(m=0 ; m < NEbin; m++){
	std::sprintf(fname2,"%s/RootData/Fluence%02d.root",dirName,static_cast<int>(m+1));
	printf("write root file: %s\n",fname2);
	TFile* outfile2 = TFile::Open(fname2,"recreate");
	TreeHolder[m]->Write();	
	outfile2->Close();
  }
  
  while(!TreeHolder.empty()){
	delete TreeHolder.back(), TreeHolder.pop_back();
  }
  TreeHolder.clear();
}

