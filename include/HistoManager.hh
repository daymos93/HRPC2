
#ifndef INCLUDE_HISTOMANAGER_HH_
#define INCLUDE_HISTOMANAGER_HH_

#include "g4root.hh"
//#include "G4RunManager.hh"

class HistoManager {
public:
	// CTR
	HistoManager();
	~HistoManager();

	G4AnalysisManager* fAnalysisManager;

	void BookHisto();
	void Normalize(G4int nevt);
	void Save();

private:
	G4int	nbofEvents;
};

#endif /* INCLUDE_HISTOMANAGER_HH_ */
