
#include "G4RunManager.hh"
#include "G4UImanager.hh"
#include "Randomize.hh"

#include "DetectorConstruction.hh"
#include "PhysicsList.hh"
#include "PrimaryGeneratorAction.hh"

// To be able to use and combine the references physics lists
#include "G4PhysListFactory.hh"

#include "RunAction.hh"
#include "TrackingAction.hh"
#include "SteppingAction.hh"
#include "SteppingVerbose.hh"

#include "G4UIExecutive.hh"
#include "G4VisExecutive.hh"

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......
 
int main(int argc,char** argv) {

  //detect interactive mode (if no arguments) and define UI session
  G4UIExecutive* ui = nullptr;
  if (argc == 1) ui = new G4UIExecutive(argc,argv);

  //choose the Random engine
  CLHEP::HepRandom::setTheEngine(new CLHEP::RanecuEngine);
  
  //my Verbose output class
  G4VSteppingVerbose::SetInstance(new SteppingVerbose);
    
  //Construct the default run manager
  G4RunManager * runManager = new G4RunManager;

  //set mandatory initialization classes
  //
  DetectorConstruction*   det  = new DetectorConstruction();
  runManager->SetUserInitialization(det);
  
  //const G4String plName = "FTFP_BERT_HP";
  //G4PhysListFactory plFactory;
  //G4VModularPhysicsList *pl = plFactory.GetReferencePhysList( plName );
  PhysicsList*            pl = new PhysicsList();
  runManager->SetUserInitialization(pl);
  
  //set user action classes
  //
  PrimaryGeneratorAction* kin   = new PrimaryGeneratorAction(det);  
  RunAction*              run   = new RunAction(det,pl,kin);
  TrackingAction*         track = new TrackingAction(det,run);
  SteppingAction*         step  = new SteppingAction(det,run);
  
  runManager->SetUserAction(kin); 
  runManager->SetUserAction(run); 
  runManager->SetUserAction(track);  
  runManager->SetUserAction(step);

  //initialize visualization
  G4VisManager* visManager = nullptr;

  //get the pointer to the User Interface manager 
  G4UImanager* UImanager = G4UImanager::GetUIpointer();  

  if (ui)  {
   //interactive mode
   visManager = new G4VisExecutive;
   visManager->Initialize();
   UImanager->ApplyCommand("/control/execute vis.mac");
   ui->SessionStart();
   delete ui;
  }
  else  {
   //batch mode  
   G4String command = "/control/execute ";
   G4String fileName = argv[1];
   UImanager->ApplyCommand(command+fileName);
  }

  //job termination 
  delete visManager;
  delete runManager;
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......
