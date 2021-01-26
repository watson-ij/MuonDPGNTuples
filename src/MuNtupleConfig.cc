/** \class MuNtupleConfig MuNtupleConfig.cc MuDPGAnalysis/MuonDPGNtuples/src/MuNtupleConfig.cc
 *  
 * Helper class to handle :
 * - configuration parameters for edm::ParameterSet
 * - DB information from edm::EventSetup
 *
 * \author C. Battilana (INFN BO)
 *
 *
 */

#include "MuDPGAnalysis/MuonDPGNtuples/src/MuNtupleConfig.h"

#include "FWCore/ParameterSet/interface/ParameterSet.h"
#include "FWCore/Framework/interface/EventSetup.h"
#include "FWCore/Framework/interface/Run.h"
#include "FWCore/Framework/interface/ConsumesCollector.h"

#include "Geometry/Records/interface/GlobalTrackingGeometryRecord.h"
#include "Geometry/Records/interface/MuonGeometryRecord.h"

#include "TrackingTools/TransientTrack/interface/TransientTrackBuilder.h"
#include "TrackingTools/Records/interface/TransientTrackRecord.h"

#include "CalibMuon/DTDigiSync/interface/DTTTrigSyncFactory.h"

#include "TString.h"
#include "TRegexp.h"

MuNtupleConfig::MuNtupleConfig(const edm::ParameterSet & config) 
{ 
  
  edm::InputTag none = edm::InputTag("none");
  
  m_inputTags["ph1DtDigiTag"] = config.getUntrackedParameter<edm::InputTag>("ph1DtDigiTag", none);
  m_inputTags["ph2DtDigiTag"] = config.getUntrackedParameter<edm::InputTag>("ph2DtDigiTag", none);

  m_inputTags["ph1DtSegmentTag"] = config.getUntrackedParameter<edm::InputTag>("ph1DtSegmentTag", none);
  m_inputTags["ph2DtSegmentTag"] = config.getUntrackedParameter<edm::InputTag>("ph2DtSegmentTag", none);

  if (m_inputTags["ph1DtSegmentTag"].label() != "none")
    m_dtSyncs[PhaseTag::PH1] = DTTTrigSyncFactory::get()->create(config.getUntrackedParameter<std::string>("ph1DTtTrigMode"),
								 config.getUntrackedParameter<edm::ParameterSet>("ph1DTtTrigModeConfig"));

  if (m_inputTags["ph2DtSegmentTag"].label() != "none")
    m_dtSyncs[PhaseTag::PH2] = DTTTrigSyncFactory::get()->create(config.getUntrackedParameter<std::string>("ph2DTtTrigMode"),
								 config.getUntrackedParameter<edm::ParameterSet>("ph2DTtTrigModeConfig"));

  m_inputTags["gemDigiTag"] = config.getUntrackedParameter<edm::InputTag>("gemDigiTag", none);
  
  m_inputTags["gemRecHitTag"] = config.getUntrackedParameter<edm::InputTag>("gemRecHitTag", 
none);

  m_inputTags["gemSegmentTag"] = config.getUntrackedParameter<edm::InputTag>("gemSegmentTag",none);

  m_inputTags["cscSegmentTag"] = config.getUntrackedParameter<edm::InputTag>("cscSegmentTag",none);

  m_inputTags["muonTag"] = config.getUntrackedParameter<edm::InputTag>("muonTag",none);

  m_inputTags["primaryVerticesTag"] = config.getUntrackedParameter<edm::InputTag>("primaryVerticesTag",none);

  m_inputTags["gemRecHitTag"] = config.getUntrackedParameter<edm::InputTag>("gemRecHitTag",none);

  residual_x_cut = static_cast<float>(config.getParameter<double>("residualXCut"));
}

void MuNtupleConfig::getES(const edm::EventSetup & environment) 
{ 

  if (m_inputTags["ph1DtSegmentTag"].label() != "none")
    m_dtSyncs[PhaseTag::PH1]->setES(environment);

  if (m_inputTags["ph2DtSegmentTag"].label() != "none")
    m_dtSyncs[PhaseTag::PH2]->setES(environment);

  environment.get<MuonGeometryRecord>().get(m_dtGeometry);
  environment.get<GlobalTrackingGeometryRecord>().get(m_trackingGeometry);
  environment.get<MuonGeometryRecord>().get(m_gemGeometry);
  environment.get<TransientTrackRecord>().get("TransientTrackBuilder", m_transientTrackBuilder);
  environment.get<MuonGeometryRecord>().get(m_cscGeometry);
     
 }

void MuNtupleConfig::getES(const edm::Run &run, const edm::EventSetup & environment) 
{
 
  getES(environment);

}


