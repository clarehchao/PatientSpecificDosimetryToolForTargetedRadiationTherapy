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
//
/**
 * @file   VHDMSDSteppingAction.cc
 * @brief  stepping action definition
 *
 * @date   7th Aug 2013
 * @author Shih-ying Huang
 * @name   Geant4.9.6-p02
 */


#include "G4ios.hh"
#include "VHDMSDSteppingAction.hh"
#include "G4Track.hh"
#include "globals.hh"
#include "G4SteppingManager.hh"
#include "G4Gamma.hh"
#include "G4Electron.hh"
#include <stdlib.h>
#include <stdio.h>
#include "VHDMultiSDEventAction.hh"
#include "G4RunManager.hh"
#include "G4ProcessType.hh"


VHDMSDSteppingAction::VHDMSDSteppingAction(char dname[])
{
   //set the data directory
   strcpy(datadir,dname);
   
   //ROOT histogram
   G4cout << "In VHDMSDSteppingAction constructor... initializing root histograms!!" << G4endl;
   
   hE_electron = new TH1F("KineticEnergy_electron","The spectrum of radionuclide decay products in electrons",1000,0.0,1.0);  //1000 bins, 1 keV per bin
   hE_electron->GetXaxis()->SetTitle("Particle Kinetic Energy (MeV) ");
   
   hE_photon = new TH1F("KineticEnergy_photon","The spectrum of radionuclide decay products in photons",1000,0.0,1.0);  //1000 bins, 1 keV per bin
   hE_photon->GetXaxis()->SetTitle("Particle Kinetic Energy (MeV) ");

}

VHDMSDSteppingAction::~VHDMSDSteppingAction()
{
   // Save and write the ROOT files of electron and photon energy histogram
   char filename1[500];
   sprintf(filename1,"%s/RootData/hE_electron.root",datadir);
   TFile* outfile1 = TFile::Open(filename1,"recreate");
   hE_electron->Write();	
   delete hE_electron;
   hE_electron = 0;
   outfile1->Close();

   char filename2[500];
   sprintf(filename2,"%s/RootData/hE_photon.root",datadir);
   TFile* outfile2 = TFile::Open(filename2,"recreate");
   hE_photon->Write();	
   delete hE_photon;
   hE_photon = 0;
   outfile2->Close();

   G4cout << "destroying VHDMSDSteppingAction ..." << G4endl;
   MaterialOfInterest.clear();
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo....

void VHDMSDSteppingAction::UserSteppingAction(const G4Step* aStep) 
{
  //this emitted decay product spectrum is inspired by the geant4.9.6/examples/extended/radioactivedecay/rdecay02
  G4Track* aTrack = aStep->GetTrack();
  G4int StepID = aTrack->GetCurrentStepNumber();
  G4int TrackID = aTrack->GetTrackID();
  //G4cout << "in stepping action..." << G4endl;

  G4double energy,weight;
  if(StepID == 1 && TrackID != 1)
  {
  	//CAUTION! can only access creator process for non-primary particles or else the app crashes
  	G4String ProcName = aTrack->GetCreatorProcess()->GetProcessName();
  	G4ParticleDefinition* partDef = aTrack->GetDefinition();
  	G4String partType = partDef->GetParticleType();
  	if(ProcName == "RadioactiveDecay" && partType != "nucleus")
  	{
		energy = aStep->GetPreStepPoint()->GetKineticEnergy();
		weight = aStep->GetPreStepPoint()->GetWeight();
		if(partDef == G4Electron::ElectronDefinition())
		{
			hE_electron->Fill(energy,weight);  //inrement the histogram by weight
		}
		if(partDef == G4Gamma::GammaDefinition())
		{	
			hE_photon->Fill(energy,weight);  //inrement the histogram by weight
		}
	}
  }
}

void VHDMSDSteppingAction::SetMaterialOfInterest(G4String dirname)
 {
 	G4String lename;
 	G4int Norgan;
 	
	G4String fname = dirname + "/CellFluxMaterialOfInterest.txt";
	std::ifstream fin(fname);
	if(fin.good() != 1 ){
		G4Exception("VHDMSDSteppingAction::SetMaterialOfInterest(G4String dirname)","",FatalErrorInArgument,G4String("Invalid file name: " + fname).c_str());
	}
 	fin >> Norgan;
 	for(G4int i=0; i < Norgan; i++)
 	{
 		fin >> lename;
 		MaterialOfInterest.push_back(lename);
 	}
 	fin.close();
 }
//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo....


