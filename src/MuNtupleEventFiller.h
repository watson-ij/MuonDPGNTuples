#ifndef MuNTuple_EventFiller_h
#define MuNTuple_EventFiller_h

#include "MuDPGAnalysis/MuonDPGNtuples/src/MuNtupleBaseFiller.h"

#include "FWCore/Framework/interface/ConsumesCollector.h"

#include <vector>
#include <cstdint>

class MuNtupleEventFiller : public MuNtupleBaseFiller
{

 public:

  /// Constructor
  MuNtupleEventFiller(edm::ConsumesCollector && collector,
		      const std::shared_ptr<MuNtupleConfig> config, 
		      std::shared_ptr<TTree> tree, const std::string & label);

  ///Destructor
  virtual ~MuNtupleEventFiller();
 
  /// Intialize function : setup tree branches etc ... 
  virtual void initialize() final;
  
  /// Clear branches before event filling 
  virtual void clear() final;

  /// Fill tree branches for a given events
  virtual void fill(const edm::Event & ev) final;    

 private :

  int m_runNumber;
  int  m_lumiBlock;
  int64_t m_eventNumber;

};
  
#endif
