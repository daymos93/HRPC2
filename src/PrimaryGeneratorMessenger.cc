#include "PrimaryGeneratorMessenger.hh"

#include "PrimaryGeneratorAction.hh"
#include "G4UIdirectory.hh"
#include "G4UIcmdWithADoubleAndUnit.hh"

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

PrimaryGeneratorMessenger::PrimaryGeneratorMessenger(
                                                   PrimaryGeneratorAction* Gun)
:G4UImessenger(),fAction(Gun),
 fGunDir(0),         
 fRndmCmd(0)
{ 
  fGunDir = new G4UIdirectory("/hrpc/gun/");
  fGunDir->SetGuidance("gun control");

  fRndmCmd = new G4UIcmdWithADoubleAndUnit("/hrpc/gun/rndm",this);
  fRndmCmd->SetGuidance("random lateral extension on the beam");
  fRndmCmd->SetParameterName("rBeam",false);
  fRndmCmd->SetRange("rBeam>=0.");
  fRndmCmd->SetUnitCategory("Length");
  fRndmCmd->AvailableForStates(G4State_PreInit,G4State_Idle);  
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

PrimaryGeneratorMessenger::~PrimaryGeneratorMessenger()
{
  delete fRndmCmd;
  delete fGunDir;
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

void PrimaryGeneratorMessenger::SetNewValue(G4UIcommand* command,
                                               G4String newValue)
{ 
  if (command == fRndmCmd)
   {fAction->SetRndmBeam(fRndmCmd->GetNewDoubleValue(newValue));}   
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

