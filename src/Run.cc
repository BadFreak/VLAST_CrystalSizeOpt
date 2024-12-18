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
//
/// \file electromagnetic/TestEm11/src/Run.cc
/// \brief Implementation of the Run class
//
// $Id: Run.cc 71376 2013-06-14 07:44:50Z maire $
// 
//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......
//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

#include "Run.hh"
#include "PrimaryGeneratorAction.hh"
#include "HistoManager.hh"

#include "G4SystemOfUnits.hh"
#include "G4UnitsTable.hh"
#include "G4PhysicalConstants.hh"

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

Run::Run()
: G4Run(),
  fParticle(0), fEkin(0.),
  fDecayCount(0), fTimeCount(0), fPrimaryTime(0.)
{
	fEkinTot[0] = fPbalance[0] = fEventTime[0] = 0. ;
	fEkinTot[1] = fPbalance[1] = fEventTime[1] = DBL_MAX;
	fEkinTot[2] = fPbalance[2] = fEventTime[2] = 0. ;     
}
//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

Run::~Run()
{ }

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

void Run::SetPrimary(G4ParticleDefinition* particle, G4double energy)
{ 
	fParticle = particle;
	fEkin = energy;
} 

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

void Run::ParticleCount(G4String name, G4double Ekin)
{
	G4cout << "................666666666666666666666............" << G4endl;
	std::map<G4String, ParticleData>::iterator it = fParticleDataMap.find(name);
  	if ( it == fParticleDataMap.end()) {
    	fParticleDataMap[name] = ParticleData(1, Ekin, Ekin, Ekin);
  	} else {
    	ParticleData& data = it->second;
    	data.fCount++;
    	data.fEmean += Ekin;
    	//update min max
    	G4double emin = data.fEmin;
    	if (Ekin < emin) data.fEmin = Ekin;
    	G4double emax = data.fEmax;
    	if (Ekin > emax) data.fEmax = Ekin; 
	}   
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

void Run::Balance(G4double Ekin, G4double Pbal)
{
	fDecayCount++;
	fEkinTot[0] += Ekin;
	//update min max  
	if (fDecayCount == 1) fEkinTot[1] = fEkinTot[2] = Ekin;
	if (Ekin < fEkinTot[1]) fEkinTot[1] = Ekin;
	if (Ekin > fEkinTot[2]) fEkinTot[2] = Ekin;

	fPbalance[0] += Pbal;
	//update min max   
	if (fDecayCount == 1) fPbalance[1] = fPbalance[2] = Pbal;  
	if (Pbal < fPbalance[1]) fPbalance[1] = Pbal;
	if (Pbal > fPbalance[2]) fPbalance[2] = Pbal;    
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

void Run::EventTiming(G4double time)
{
	fTimeCount++;  
	fEventTime[0] += time;
	if (fTimeCount == 1) fEventTime[1] = fEventTime[2] = time;  
	if (time < fEventTime[1]) fEventTime[1] = time;
	if (time > fEventTime[2]) fEventTime[2] = time;             
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

void Run::PrimaryTiming(G4double ptime)
{
	fPrimaryTime += ptime;
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......
/*
void Run::Merge(const G4Run* run)
{
  const Run* localRun = static_cast<const Run*>(run);

  //primary particle info
  //
  fParticle = localRun->fParticle;
  fEkin     = localRun->fEkin;
   
  // accumulate sums
  //
  fDecayCount  += localRun->fDecayCount;
  fTimeCount   += localRun->fTimeCount;  
  fPrimaryTime += localRun->fPrimaryTime;

  fEkinTot[0]   += localRun->fEkinTot[0];
  fPbalance[0]  += localRun->fPbalance[0];
  fEventTime[0] += localRun->fEventTime[0];
  
  G4double min,max;  
  min = localRun->fEkinTot[1]; max = localRun->fEkinTot[2];
  if (fEkinTot[1] > min) fEkinTot[1] = min;
  if (fEkinTot[2] < max) fEkinTot[2] = max;
  //
  min = localRun->fPbalance[1]; max = localRun->fPbalance[2];
  if (fPbalance[1] > min) fPbalance[1] = min;
  if (fPbalance[2] < max) fPbalance[2] = max;
  //
  min = localRun->fEventTime[1]; max = localRun->fEventTime[2];
  if (fEventTime[1] > min) fEventTime[1] = min;
  if (fEventTime[2] < max) fEventTime[2] = max;
                
  //maps
  std::map<G4String,ParticleData>::const_iterator itn;
  for (itn = localRun->fParticleDataMap.begin(); 
       itn != localRun->fParticleDataMap.end(); ++itn) {
    
    G4String name = itn->first;
    const ParticleData& localData = itn->second;   
    if ( fParticleDataMap.find(name) == fParticleDataMap.end()) {
      fParticleDataMap[name]
       = ParticleData(localData.fCount, 
                      localData.fEmean, 
                      localData.fEmin, 
                      localData.fEmax);
    }
    else {
      ParticleData& data = fParticleDataMap[name];   
      data.fCount += localData.fCount;
      data.fEmean += localData.fEmean;
      G4double emin = localData.fEmin;
      if (emin < data.fEmin) data.fEmin = emin;
      G4double emax = localData.fEmax;
      if (emax > data.fEmax) data.fEmax = emax; 
    }   
  }
  
  G4Run::Merge(run); 
} 
*/  
//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

void Run::EndOfRun() 
{
	 G4int nbEvents = numberOfEvent;
	 G4String partName = fParticle->GetParticleName();
	 
	 G4cout << "\n ======================== run summary ======================";  
	 G4cout << "\n The run was " << nbEvents << " " << partName << " of "
			<< G4BestUnit(fEkin,"Energy");
	 G4cout << "\n ===========================================================\n";
	 G4cout << G4endl;
	 if (nbEvents == 0) { return; }
	 
	 G4int prec = 4, wid = prec + 2;
	 G4int dfprec = G4cout.precision(prec);
		  
	 //particle count
	 //
	 G4cout << " Nb of generated particles: \n" << G4endl;
		 
	 std::map<G4String,ParticleData>::iterator it;               
	 for (it = fParticleDataMap.begin(); it != fParticleDataMap.end(); it++) { 
		G4String name     = it->first;
		ParticleData data = it->second;
		G4int count    = data.fCount;
		G4double eMean = data.fEmean/count;
		G4double eMin  = data.fEmin;
		G4double eMax  = data.fEmax;    
			 
		G4cout << "  " << std::setw(13) << name << ": " << std::setw(7) << count
			   << "  Emean = " << std::setw(wid) << G4BestUnit(eMean, "Energy")
			   << "\t( "  << G4BestUnit(eMin, "Energy")
			   << " --> " << G4BestUnit(eMax, "Energy") 
			   << ")" << G4endl;           
	 }
	 
	 //energy momentum balance
	 //

	 if (fDecayCount > 0) {
		G4double Ebmean = fEkinTot[0]/fDecayCount;
		G4double Pbmean = fPbalance[0]/fDecayCount;
			 
		G4cout << "\n   Ekin Total (Q): mean = "
			   << std::setw(wid) << G4BestUnit(Ebmean, "Energy")
			   << "\t( "  << G4BestUnit(fEkinTot[1], "Energy")
			   << " --> " << G4BestUnit(fEkinTot[2], "Energy")
			   << ")" << G4endl;    
			   
		G4cout << "\n   Momentum balance (excluding gamma desexcitation): mean = " 
			   << std::setw(wid) << G4BestUnit(Pbmean, "Energy")
			   << "\t( "  << G4BestUnit(fPbalance[1], "Energy")
			   << " --> " << G4BestUnit(fPbalance[2], "Energy")
			   << ")" << G4endl;
	 }
				
	 //total time of life
	  //fHitNu = 0;
	 //
	 if (fTimeCount > 0) {
		G4double Tmean = fEventTime[0]/fTimeCount;
		G4double halfLife = Tmean*std::log(2.);
	   
		G4cout << "\n   Total time of life : mean = "
			   << std::setw(wid) << G4BestUnit(Tmean, "Time")
			   << "  half-life = "
	 // fHitNu = 0;
			   << std::setw(wid) << G4BestUnit(halfLife, "Time")
			   << "   ( "  << G4BestUnit(fEventTime[1], "Time")
			   << " --> "  << G4BestUnit(fEventTime[2], "Time")
			   << ")" << G4endl;
	 }
				
	 //activity of primary ion
	 //
	 G4double pTimeMean = fPrimaryTime/nbEvents;
	 G4double molMass = fParticle->GetAtomicMass()*g/mole;
	 G4double nAtoms_perUnitOfMass = Avogadro/molMass;
	 G4double Activity_perUnitOfMass = 0.0;
	 if (pTimeMean > 0.0){ 
	 	Activity_perUnitOfMass = nAtoms_perUnitOfMass/pTimeMean; 
	}
	   
	 G4cout << "\n   Activity of " << partName << " = "
				<< std::setw(wid)  << Activity_perUnitOfMass*g/becquerel
				<< " Bq/g   ("     << Activity_perUnitOfMass*g/curie
				<< " Ci/g) \n" 
				<< G4endl;
				
	 //normalize histograms
	 //
	 G4GenericAnalysisManager* analysisManager = G4GenericAnalysisManager::Instance();
	 G4double factor = 100./nbEvents;
	 analysisManager->ScaleH1(1,factor);
	 analysisManager->ScaleH1(2,factor);
	 analysisManager->ScaleH1(3,factor);
	 analysisManager->ScaleH1(4,factor);
	 analysisManager->ScaleH1(5,factor);
													
	 // remove all contents in fParticleDataMap
	 // 
	 fParticleDataMap.clear(); 

	 // restore default precision
	 // 
	 G4cout.precision(dfprec);
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......
