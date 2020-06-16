
#include "RunAction.hh"
#include "DetectorConstruction.hh"
#include "PhysicsList.hh"
#include "StepMax.hh"
#include "PrimaryGeneratorAction.hh"

#include "G4Run.hh"
#include "G4RunManager.hh"
#include "G4UnitsTable.hh"
#include "G4SystemOfUnits.hh"
#include "G4ios.hh"

#include "Randomize.hh"

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

RunAction::RunAction(DetectorConstruction* det, PhysicsList* phys,
                     PrimaryGeneratorAction* kin)
 : G4UserRunAction(),
   /*fAnalysisManager(0),*/ fDetector(det), fPhysics(phys), fKinematic(kin),
   fTallyEdep(new G4double[kMaxTally]), fProjRange(0.), fProjRange2(0.),
   fEdeptot(0.), fEniel(0.), fNbPrimarySteps(0), fRange(0)
{ 
	histo = new HistoManager();
  // Book predefined histograms
	histo -> BookHisto();
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

RunAction::~RunAction()
{
  delete [] fTallyEdep;
  delete histo;
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

void RunAction::BeginOfRunAction(const G4Run* aRun)
{  
  G4cout << "### Run " << aRun->GetRunID() << " start." << G4endl;

  if(!histo->fAnalysisManager) { histo->BookHisto(); }
  
  CLHEP::HepRandom::showEngineStatus();
     
  //initialize projected range, tallies, Ebeam, and book histograms
  //
  fNbPrimarySteps = 0;
  fRange = 0;
  fProjRange = fProjRange2 = 0.;
  fEdeptot = fEniel = 0.;
  for (G4int j=0; j<kMaxTally; ++j) { fTallyEdep[j] = 0.; }
  fKinematic->ResetEbeamCumul();
  
  if (histo->fAnalysisManager->IsActive()) {
	  histo->fAnalysisManager->OpenFile();

    // histogram "1" is defined by the length of the target
    // zoomed histograms are defined by UI command  
    G4double length  = fDetector->GetAbsorSizeX();
    G4double stepMax = fPhysics->GetStepMaxProcess()->GetMaxStep();
    G4int nbmin = 100;
    G4int nbBins = (G4int)(0.5 + length/stepMax);
    if (nbBins < nbmin) nbBins = nbmin;
    histo->fAnalysisManager->SetH1(1, nbBins, 0., length, "mm");
  }
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

void RunAction::EndOfRunAction(const G4Run* aRun)
{
  G4int nbofEvents = aRun->GetNumberOfEvent();
  if (nbofEvents == 0) return;

  //run conditions
  //  
  const G4Material* material = fDetector->GetAbsorMaterial();
  G4double density = material->GetDensity();
   
  G4String particle = fKinematic->GetParticleGun()->GetParticleDefinition()
                      ->GetParticleName();    
  G4double energy = fKinematic->GetParticleGun()->GetParticleEnergy();
  G4cout << "\n The run consists of " << nbofEvents << " "<< particle << " of "
         << G4BestUnit(energy,"Energy") << " through " 
         << G4BestUnit(fDetector->GetAbsorSizeX(),"Length") << " of "
         << material->GetName() << " ( density: "
         << G4BestUnit(density,"Volumic Mass") << ")" << G4endl;
         
  //compute projected range and straggling
  //
  if(fRange > 0) {
    fProjRange /= fRange; 
    fProjRange2 /= fRange;
  }
  G4double rms = fProjRange2 - fProjRange*fProjRange;        
  if (rms>0.) rms = std::sqrt(rms); else rms = 0.;

  G4double nstep = G4double(fNbPrimarySteps)/G4double(nbofEvents);

  G4cout.precision(6);       
  G4cout << "\n Projected Range= "<< G4BestUnit(fProjRange,"Length")
         << "   rms= "            << G4BestUnit( rms,"Length")
         << G4endl;
  G4cout << " Mean number of primary steps = "<< nstep << G4endl;

  //compute energy deposition and niel
  //
  fEdeptot /= nbofEvents; 
  G4cout << " Total energy deposit= "<< G4BestUnit(fEdeptot,"Energy")
         << G4endl;
  fEniel /= nbofEvents; 
  G4cout << " niel energy deposit = "<< G4BestUnit(fEniel,"Energy")
         << G4endl;
     
  //print dose in tallies
  //
  G4int tallyNumber = fDetector->GetTallyNumber();
  if (tallyNumber > 0) {
    G4double Ebeam = fKinematic->GetEbeamCumul();
    G4cout << "\n---------------------------------------------------------\n";
    G4cout << " Cumulated Doses : \tEdep      \tEdep/Ebeam \tDose" << G4endl;
    for (G4int j=0; j < tallyNumber; ++j) {
      G4double Edep = fTallyEdep[j], ratio = 100*Edep/Ebeam;
      G4double tallyMass = fDetector->GetTallyMass(j);      
      G4double Dose = Edep/tallyMass;
      G4cout << " tally " << j << ": \t \t"
             << G4BestUnit(Edep,"Energy") << "\t"
             << ratio << " % \t"
             << G4BestUnit(Dose,"Dose")   << G4endl;
    }
    G4cout << "\n---------------------------------------------------------\n";
    G4cout << G4endl; 
  }

  if (histo->fAnalysisManager->IsActive() ) {
    // normalize histograms
    //
	  histo->Normalize(nbofEvents);
    /*for (G4int j=1; j<3; ++j) {
      G4double binWidth = histo->fAnalysisManager->GetH1Width(j);
      G4double fac = (mm/MeV)/(nbofEvents * binWidth);
      histo->fAnalysisManager->ScaleH1(j, fac);
    }
    histo->fAnalysisManager->ScaleH1(3, 1./nbofEvents);*/
 
    // save histograms
	  histo->Save();
    /*histo->fAnalysisManager->Write();
    histo->fAnalysisManager->CloseFile();
    delete histo->fAnalysisManager;
    histo->fAnalysisManager = 0;*/
  }
   
  // show Rndm status
  //
  CLHEP::HepRandom::showEngineStatus();
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......
/*
void RunAction::BookHisto()
{
  // Create or get analysis manager
  // The choice of analysis technology is done via selection of a namespace
  // in HistoManager.hh
  fAnalysisManager = G4AnalysisManager::Instance();
  fAnalysisManager->SetFileName("testem7");
  fAnalysisManager->SetVerboseLevel(1);
  fAnalysisManager->SetActivation(true);  // enable inactivation of histograms

  // Define histograms start values
  const G4int kMaxHisto = 4;
  const G4String id[] = { "h0", "h1", "h2", "h3" };
  const G4String title[] = 
                { "dummy",                                      //0
                  "Edep (MeV/mm) along absorber ",              //1
                  "Edep (MeV/mm) along absorber zoomed",        //2
                  "projectile range"                            //3
                 };  

  // Default values (to be reset via /analysis/h1/set command)               
  G4int nbins = 100;
  G4double vmin = 0.;
  G4double vmax = 100.;

  // Create all histograms as inactivated 
  // as we have not yet set nbins, vmin, vmax
  for (G4int k=0; k<kMaxHisto; ++k) {
    G4int ih = fAnalysisManager->CreateH1(id[k], title[k], nbins, vmin, vmax);
    G4bool activ = false;
    if (k == 1) activ = true;
    fAnalysisManager->SetH1Activation(ih, activ);
  }
}
*/
//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......
