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
/// \file radioactivedecay/rdecay01/src/DetectorConstruction.cc
/// \brief Implementation of the DetectorConstruction class
//
//
// $Id: DetectorConstruction.cc 78307 2013-12-11 10:55:57Z gcosmo $
// 
//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

#include "DetectorConstruction.hh"
#include "SteppingAction.hh"

#include "G4NistManager.hh"
#include "G4Box.hh"
#include "G4Orb.hh"
#include "G4LogicalVolume.hh"
#include "G4PVPlacement.hh"
#include "G4SystemOfUnits.hh"
#include "G4VisAttributes.hh"
#include "G4Colour.hh"
#include "G4GeometryTolerance.hh"
#include "G4GeometryManager.hh"
#include "G4Element.hh"
#include "G4Material.hh"
#include "G4GlobalMagFieldMessenger.hh"
#include "G4SubtractionSolid.hh"

//#include "yaml-cpp/yaml.h"

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

DetectorConstruction::DetectorConstruction()
 : G4VUserDetectorConstruction()
{}

DetectorConstruction::~DetectorConstruction()
{}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

G4VPhysicalVolume* DetectorConstruction::Construct()
{
    // define a material

    G4Material* Vacuum =
        G4NistManager::Instance()->FindOrBuildMaterial("G4_Galactic");// vacuum  

    G4Material* Kapton =
        G4NistManager::Instance()->FindOrBuildMaterial("G4_KAPTON"); 

    G4Material* Aluminum =
        G4NistManager::Instance()->FindOrBuildMaterial("G4_Al"); 

	G4Material* Air = 
		G4NistManager::Instance()->FindOrBuildMaterial("G4_AIR");	

	G4Material* SiO2 = 
		G4NistManager::Instance()->FindOrBuildMaterial("G4_SILICON_DIOXIDE");	

    G4Element* elH = 
        G4NistManager::Instance()->FindOrBuildElement("H");

    G4Element* elO = 
        G4NistManager::Instance()->FindOrBuildElement("O");

    G4Element* elC = 
        G4NistManager::Instance()->FindOrBuildElement("C");
	
	G4Element* elCs = G4NistManager::Instance()->FindOrBuildElement("Cs");
    G4Element* elI = G4NistManager::Instance()->FindOrBuildElement("I");
    G4Element* elTl = G4NistManager::Instance()->FindOrBuildElement("Tl");	
    G4double density = 4.51 * g / cm3; // 密度
    G4Material* CsI_Tl = new G4Material("CsI_Tl", density, 3); // 3 表示有 3 种成分

    // 按质量分数添加元素（假设 Tl 的掺杂量为 0.1%）
    CsI_Tl->AddElement(elCs, 0.511);
    CsI_Tl->AddElement(elI,  0.4889);
    CsI_Tl->AddElement(elTl, 0.0001); // Tl 掺杂
   
	G4String name;
    G4int ncomponents;
	G4int natoms;
    G4double fractionmass;

	density = 1.25*g/cm3;
	//******************************************************
    // Option to switch checking of volumes Overlaps
    G4bool checkOverlaps = true;
	
	//G4bool overlaps = myPhysicalVolume->CheckOverlaps(); 
	//if(overlaps)	{G4cout << "Warning: Overlaps detected in the geometry!" << G4endl;}
    
	// World : Full sphere shape
    G4double solidWorld_rmax = 100*cm;
    G4Orb* solidWorld 
			= new G4Orb("World", solidWorld_rmax);             
    G4LogicalVolume* logicWorld 
			= new G4LogicalVolume(solidWorld, Air, "World");   
    G4VPhysicalVolume* physiWorld
			= new G4PVPlacement(0, G4ThreeVector(), logicWorld, "World", 0, false, 0, checkOverlaps);       

	//******************************************************
    //Absorber & Crystal(Scintillator) & PCB
	const G4double SIDE = 300*mm;
	//YAML::Node config = YAML::LoadFile(yamlFile);
	//G4int SideCrystalNo = config["crystalArraySize"].as<int>;
    G4int SideCrystalNo = 5;
    
	G4double crystalX = SIDE / SideCrystalNo;
    G4double crystalY = SIDE / SideCrystalNo;
    
    //G4double crystalZ = config["crystalLength"].as<double>;
	G4double crystalZ = 200*mm;
    G4double crystalGapX = 0.*mm;
    G4double crystalGapY = 0.*mm;
    
	G4double crystalPositionX 	= - (SideCrystalNo - 1.) / 2. * crystalX;	
    G4double crystalPositionY	= - (SideCrystalNo - 1.) / 2. * crystalY;
    G4double crystalPositionZ	= crystalZ / 2 ;
    
    //******************************************************
    //******************************************************
 
  	//Scintillator && ESR Film
    G4Box* solidCrystal
			= new G4Box("Crystal", 0.5*crystalX, 0.5*crystalY, 0.5*crystalZ);    

    G4LogicalVolume* logicCrystal
			= new G4LogicalVolume(solidCrystal, CsI_Tl, "Crystal"); 
	
	//           y
	//			 /\					.
	//           |					.
	//           |					15	...
	//   ----------------->x
	//           |					10  ...  ...
	//           |					5	6	7	8	9
	//           |					0	1	2	3	4
	//

	for(G4int iy = 0; iy != SideCrystalNo; ++iy) {
		for(G4int ix = 0; ix != SideCrystalNo; ++ix) {
			G4double XNow = crystalPositionX + ix * (crystalX + crystalGapX) ;
			G4double YNow = crystalPositionY + iy * (crystalY + crystalGapY) ;
			G4double ZNow = crystalPositionZ;
			G4int copyNo = ix * SideCrystalNo + iy; 
			
			G4VPhysicalVolume* physicalCrystal
					= new G4PVPlacement(0, G4ThreeVector(XNow,YNow,ZNow), logicCrystal, "Crystal", logicWorld, false, copyNo, checkOverlaps);
		}
	}

  
// **********************************************************************
// **********************************************************************
// **********************************************************************
    G4VisAttributes* visAttributes = new G4VisAttributes(G4Colour(0.9,0.0,0.0));
    //G4VisAttributes* chamberVisAtt = new G4VisAttributes(G4Colour(1.0,1.0,0.0));
    visAttributes ->SetVisibility(false);
    logicWorld ->SetVisAttributes(visAttributes);
    //logicAbsorber ->SetVisAttributes(visAttributes);
    //logicPCB ->SetVisAttributes(visAttributes);
	//logicFilm ->SetVisAttributes(visAttributes);

    visAttributes = new G4VisAttributes(G4Colour(0.9,0.9,0.9));//white
    logicCrystal ->SetVisAttributes(visAttributes);

    // Set scoring volume to stepping action(where we will account energy deposit)
    SteppingAction* steppingAction = SteppingAction::Instance();
    steppingAction->SetVolume(logicCrystal);

    //always return the physical World
    return physiWorld;
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......
