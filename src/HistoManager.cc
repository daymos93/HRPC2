
#include "HistoManager.hh"
#include "globals.hh"
#include "G4UnitsTable.hh"
#include "G4SystemOfUnits.hh"


HistoManager::HistoManager():
fAnalysisManager(0){
	// TODO Auto-generated constructor stub
}

HistoManager::~HistoManager() {}

void HistoManager::BookHisto()
{
	fAnalysisManager = G4AnalysisManager::Instance();
	fAnalysisManager->SetFileName("hrpc");
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

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

void HistoManager::Normalize(G4int nevt){
	nbofEvents = nevt;
    for (G4int j=1; j<3; ++j) {
      G4double binWidth = fAnalysisManager->GetH1Width(j);
      G4double fac = (CLHEP::mm/CLHEP::MeV)/(nbofEvents * binWidth);
      fAnalysisManager->ScaleH1(j, fac);
    }
    fAnalysisManager->ScaleH1(3, 1./nbofEvents);
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

void HistoManager::Save(){
    fAnalysisManager->Write();
    fAnalysisManager->CloseFile();
    delete fAnalysisManager;
    fAnalysisManager = 0;
}

