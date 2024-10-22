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
 * @file   VHDPrimaryGeneratorAction.cc
 * @brief  primary generator action defition
 *
 * @date   7th Aug 2013
 * @author Shih-ying Huang
 * @name   Geant4.9.6-p02
 */



#include "VHDPrimaryGeneratorAction.hh"
#include "G4ParticleTable.hh"
#include "G4ParticleDefinition.hh"
#include "Randomize.hh"
#include "globals.hh"
//#include "G4GeneralParticleSource.hh"
//#include "G4SingleParticleSource.hh"

#include "G4UnitsTable.hh"
#include <stdlib.h>
#include <stdio.h>
#include "CLHEP/Random/RandFlat.h"

using namespace std;


VHDPrimaryGeneratorAction::VHDPrimaryGeneratorAction(const G4String& dname, const G4int isSparse)
{
   //pgun = new G4GeneralParticleSource();
   pgun = new G4ParticleGun(); 
   MinTheta = 0.;
   MaxTheta = pi;
   MinPhi = 0.;
   MaxPhi = twopi;

   //set source map directory: read Sparse sourcemap format or volume sourcemap format
   if(isSparse==1)
	SetSourceProbMapSparse(dname);
   else
   	SetSourceProbMap(dname);
   G4cout << "in VHDPrimaryGeneratorAction: dname = " << dname << G4endl;   
}


VHDPrimaryGeneratorAction::~VHDPrimaryGeneratorAction()
{
  delete pgun;
  probAccum.clear();
  theProbAccum.clear();
  
  G4cout << "destroying VHDPrimaryGeneratorAction" << G4endl;
}

//
void VHDPrimaryGeneratorAction::GeneratePrimaries(G4Event* anEvent)
{ 
  
  position = GeneratePosition();
  momentum = GenerateIsotropicMomentum();

  pgun->SetParticlePosition(position);
  pgun->SetParticleMomentumDirection(momentum);
  pgun->GeneratePrimaryVertex(anEvent);
}

void VHDPrimaryGeneratorAction::SetSourceProbMap(const G4String& dirname)
{
  // open Data.dat to get all the files to read 
  G4String datafname = dirname + "/Data.dat";
  std::ifstream fin(datafname);
  if(!fin.is_open()) ErrorFileNotFound(datafname);
  G4cout << "reading from " << datafname << G4endl;
  if(fin.eof()){
	ErrorFileTooShort(datafname);
  }

  G4String fname1,fname2;
  fin >> fNoFiles;
  
  nVoxels = 0;
  NVoxelZ = 0;
  theProbSum = 0;
  for(nfile = 0; nfile < fNoFiles; nfile++)
  {
    fin >> fname1;
    //--- Read one data file
    fname2 = dirname + "/" + fname1;
    ReadDoseMapFile(fname2);
  }
  fin.close();

  //-- Normalize probabilities
  std::map<G4double,G4int>::iterator ite;
  for(ite = probAccum.begin(); ite != probAccum.end(); ite++)
  {
    theProbAccum[ (*ite).first / theProbSum ] = (*ite).second;
  }

  G4cout << "NVoxelX = " << NVoxelX << ", NVoxelY = " << NVoxelY << ", NVoxelZ = " << NVoxelZ << G4endl;
  G4cout << "dX = " << dX << ", dY = " << dY << ", dZ = " << dZ << G4endl;
  G4cout << "offsetX = " << offsetX << ", offsetY = " << offsetY << ", offsetZ = " << offsetZ << G4endl;
  G4cout << "theProbSum = " << theProbSum << G4endl;
}


void VHDPrimaryGeneratorAction::ReadDoseMapFile(const G4String& fname)
{
  std::ifstream fin(fname.c_str(), std::ios_base::in);  //ios_base::in ==> open file for reading
  if(!fin.is_open()) ErrorFileNotFound(fname);

  G4int nz;
  fin >> NVoxelX >> NVoxelY >> nz;
  fin >> Xmin >> Xmax;
  fin >> Ymin >> Ymax;
  fin >> Zmin >> Zmax;

  if(nfile == 0){
	NVoxelXY = NVoxelX * NVoxelY;
	dX = (Xmax - Xmin)/NVoxelX;
	dY = (Ymax - Ymin)/NVoxelY;
	dZ = (Zmax - Zmin)/nz;
	offsetX = Xmin + dX/2.;
	offsetY = Ymin + dY/2.;
	offsetZ = Zmin + dZ/2.;
  }
  NVoxelZ += nz;

  G4double prob;
  for( G4int ii = 0; ii < NVoxelXY; ii++, nVoxels++ )
  {
    fin >> prob;
    if( fin.eof() && ii != NVoxelXY-1)
    {
	ErrorFileTooShort(fname);
    }
    if(prob > 0.0)
    {
	theProbSum += prob;
    	probAccum[theProbSum] = nVoxels;
    }
  }
  fin.close();
}

