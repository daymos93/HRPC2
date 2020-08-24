
#include "HistoManager.hh"
#include "globals.hh"
#include "G4UnitsTable.hh"
#include "G4SystemOfUnits.hh"
#include "g4root.hh"
#include <cmath>

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
  fAnalysisManager->SetNtupleMerging(true);

  // Create directories
  //fAnalysisManager->SetHistoDirectoryName("histo");
  //fAnalysisManager->SetNtupleDirectoryName("ntuple");

  // Define histograms start values
  const G4int kMaxHisto = 4;
  //const G4String id[] = { "h0", "h1", "h2", "h3"};
  const G4String id[] = { "h0", "edep_phantom", "edep_phantom_zoomed", "projectile_range"/*, "sec_neutron", "sec_gamma", "edep_rpc"*/, "dose_phantom"};
  const G4String title[] =
                { "dummy",                                      //0
                  "E dep [MeV/mm] along phantom",              	//1
                  "E dep [MeV/mm] along phantom zoomed",        	//2
                  "Projectile range",							//3
				  //"Secondary neutron radiation produced",		//4
				  //"Secondary gamma radiation produced",		//5
				  //"E dep [MeV/mm] along RPC"					//6
				  "Dose dep [Gy/mm] along phantom"				//4
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
  fAnalysisManager->CreateH1(id[4], title[4], 300, 0, 300);

  //fAnalysisManager->CreateH1(id[5], title[5], 1024, 0, 150);
  //fAnalysisManager->CreateH1(id[6], title[6], 650, -5, 60);

  // Define 2D histograms
  //const G4int kMax2DHisto = 12;
  //const G4String id2D[] = { "xy_neutron_TOT", "xy_neutron_TOP", "xy_neutron_BOT",
  //		  	  	  	  	  	"xy_gamma_TOT", "xy_gamma_TOP", "xy_gamma_BOT",
  //	  	  	  	  	  	  	"xy_charge_TOT", "xy_charge_GAP1", "xy_charge_GAP2",
  //	  	  	  	  	  	  	"xy_creation_TOT", "xy_creation_GAP1", "xy_creation_GAP2"};
  //const G4String title2D[] =
  //              { "Incident neutron X-Y position (TOT)",            //0
  //                  "Incident neutron X-Y position (TOP)",            //1
  //                  "Incident neutron X-Y position (BOT)",        	//2
  //                  "Incident gamma X-Y position (TOT)",				//3
  //				  "Incident gamma X-Y position (TOP)",				//4
  //				  "Incident gamma X-Y position (BOT)",				//5
  //				  "Charge events X-Y position (TOT)",				//6
  //				  "Charge events X-Y position (GAP1)",				//7
  //				  "Charge events X-Y position (GAP2)",				//8
  //				  "Creation events X-Y position (TOT)",				//9
  //				  "Creation events X-Y position (GAP1)",			//10
  //				  "Creation events X-Y position (GAP2)",			//11
 //               };

  // Default values (to be reset via /analysis/h1/set command)
  //G4int nbins2D = 1000;
  //G4double lmin = -1000;
  //G4double lmax = 1000;
  //G4double ymin = -550;
  //G4double ymax = 550;

  // Create 2D histograms
  // as we have not yet set nbins, vmin, vmax
  //for (G4int k=0; k<kMax2DHisto; ++k) {
	 // fAnalysisManager->CreateH2(id2D[k], title2D[k], nbins2D, lmin, lmax, nbins2D, lmin, lmax);
  	 //  }

  // Define NTuples
  CreateCounterNtuple("neutron_incident_TOT");	// Create ntuple (id = 0)
  CreateCounterNtuple("neutron_incident_TOP");	// Create ntuple (id = 1)
  CreateCounterNtuple("neutron_incident_BOT");	// Create ntuple (id = 2)
  CreateCounterNtuple("gamma_incident_TOT");	// Create ntuple (id = 3)
  CreateCounterNtuple("gamma_incident_TOP");	// Create ntuple (id = 4)
  CreateCounterNtuple("gamma_incident_BOT");	// Create ntuple (id = 5)

  CreateCounterNtuple("neutron_gap_TOT");	// Create ntuple (id = 6)
  CreateCounterNtuple("neutron_gap_TOP");	// Create ntuple (id = 7)
  CreateCounterNtuple("neutron_gap_BOT");	// Create ntuple (id = 8)
  CreateCounterNtuple("gamma_gap_TOT");		// Create ntuple (id = 9)
  CreateCounterNtuple("gamma_gap_TOP");		// Create ntuple (id = 10)
  CreateCounterNtuple("gamma_gap_BOT");		// Create ntuple (id = 11)

  /*CreateChargeNtuple("charge_TOT");				// Create ntuple (id = 6)
  CreateChargeNtuple("charge_Gap1");			// Create ntuple (id = 7)
  CreateChargeNtuple("charge_Gap2");			// Create ntuple (id = 8)

  fAnalysisManager->CreateNtuple("Ek_incident_signal", "Ek_incident_signal"); // Create ntuple (id = 9)
  	  fAnalysisManager->CreateNtupleDColumn("Ek_incident_signal"); 			  // column Id = 0
  	  fAnalysisManager->FinishNtuple();

  fAnalysisManager->CreateNtuple("KODEL_step", "KODEL_step"); // Create ntuple (id = 10)
  	  fAnalysisManager->CreateNtupleDColumn("x"); 			  // column Id = 0
  	  fAnalysisManager->CreateNtupleDColumn("y");
  	  fAnalysisManager->CreateNtupleDColumn("z");
  	  fAnalysisManager->CreateNtupleDColumn("deposited_energy_step");
  	  fAnalysisManager->CreateNtupleDColumn("ionization_energy_step");
  	  fAnalysisManager->CreateNtupleDColumn("photoelectric_phenomenons_step");
  	  fAnalysisManager->CreateNtupleDColumn("compton_phenomenons_step");
  	  fAnalysisManager->CreateNtupleDColumn("msc_phenomenons_step");
  	  fAnalysisManager->CreateNtupleDColumn("ionization_phenomenons_step");
  	  fAnalysisManager->CreateNtupleDColumn("eBrem_phenomenons_step");
  	  fAnalysisManager->FinishNtuple();*/

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

    G4double fac2 = (1/CLHEP::gray)/(nbofEvents * fAnalysisManager->GetH1Width(4));

    fAnalysisManager->ScaleH1(4, fac2);
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

void HistoManager::Save(){
    fAnalysisManager->Write();
    fAnalysisManager->CloseFile();
    delete fAnalysisManager;
    fAnalysisManager = 0;
}

void HistoManager::CreateCounterNtuple(G4String name){
	  // Create ntuple
	  fAnalysisManager->CreateNtuple(name, name);
	  fAnalysisManager->CreateNtupleDColumn("PARTICLE_INCIDENT"); // column Id = 0
	  fAnalysisManager->CreateNtupleDColumn("PARTICLE_DATA_GROUP"); // column Id = 1
	  fAnalysisManager->CreateNtupleDColumn("ID");
	  fAnalysisManager->CreateNtupleDColumn("PARENT_ID");
	  fAnalysisManager->CreateNtupleDColumn("KINETIC_ENERGY");
	  fAnalysisManager->CreateNtupleDColumn("x");
	  fAnalysisManager->CreateNtupleDColumn("y");
	  fAnalysisManager->CreateNtupleDColumn("z");
	  fAnalysisManager->FinishNtuple();
}

void HistoManager::CreateChargeNtuple(G4String charNtuplename){
	fAnalysisManager->CreateNtuple(charNtuplename, charNtuplename);
	fAnalysisManager->CreateNtupleDColumn("p_charge");
	fAnalysisManager->CreateNtupleDColumn("PARTICLE_DATA_GROUP");
	fAnalysisManager->CreateNtupleDColumn("ID");
	fAnalysisManager->CreateNtupleDColumn("PARENT_ID");
	fAnalysisManager->CreateNtupleDColumn("KINETIC_ENERGY");
	fAnalysisManager->CreateNtupleDColumn("x");
	fAnalysisManager->CreateNtupleDColumn("y");
	fAnalysisManager->CreateNtupleDColumn("z");
	fAnalysisManager->CreateNtupleDColumn("msc_interaction");
	fAnalysisManager->CreateNtupleDColumn("ion_interaction");
	fAnalysisManager->CreateNtupleDColumn("eBrem_interaction");
	fAnalysisManager->CreateNtupleDColumn("x_creation");
	fAnalysisManager->CreateNtupleDColumn("y_creation");
	fAnalysisManager->CreateNtupleDColumn("z_creation");
	fAnalysisManager->CreateNtupleDColumn("compton_creation");
	fAnalysisManager->CreateNtupleDColumn("photo_creation");
	fAnalysisManager->FinishNtuple();
}

void HistoManager::FillCounterNtuple(particleData part, G4int nTupleID){
	//p_incident_TOT = part;
	G4AnalysisManager* analysisManager = G4AnalysisManager::Instance();
	analysisManager->FillNtupleDColumn(nTupleID, 0, part.p_counter);
	analysisManager->FillNtupleDColumn(nTupleID, 1, part.PDG);
	analysisManager->FillNtupleDColumn(nTupleID, 2, part.ID);
	analysisManager->FillNtupleDColumn(nTupleID, 3, part.PID);
	analysisManager->FillNtupleDColumn(nTupleID, 4, part.Ek);
	analysisManager->FillNtupleDColumn(nTupleID, 5, part.x);
	analysisManager->FillNtupleDColumn(nTupleID, 6, part.y);
	analysisManager->FillNtupleDColumn(nTupleID, 7, part.z);
	analysisManager->AddNtupleRow(nTupleID);
}

void HistoManager::FillChargeNtuple(particleChargeData partCharge, G4int nTupleCharID){
	G4AnalysisManager* analysisManager = G4AnalysisManager::Instance();
	analysisManager->FillNtupleDColumn(nTupleCharID, 0, partCharge.p_charge);
	analysisManager->FillNtupleDColumn(nTupleCharID, 1, partCharge.PDG);
	analysisManager->FillNtupleDColumn(nTupleCharID, 2, partCharge.ID);
	analysisManager->FillNtupleDColumn(nTupleCharID, 3, partCharge.PID);
	analysisManager->FillNtupleDColumn(nTupleCharID, 4, partCharge.Ek);
	analysisManager->FillNtupleDColumn(nTupleCharID, 5, partCharge.x);
	analysisManager->FillNtupleDColumn(nTupleCharID, 6, partCharge.y);
	analysisManager->FillNtupleDColumn(nTupleCharID, 7, partCharge.z);
	analysisManager->FillNtupleDColumn(nTupleCharID, 8, partCharge.msc_interaction);
	analysisManager->FillNtupleDColumn(nTupleCharID, 9, partCharge.ion_interaction);
	analysisManager->FillNtupleDColumn(nTupleCharID, 10, partCharge.eBrem_interaction);
	analysisManager->FillNtupleDColumn(nTupleCharID, 11, partCharge.x_creation);
	analysisManager->FillNtupleDColumn(nTupleCharID, 12, partCharge.y_creation);
	analysisManager->FillNtupleDColumn(nTupleCharID, 13, partCharge.z_creation);
	analysisManager->FillNtupleDColumn(nTupleCharID, 14, partCharge.compton_creation);
	analysisManager->FillNtupleDColumn(nTupleCharID, 15, partCharge.photo_creation);
	analysisManager->AddNtupleRow(nTupleCharID);
}

void HistoManager::FillIncidentSignal(G4double Ek_in_signal, G4int ID){
	G4AnalysisManager* analysisManager = G4AnalysisManager::Instance();
	analysisManager->FillNtupleDColumn(ID, 0, Ek_in_signal);
	analysisManager->AddNtupleRow(ID);
}

void HistoManager::FillKODELStep(particleKODELData partKODEL, G4int kID){
	G4AnalysisManager* analysisManager = G4AnalysisManager::Instance();
	analysisManager->FillNtupleDColumn(kID, 0, partKODEL.x);
	analysisManager->FillNtupleDColumn(kID, 1, partKODEL.y);
	analysisManager->FillNtupleDColumn(kID, 2, partKODEL.z);
	analysisManager->FillNtupleDColumn(kID, 3, partKODEL.edep);
	analysisManager->FillNtupleDColumn(kID, 4, partKODEL.eIon);
	analysisManager->FillNtupleDColumn(kID, 5, partKODEL.photo);
	analysisManager->FillNtupleDColumn(kID, 6, partKODEL.compt);
	analysisManager->FillNtupleDColumn(kID, 7, partKODEL.msc);
	analysisManager->FillNtupleDColumn(kID, 7, partKODEL.ion);
	analysisManager->FillNtupleDColumn(kID, 7, partKODEL.eBrem);
	analysisManager->AddNtupleRow(kID);
}

void HistoManager::FillLastHisto(){

	G4double neutron_incident_sensitivity= fAnalysisManager->GetNtuple(0)->entries();
	if (fAnalysisManager->GetNtuple(6)->entries() != 0){
		G4double p_charge_sensitivity = fAnalysisManager->GetNtuple(6)->entries();

		//compute sensitivity
		sensitivity = (p_charge_sensitivity/neutron_incident_sensitivity)*100;	// calcolo sensitività
		fAnalysisManager->CreateH1("sensitivity","sensitivity",10,0,2);  //Id = 4
		fAnalysisManager->FillH1(4,sensitivity);
		fAnalysisManager->SetH1YAxisTitle(4,"sensitivity %");
	}
}
