
#include "PrimaryGeneratorAction.hh"

#include "DetectorConstruction.hh"
#include "PrimaryGeneratorMessenger.hh"

#include "G4Event.hh"
#include "G4ParticleGun.hh"
#include "G4ParticleTable.hh"
#include "G4ParticleDefinition.hh"
#include "G4SystemOfUnits.hh"
#include "Randomize.hh"

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

PrimaryGeneratorAction::PrimaryGeneratorAction(DetectorConstruction* det)
:G4VUserPrimaryGeneratorAction(),                                              
 fParticleGun(0),
 fDetector(det),
 fRndmBeam(0),
 fEbeamCumul(0),       
 fGunMessenger(0)
{
  fParticleGun  = new G4ParticleGun(1);
  G4ParticleDefinition* particle
           = G4ParticleTable::GetParticleTable()->FindParticle("proton");
  fParticleGun->SetParticleDefinition(particle);
  fParticleGun->SetParticleEnergy(200*MeV);
  fParticleGun->SetParticleMomentumDirection(G4ThreeVector(1.,0.,0.));
    
  //create a messenger for this class
  fGunMessenger = new PrimaryGeneratorMessenger(this);  
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

PrimaryGeneratorAction::~PrimaryGeneratorAction()
{
  delete fParticleGun;
  delete fGunMessenger;  
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

void PrimaryGeneratorAction::GeneratePrimaries(G4Event* anEvent)
{
  //this function is called at the begining of event
  //
  G4double x0 = -0.5*(fDetector->GetWorldSizeX());
  G4double y0 = 0.*cm, z0 = 0.*cm;
    
  //randomize the beam, if requested.
  //
  if (fRndmBeam > 0.) 
    {
      if (fRndmBeam > fDetector->GetAbsorSizeYZ())
        fRndmBeam = fDetector->GetAbsorSizeYZ(); 
      G4double rbeam = 0.5*fRndmBeam;
      y0 = (2*G4UniformRand()-1.)*rbeam;
      z0 = (2*G4UniformRand()-1.)*rbeam;
    }
  fParticleGun->SetParticlePosition(G4ThreeVector(x0,y0,z0));  
  fParticleGun->GeneratePrimaryVertex(anEvent);
  
  fEbeamCumul += fParticleGun->GetParticleEnergy(); 
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