void VHDPrimaryGeneratorAction::SetSourceProbMapSparse(const G4String& dirname)
{
  G4String fname = dirname + "/SparseDoseMap.g4d";
  std::ifstream fin(fname.c_str(), std::ios_base::in);  //ios_base::in ==> open file for reading
  if(!fin.is_open()) ErrorFileNotFound(fname);

  G4int nz;
  fin >> NVoxelX >> NVoxelY >> NVoxelZ >> nz;
  fin >> Xmin >> Xmax;
  fin >> Ymin >> Ymax;
  fin >> Zmin >> Zmax;

  NVoxelXY = NVoxelX * NVoxelY;
  dX = (Xmax - Xmin)/NVoxelX;
  dY = (Ymax - Ymin)/NVoxelY;
  dZ = (Zmax - Zmin)/nz;
  offsetX = Xmin + dX/2.;
  offsetY = Ymin + dY/2.;
  offsetZ = Zmin + dZ/2.;

  G4double cumprob;
  G4int ivox;
  while(!fin.eof())
  {
    fin >> ivox >> cumprob;
    theProbAccum[cumprob] = ivox;
  }
  fin.close();
  
  G4cout << "NVoxelX = " << NVoxelX << ", NVoxelY = " << NVoxelY << ", NVoxelZ = " << NVoxelZ << G4endl;
  G4cout << "dX = " << dX << ", dY = " << dY << ", dZ = " << dZ << G4endl;
  G4cout << "offsetX = " << offsetX << ", offsetY = " << offsetY << ", offsetZ = " << offsetZ << G4endl;
  G4cout << "theProbSum = " << theProbSum << G4endl;
}

G4ThreeVector VHDPrimaryGeneratorAction::GeneratePosition()
{
  
  // Sample the position observing from a given probability distributiion using Rejection Sampling method
  G4double rnd = CLHEP::RandFlat::shoot();
  std::map<G4double,G4int>::iterator ite = theProbAccum.upper_bound(rnd);  //upper_bound: return the iterator pointing to the first element that is GREATER than rnd

  G4int nVox = (*ite).second;
  G4int nx = nVox%NVoxelX;
  G4int ny = (nVox/NVoxelX)%NVoxelY;
  G4int nz = nVox/NVoxelXY;

  G4ThreeVector pos( offsetX + dX*nx,
		     offsetY + dY*ny,
		     offsetZ + dZ*nz);

  return pos;
}

G4ThreeVector VHDPrimaryGeneratorAction::GenerateIsotropicMomentum()
{
	G4double rnd1,rnd2,px,py,pz,phi;
	G4double sintheta,sinphi,costheta,cosphi;

	rnd1 = G4UniformRand();
	costheta = std::cos(MinTheta) - rnd1*(std::cos(MinTheta) - std::cos(MaxTheta));
	sintheta = std::sqrt(1. - costheta*costheta);
	
	rnd2 = G4UniformRand();
	phi = MinPhi + (MaxPhi - MinPhi)*rnd2;
	sinphi = std::sin(phi);
	cosphi = std::cos(phi);

	px = -sintheta*cosphi;
	py = -sintheta*sinphi;
	pz = -costheta;
	
	G4double mag = std::sqrt(px*px + py*py + pz*pz);

	G4ThreeVector mom(px/mag,py/mag,pz/mag);

	return mom;
}

 

void VHDPrimaryGeneratorAction::ErrorFileTooShort(const G4String& fname)
{
	G4Exception(G4String("VHDPrimaryGeneratorAction::SetSourceProbMap, the file, " + fname + " is too short!"),"Error in argument",FatalErrorInArgument,"end of file is true, file too short!");
}

void VHDPrimaryGeneratorAction::ErrorFileNotFound(const G4String& fname)
{
	G4Exception(G4String("VHDPrimaryGeneratorAction::ReadDoseMapFile, the file, " + fname + " is not found!"),"Error in argument",FatalErrorInArgument,"file not found!");
}


