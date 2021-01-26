#ifndef MuNtuple_MuNtupleConfig_h
#define MuNtuple_MuNtupleConfig_h

/** \class MuNtupleConfig MuNtupleConfig.h MuDPGAnalysis/MuNtuples/src/MuNtupleConfig.h
 *  
 * Helper class to handle :
 * - configuration parameters for edm::ParameterSet
 * - DB information from edm::EventSetup
 * - HLT configuration from dm::EventSetup and dm::Run
 *
 * \author C. Battilana (INFN BO)
 *
 *
 */

#include "FWCore/Utilities/interface/InputTag.h"
#include "FWCore/Framework/interface/ESHandle.h"

#include "CalibMuon/DTDigiSync/interface/DTTTrigBaseSync.h"
#include "Geometry/CommonDetUnit/interface/GlobalTrackingGeometry.h"
#include "Geometry/DTGeometry/interface/DTGeometry.h"
#include "Geometry/GEMGeometry/interface/GEMGeometry.h"
#include "Geometry/CSCGeometry/interface/CSCGeometry.h"
#include "TrackingTools/TransientTrack/interface/TransientTrackBuilder.h"
#include "TrackingTools/Records/interface/TransientTrackRecord.h"

#include "RecoMuon/TrackingTools/interface/MuonServiceProxy.h"

#include <map>
#include <string>
#include <memory>

namespace edm
{
  class ParameterSet;
  class EventSetup;
  class Run;
}

class MuNtupleConfig
{

 public :

  enum class PhaseTag { PH1 = 0, PH2 };

  /// Constructor
  MuNtupleConfig(const edm::ParameterSet & config);

  /// Update EventSetup information
  void getES(const edm::EventSetup & environment);

  /// Update EventSetup information
  void getES(const edm::Run &run, 
	     const edm::EventSetup & environment);

  edm::EventSetup passES();

  /// Map containing different input tags
  std::map<std::string, edm::InputTag> m_inputTags;

  /// The class to handle DT trigger time pedestals
  std::map<PhaseTag, std::unique_ptr<DTTTrigBaseSync>> m_dtSyncs;

  /// Handle to the tracking geometry
  edm::ESHandle<GlobalTrackingGeometry> m_trackingGeometry;

  /// Handle to the DT geometry
  edm::ESHandle<DTGeometry> m_dtGeometry;

  /// Handle to the GEM geometry
  edm::ESHandle<GEMGeometry> m_gemGeometry;

  /// Handle to the CSC geometry
  edm::ESHandle<CSCGeometry> m_cscGeometry;

  /// Handle to the Transient Track Builder
  edm::ESHandle<TransientTrackBuilder> m_transientTrackBuilder;

  float residual_x_cut;
    
  //edm::ParameterSet & muon_service_parameter{};
  MuonServiceProxy *muon_service;

};

#endif
