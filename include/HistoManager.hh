
#ifndef INCLUDE_HISTOMANAGER_HH_
#define INCLUDE_HISTOMANAGER_HH_

#include "g4root.hh"
#include "G4RunManager.hh"

class HistoManager {
public:
	// CTR
	HistoManager();
	~HistoManager();

	G4AnalysisManager* fAnalysisManager;

	void BookHisto();
	void Normalize(G4int nevt);
	//void Normalize(G4int nevt, G4double dose);
	void Save();
	void CreateCounterNtuple(G4String name);
	void CreateChargeNtuple(G4String charNtuplename);

	struct particleData{
	    	G4int 		p_counter;
	    	G4int 		ID; 	//ID particelle
	    	G4int 		PID; 	//PID parent ID particelle
	    	G4int 		PDG;	//PDG particelle
	    	G4double 	Ek;		//Energia cinetica
	    	G4double 	x;		//posizione sulla superficie
			G4double 	y;
	    	G4double 	z;
	    };

	struct particleChargeData{
				G4int		p_charge;
		    	G4int 		PDG;
		    	G4int 		ID;
		    	G4int 		PID;
		    	G4double 	Ek;
		    	G4double 	x;
		    	G4double 	y;
				G4double 	z;
		    	G4double 	msc_interaction;
		    	G4double 	ion_interaction;
		    	G4double 	eBrem_interaction;
		    	G4double 	x_creation;
		    	G4double 	y_creation;
		    	G4double	z_creation;
		    	G4double	compton_creation;
		    	G4double	photo_creation;
		    };

	struct particleKODELData{
		G4double		x;
		G4double 		y;
		G4double 		z;
		G4double 		edep;
		G4double 		eIon;
		G4int 			compt;
		G4int 			photo;
		G4int 			ion;
		G4int 			msc;
		G4double 		eBrem;
			};

	void FillCounterNtuple (particleData part, G4int nTupleID);  //passa le variabili a ttree TOT
	void FillChargeNtuple (particleChargeData partCharge, G4int nTupleCharID);
	void FillIncidentSignal(G4double Ek_in_signal, G4int ID);
	void FillKODELStep(particleKODELData partKODEL, G4int kID);
	void FillLastHisto();


private:
	G4int					nbofEvents;
	//G4double				totalDose;

	//sensitivity
	G4double 				sensitivity;
};

#endif /* INCLUDE_HISTOMANAGER_HH_ */
