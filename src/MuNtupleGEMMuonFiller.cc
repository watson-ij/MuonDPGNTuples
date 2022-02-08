#include "MuDPGAnalysis/MuonDPGNtuples/src/MuNtupleGEMMuonFiller.h"
#include "FWCore/Framework/interface/Event.h"
#include "FWCore/Framework/interface/LuminosityBlock.h"
#include "FWCore/Framework/interface/EDAnalyzer.h"
#include "FWCore/Framework/interface/ESHandle.h"
#include "DataFormats/Common/interface/Ref.h"

#include "Geometry/GEMGeometry/interface/GEMGeometry.h"
#include "Geometry/CSCGeometry/interface/CSCGeometry.h"

#include "DataFormats/DetId/interface/DetId.h"
#include "DataFormats/MuonDetId/interface/MuonSubdetId.h"
#include "DataFormats/MuonDetId/interface/DTChamberId.h"
#include "DataFormats/MuonReco/interface/MuonChamberMatch.h"
#include "DataFormats/MuonReco/interface/MuonSegmentMatch.h"
#include "DataFormats/MuonReco/interface/MuonSelectors.h"

#include "DataFormats/GEMRecHit/interface/GEMRecHitCollection.h"
#include "DataFormats/GEMRecHit/interface/GEMSegment.h"
#include "DataFormats/GEMRecHit/interface/GEMSegmentCollection.h"
#include "Geometry/GEMGeometry/interface/GEMEtaPartitionSpecs.h"

#include "DataFormats/TrackingRecHit/interface/TrackingRecHitFwd.h"
#include "DataFormats/CSCRecHit/interface/CSCSegment.h"
#include "DataFormats/CSCRecHit/interface/CSCSegmentCollection.h"
#include "DataFormats/CSCRecHit/interface/CSCRecHit2D.h"
#include <DataFormats/CSCDigi/interface/CSCCorrelatedLCTDigiCollection.h>
#include <DataFormats/CSCDigi/interface/CSCCorrelatedLCTDigi.h>

#include "DataFormats/MuonDetId/interface/CSCDetId.h"
#include "DataFormats/MuonDetId/interface/GEMDetId.h"

#include "DataFormats/GeometryCommonDetAlgo/interface/ErrorFrameTransformer.h"
#include "TrackPropagation/SteppingHelixPropagator/interface/SteppingHelixStateInfo.h"
#include "MagneticField/Engine/interface/MagneticField.h"

#include "TrackingTools/GeomPropagators/interface/Propagator.h"
#include "TrackingTools/TransientTrack/interface/TransientTrackBuilder.h"
#include "TrackingTools/Records/interface/TransientTrackRecord.h"
#include "TrackPropagation/SteppingHelixPropagator/interface/SteppingHelixPropagator.h"
#include "RecoMuon/TrackingTools/interface/MuonServiceProxy.h"

#include "Geometry/Records/interface/MuonGeometryRecord.h"
#include "Geometry/CommonTopologies/interface/StripTopology.h"

#include "DataFormats/Math/interface/deltaR.h"
#include "HLTrigger/HLTcore/interface/HLTConfigProvider.h"
#include "DataFormats/Common/interface/TriggerResults.h"
#include "DataFormats/HLTReco/interface/TriggerEvent.h"
#include "DataFormats/HLTReco/interface/TriggerObject.h"

#include <iostream>
#include <fstream>
#include <thread>
#include <chrono>

#include "TVectorF.h"
#include "TFile.h"

MuNtupleGEMMuonFiller::MuNtupleGEMMuonFiller(edm::ConsumesCollector && collector,
					     const std::shared_ptr<MuNtupleConfig> config, 
					     std::shared_ptr<TTree> tree, const std::string & label) : 
MuNtupleBaseFiller(config, tree, label), m_nullVecF()
{

  edm::InputTag & muonTag = m_config->m_inputTags["muonTag"];
  if (muonTag.label() != "none") m_muToken = collector.consumes<reco::MuonCollection>(muonTag);

  edm::InputTag & primaryVerticesTag = m_config->m_inputTags["primaryVerticesTag"];
  if (primaryVerticesTag.label() != "none") m_primaryVerticesToken = collector.consumes<std::vector<reco::Vertex>>(primaryVerticesTag);

  edm::InputTag & gemSegmentTag = m_config->m_inputTags["gemSegmentTag"];
  if (gemSegmentTag.label() != "none") m_gemSegmentToken = collector.consumes<GEMSegmentCollection>(gemSegmentTag);
  
  edm::InputTag & cscSegmentTag = m_config->m_inputTags["cscSegmentTag"];
  if(cscSegmentTag.label() != "none") m_cscSegmentToken = collector.consumes<CSCSegmentCollection>(cscSegmentTag);

  edm::InputTag & trigResultsTag = m_config->m_inputTags["trigResultsTag"];
  if (trigResultsTag.label() != "none") m_trigResultsToken = collector.consumes<edm::TriggerResults>(trigResultsTag);

  edm::InputTag & trigEventTag = m_config->m_inputTags["trigEventTag"];
  if (trigEventTag.label() != "none") m_trigEventToken = collector.consumes<trigger::TriggerEvent>(trigEventTag);

  edm::InputTag & gemRecHitTag = m_config->m_inputTags["gemRecHitTag"];
  if (gemRecHitTag.label() != "none") m_gemRecHitToken = collector.consumes<GEMRecHitCollection>(gemRecHitTag);
  
}

MuNtupleGEMMuonFiller::~MuNtupleGEMMuonFiller() 
{ 

}

void MuNtupleGEMMuonFiller::initialize()
{

  m_tree->Branch((m_label + "_nMuons").c_str(), &m_nMuons);
  
  m_tree->Branch((m_label + "_pt").c_str(), &m_pt);
  m_tree->Branch((m_label + "_phi").c_str(), &m_phi);
  m_tree->Branch((m_label + "_eta").c_str(), &m_eta);
  m_tree->Branch((m_label + "_charge").c_str(), &m_charge);

  m_tree->Branch((m_label + "_isGlobal").c_str(), &m_isGlobal);
  m_tree->Branch((m_label + "_isStandalone").c_str(), &m_isStandalone);
  m_tree->Branch((m_label + "_isTracker").c_str(), &m_isTracker);
  m_tree->Branch((m_label + "_isGEM").c_str(), &m_isGEM);
  m_tree->Branch((m_label + "_isCSC").c_str(), &m_isCSC);
  m_tree->Branch((m_label + "_isME11").c_str(), &m_isME11);
  
  m_tree->Branch((m_label + "_isLoose").c_str(), &m_isLoose);
  m_tree->Branch((m_label + "_isMedium").c_str(), &m_isMedium);
  m_tree->Branch((m_label + "_isTight").c_str(), &m_isTight);

  m_tree->Branch((m_label + "_propagated_isME11").c_str(), &m_propagated_isME11);

  m_tree->Branch((m_label + "_propagated_TrackNormChi2").c_str(), &m_propagated_TrackNormChi2);

  m_tree->Branch((m_label + "_propagated_numberOfValidPixelHits").c_str(), &m_propagated_numberOfValidPixelHits);
  m_tree->Branch((m_label + "_propagated_innerTracker_ValidFraction").c_str(), &m_propagated_innerTracker_ValidFraction);
  m_tree->Branch((m_label + "_propagated_numberOfValidTrackerHits").c_str(), &m_propagated_numberOfValidTrackerHits);
  
  m_tree->Branch((m_label + "_propagated_path_length").c_str(), &m_propagated_path_length);

  m_tree->Branch((m_label + "_propagated_isinsideout").c_str(), &m_propagated_isinsideout);
  m_tree->Branch((m_label + "_propagated_isincoming").c_str(), &m_propagated_isincoming);

  m_tree->Branch((m_label + "_propagated_region").c_str(), &m_propagated_region);
  m_tree->Branch((m_label + "_propagated_layer").c_str(), &m_propagated_layer);
  m_tree->Branch((m_label + "_propagated_chamber").c_str(), &m_propagated_chamber);
  m_tree->Branch((m_label + "_propagated_etaP").c_str(), &m_propagated_etaP);

  m_tree->Branch((m_label + "_propagated_pt").c_str(), &m_propagated_pt);
  m_tree->Branch((m_label + "_propagated_phi").c_str(), &m_propagated_phi);
  m_tree->Branch((m_label + "_propagated_eta").c_str(), &m_propagated_eta);
  m_tree->Branch((m_label + "_propagated_charge").c_str(), &m_propagated_charge);

  m_tree->Branch((m_label + "_propagatedSeg_pt").c_str(), &m_propagatedSeg_pt);
  m_tree->Branch((m_label + "_propagatedSeg_phi").c_str(), &m_propagatedSeg_phi);
  m_tree->Branch((m_label + "_propagatedSeg_eta").c_str(), &m_propagatedSeg_eta);
  m_tree->Branch((m_label + "_propagatedSeg_charge").c_str(), &m_propagatedSeg_charge);

  m_tree->Branch((m_label + "_propagatedLoc_x").c_str(), &m_propagatedLoc_x);
  m_tree->Branch((m_label + "_propagatedLoc_y").c_str(), &m_propagatedLoc_y);
  m_tree->Branch((m_label + "_propagatedLoc_z").c_str(), &m_propagatedLoc_z);
  m_tree->Branch((m_label + "_propagatedLoc_r").c_str(), &m_propagatedLoc_r);
  m_tree->Branch((m_label + "_propagated_isGEM").c_str(), &m_propagated_isGEM);
  m_tree->Branch((m_label + "_propagatedLoc_phi").c_str(), &m_propagatedLoc_phi);
  m_tree->Branch((m_label + "_propagatedLoc_errX").c_str(), &m_propagatedLoc_errX);
  m_tree->Branch((m_label + "_propagatedLoc_errY").c_str(), &m_propagatedLoc_errY);
  m_tree->Branch((m_label + "_propagatedLoc_dirX").c_str(), &m_propagatedLoc_dirX);
  m_tree->Branch((m_label + "_propagatedLoc_dirY").c_str(), &m_propagatedLoc_dirY);
  m_tree->Branch((m_label + "_propagatedLoc_dirZ").c_str(), &m_propagatedLoc_dirZ);

  m_tree->Branch((m_label + "_propagatedSegLoc_x").c_str(), &m_propagatedSegLoc_x);
  m_tree->Branch((m_label + "_propagatedSegLoc_y").c_str(), &m_propagatedSegLoc_y);
  m_tree->Branch((m_label + "_propagatedSegLoc_z").c_str(), &m_propagatedSegLoc_z);
  m_tree->Branch((m_label + "_propagatedSegLoc_r").c_str(), &m_propagatedSegLoc_r);
  m_tree->Branch((m_label + "_propagatedSegLoc_phi").c_str(), &m_propagatedSegLoc_phi);
  m_tree->Branch((m_label + "_propagatedSegLoc_errX").c_str(), &m_propagatedSegLoc_errX);
  m_tree->Branch((m_label + "_propagatedSegLoc_errY").c_str(), &m_propagatedSegLoc_errY);
  m_tree->Branch((m_label + "_propagatedSegLoc_dirX").c_str(), &m_propagatedSegLoc_dirX);
  m_tree->Branch((m_label + "_propagatedSegLoc_dirY").c_str(), &m_propagatedSegLoc_dirY);
  m_tree->Branch((m_label + "_propagatedSegLoc_dirZ").c_str(), &m_propagatedSegLoc_dirZ);


  m_tree->Branch((m_label + "_propagatedGlb_x").c_str(), &m_propagatedGlb_x);
  m_tree->Branch((m_label + "_propagatedGlb_y").c_str(), &m_propagatedGlb_y);
  m_tree->Branch((m_label + "_propagatedGlb_z").c_str(), &m_propagatedGlb_z);
  m_tree->Branch((m_label + "_propagatedGlb_r").c_str(), &m_propagatedGlb_r);
  m_tree->Branch((m_label + "_propagatedGlb_phi").c_str(), &m_propagatedGlb_phi);
  m_tree->Branch((m_label + "_propagatedGlb_errX").c_str(), &m_propagatedGlb_errX);
  m_tree->Branch((m_label + "_propagatedGlb_errY").c_str(), &m_propagatedGlb_errY);
  m_tree->Branch((m_label + "_propagatedGlb_errR").c_str(), &m_propagatedGlb_rerr);
  m_tree->Branch((m_label + "_propagatedGlb_errPhi").c_str(), &m_propagatedGlb_phierr);

  m_tree->Branch((m_label + "_propagatedSegGlb_x").c_str(), &m_propagatedSegGlb_x);
  m_tree->Branch((m_label + "_propagatedSegGlb_y").c_str(), &m_propagatedSegGlb_y);
  m_tree->Branch((m_label + "_propagatedSegGlb_z").c_str(), &m_propagatedSegGlb_z);
  m_tree->Branch((m_label + "_propagatedSegGlb_r").c_str(), &m_propagatedSegGlb_r);
  m_tree->Branch((m_label + "_propagatedSegGlb_phi").c_str(), &m_propagatedSegGlb_phi);
  m_tree->Branch((m_label + "_propagatedSegGlb_errX").c_str(), &m_propagatedSegGlb_errX);
  m_tree->Branch((m_label + "_propagatedSegGlb_errY").c_str(), &m_propagatedSegGlb_errY);
  m_tree->Branch((m_label + "_propagatedSegGlb_errR").c_str(), &m_propagatedSegGlb_rerr);
  m_tree->Branch((m_label + "_propagatedSegGlb_errPhi").c_str(), &m_propagatedSegGlb_phierr);

  m_tree->Branch((m_label + "_propagated_EtaPartition_centerX").c_str(), &m_propagated_EtaPartition_centerX);
  m_tree->Branch((m_label + "_propagated_EtaPartition_centerY").c_str(), &m_propagated_EtaPartition_centerY);
  m_tree->Branch((m_label + "_propagated_EtaPartition_rMax").c_str(), &m_propagated_EtaPartition_rMax);
  m_tree->Branch((m_label + "_propagated_EtaPartition_rMin").c_str(), &m_propagated_EtaPartition_rMin);
  m_tree->Branch((m_label + "_propagated_EtaPartition_phiMax").c_str(), &m_propagated_EtaPartition_phiMax);
  m_tree->Branch((m_label + "_propagated_EtaPartition_phiMin").c_str(), &m_propagated_EtaPartition_phiMin);

  m_tree->Branch((m_label + "_propagated_nME1hits").c_str(), &m_propagated_nME1hits);
  m_tree->Branch((m_label + "_propagated_nME2hits").c_str(), &m_propagated_nME2hits);
  m_tree->Branch((m_label + "_propagated_nME3hits").c_str(), &m_propagated_nME3hits);
  m_tree->Branch((m_label + "_propagated_nME4hits").c_str(), &m_propagated_nME4hits);


  m_tree->Branch((m_label + "_propagated_Innermost_x").c_str(), &m_propagated_Innermost_x);
  m_tree->Branch((m_label + "_propagated_Innermost_y").c_str(), &m_propagated_Innermost_y);
  m_tree->Branch((m_label + "_propagated_Innermost_z").c_str(), &m_propagated_Innermost_z);
  m_tree->Branch((m_label + "_propagated_Outermost_x").c_str(), &m_propagated_Outermost_x);
  m_tree->Branch((m_label + "_propagated_Outermost_y").c_str(), &m_propagated_Outermost_y);
  m_tree->Branch((m_label + "_propagated_Outermost_z").c_str(), &m_propagated_Outermost_z);


}

void MuNtupleGEMMuonFiller::clear()
{

  m_nMuons = 0;

  m_pt.clear();
  m_phi.clear();
  m_eta.clear();
  m_charge.clear();

  m_isGlobal.clear();
  m_isStandalone.clear();
  m_isTracker.clear();
  m_isGEM.clear();
  m_isCSC.clear();
  m_isME11.clear();

  m_propagated_TrackNormChi2.clear();

  m_propagated_numberOfValidPixelHits.clear();
  m_propagated_innerTracker_ValidFraction.clear();
  m_propagated_numberOfValidTrackerHits.clear();

  m_isLoose.clear();
  m_isMedium.clear();
  m_isTight.clear();

  m_propagated_path_length = 0;
  
  m_propagated_isinsideout.clear();
  m_propagated_isincoming.clear();

  m_propagated_region.clear();
  m_propagated_layer.clear();
  m_propagated_chamber.clear();
  m_propagated_etaP.clear();
  m_propagated_isME11.clear();
  
  m_propagated_pt.clear();
  m_propagated_phi.clear();
  m_propagated_eta.clear();
  m_propagated_charge.clear();

  m_propagatedSeg_pt.clear();
  m_propagatedSeg_phi.clear();
  m_propagatedSeg_eta.clear();
  m_propagatedSeg_charge.clear();

  m_propagatedLoc_x.clear();
  m_propagatedLoc_y.clear();
  m_propagatedLoc_z.clear();
  m_propagatedLoc_r.clear();
  m_propagated_isGEM.clear();
  m_propagatedLoc_phi.clear();
  m_propagatedLoc_errX.clear();
  m_propagatedLoc_errY.clear();
  m_propagatedLoc_dirX.clear();
  m_propagatedLoc_dirY.clear();
  m_propagatedLoc_dirZ.clear();

  m_propagatedSegLoc_x.clear();
  m_propagatedSegLoc_y.clear();
  m_propagatedSegLoc_z.clear();
  m_propagatedSegLoc_r.clear();
  m_propagatedSegLoc_phi.clear();
  m_propagatedSegLoc_errX.clear();
  m_propagatedSegLoc_errY.clear();
  m_propagatedSegLoc_dirX.clear();
  m_propagatedSegLoc_dirY.clear();
  m_propagatedSegLoc_dirZ.clear();

  m_propagatedGlb_errX.clear();
  m_propagatedGlb_errY.clear();
  m_propagatedGlb_rerr.clear();
  m_propagatedGlb_phierr.clear();
  m_propagatedGlb_x.clear();
  m_propagatedGlb_y.clear();
  m_propagatedGlb_z.clear();
  m_propagatedGlb_r.clear();
  m_propagatedGlb_phi.clear();

  m_propagatedSegGlb_errX.clear();
  m_propagatedSegGlb_errY.clear();
  m_propagatedSegGlb_rerr.clear();
  m_propagatedSegGlb_phierr.clear();
  m_propagatedSegGlb_x.clear();
  m_propagatedSegGlb_y.clear();
  m_propagatedSegGlb_z.clear();
  m_propagatedSegGlb_r.clear();
  m_propagatedSegGlb_phi.clear();

  m_propagated_nME1hits.clear();
  m_propagated_nME2hits.clear();
  m_propagated_nME3hits.clear();
  m_propagated_nME4hits.clear();

  m_propagated_Innermost_x.clear();
  m_propagated_Innermost_y.clear();
  m_propagated_Innermost_z.clear();

  m_propagated_Outermost_x.clear();
  m_propagated_Outermost_y.clear();
  m_propagated_Outermost_z.clear();

  m_propagated_EtaPartition_centerX.clear();
  m_propagated_EtaPartition_centerY.clear();
  m_propagated_EtaPartition_rMax.clear();
  m_propagated_EtaPartition_rMin.clear();
  m_propagated_EtaPartition_phiMax.clear();
  m_propagated_EtaPartition_phiMin.clear();
      
}

void MuNtupleGEMMuonFiller::fill(const edm::Event & ev)
{

  clear();

  auto muons = conditionalGet<reco::MuonCollection>(ev, m_muToken, "MuonCollection");
  auto gem_segments = conditionalGet<GEMSegmentCollection>(ev,m_gemSegmentToken, "GEMSegmentCollection");
  auto csc_segments = conditionalGet<CSCSegmentCollection>(ev,m_cscSegmentToken, "CSCSegmentCollection" );
  auto vtxs = conditionalGet<std::vector<reco::Vertex>>(ev, m_primaryVerticesToken, "std::vector<reco::Vertex>");

  bool isCSC = false;
  bool isME11 = false;

  edm::Handle<GEMRecHitCollection> rechit_collection;
  ev.getByToken(m_gemRecHitToken, rechit_collection);
  if (not rechit_collection.isValid()) {
    std::cout << "GEMRecHitCollection is invalid" << std::endl;
    return;
  }
 
  edm::ESHandle<Propagator>&& propagator_any = m_config->m_muonSP->propagator("SteppingHelixPropagatorAny");
  if (not propagator_any.isValid()) {
    std::cout<< "Any Propagator is invalid" << std::endl;
    return;
  }
  
  edm::ESHandle<Propagator>&& propagator_along = m_config->m_muonSP->propagator("SteppingHelixPropagatorAlong");
  if (not propagator_along.isValid()) {
    std::cout<< "Along Propagator is invalid" << std::endl;
    return;
  }

  edm::ESHandle<Propagator>&& propagator_opposite = m_config->m_muonSP->propagator("SteppingHelixPropagatorOpposite");
  if (not propagator_opposite.isValid()) {
    std::cout<< "Opposite Propagator is invalid" << std::endl;
    return;
  }

    
  edm::ESHandle<GEMGeometry> gem = m_config->m_gemGeometry;
  if (not gem.isValid()) {
    std::cout << "GEMGeometry is invalid" << std::endl;
    return;
  }

  edm::ESHandle<CSCGeometry> csc = m_config->m_cscGeometry;
  if (not csc.isValid()) {
    std::cout << "CSCGeometry is invalid" << std::endl;
    return;
  }
    
  edm::ESHandle<TransientTrackBuilder> transient_track_builder = m_config->m_transientTrackBuilder;
  if (not transient_track_builder.isValid()) {
    std::cout << "TransientTrack is invalid" << std::endl;
    return;
  }

  edm::ESHandle<GlobalTrackingGeometry> tracking_geometry = m_config->m_trackingGeometry;
  if (not tracking_geometry.isValid()) {
    std::cout << "GlobalTrackingGeometry is invalid" << std::endl;
    return;
  }

  if (muons.isValid()) // && csc_segments.isValid() && vtxs.isValid()) 
    {
    //loop on recoMuons  
    for (const auto & muon : (*muons)) {

      m_pt.push_back(muon.pt());
      m_eta.push_back(muon.eta());
      m_phi.push_back(muon.phi());
      m_charge.push_back(muon.charge());
      
      m_isGlobal.push_back(muon.isGlobalMuon());
      m_isStandalone.push_back(muon.isStandAloneMuon());
      m_isTracker.push_back(muon.isTrackerMuon());
      m_isGEM.push_back(muon.isGEMMuon());
      
      m_isLoose.push_back(muon.passed(reco::Muon::CutBasedIdLoose));
      m_isMedium.push_back(muon.passed(reco::Muon::CutBasedIdMedium));
      m_isTight.push_back(muon.passed(reco::Muon::CutBasedIdTight));
      
      m_nMuons++;
      
      isCSC = false;
      isME11 = false;

      if(!muon.outerTrack().isNull())   //STA muon
      //if(!muon.globalTrack().isNull())   //GLB muon
      {
        //const reco::Track* track = muon.globalTrack().get();   //GLB muon
        const reco::Track* track = muon.outerTrack().get();   //STA muon
        const CSCSegment *ME11_segment;
        
        if (track == nullptr) {
          std::cout << "failed to get muon track" << std::endl;
          continue;
        }
        
        const reco::TrackRef outerTrackRef = muon.outerTrack();   //STA muon
        //const reco::TrackRef trackRef = muon.globalTrack();     //GLB muon
        
        float p2_in = track->innerMomentum().mag2();
        float p2_out = track->outerMomentum().mag2();
        float pos_out = track->outerPosition().mag2();
        float pos_in = track->innerPosition().mag2();
        
        bool is_insideout = pos_in > pos_out;
        
        if(is_insideout)
        {
          std::swap(pos_in, pos_out);
          std::swap(p2_in, p2_out);
        }
        
        bool is_incoming = p2_out > p2_in;
        
        const reco::TransientTrack&& transient_track = transient_track_builder->build(track);
        if (not transient_track.isValid()) 
        {
          std::cout<<"failed  to build TransientTrack" << std::endl;
          continue;
        }
        
        const auto&& start_state = is_insideout ? transient_track.outermostMeasurementState() : transient_track.innermostMeasurementState();
        //auto& propagator = is_incoming ? propagator_along : propagator_opposite;
        
        auto recHitMu = outerTrackRef->recHitsBegin();     //STA muon
        auto recHitMuEnd = outerTrackRef->recHitsEnd();    //STA muon
        
        //auto recHitMu = trackRef->recHitsBegin();        //GLB muon
        //auto recHitMuEnd = trackRef->recHitsEnd();       //GLB muon
        
        const reco::HitPattern& htp = transient_track.hitPattern();
        //Loop over recHits
        for(; recHitMu != recHitMuEnd; ++recHitMu)
        {
          DetId detId = (*recHitMu)->geographicalId();
          if(detId.det() == DetId::Muon && detId.subdetId() == MuonSubdetId::GEM)
            std::cout<<"GEM found in STA track"<<std::endl;
          if(detId.det() == DetId::Muon && detId.subdetId() == MuonSubdetId::CSC)
          {
            isCSC = true;
            const CSCDetId csc_id{detId};
            // ME11 chambers are composed by 2 subchambers: ME11a, ME11b. In CMSSW they are referred as Stat. 1 Ring 1, Stat. 1 Ring. 4 respectively
            if(csc_id.station() == 1 && ((csc_id.ring() == 1) || (csc_id.ring() == 4)) )
            { 
              isME11 = true;
              //extracting ME11 segment
              RecSegment* Rec_segment = (RecSegment*)*recHitMu;
              ME11_segment = (CSCSegment*)*recHitMu;
              std::cout<<" extracted ME11 segment ignored "<<Rec_segment<<std::endl;
            }
          }
        } //END Loop over recHits to find if is ME11
        //Alternative way to take ME11 segment
        auto matches = muon.matches();
        for (auto MCM : matches){
          if(MCM.detector() != 2) continue;
          for(auto MSM : MCM.segmentMatches){
            auto cscSegRef = MSM.cscSegmentRef;
            auto cscDetID = cscSegRef->cscDetId();
            if(cscDetID.station() == 1 and (cscDetID.ring() == 1 or cscDetID.ring() == 4)){
              //isME11 = true;
              //ME11_segment = cscSegRef.get();
              std::cout<<" ME11 segment here too "<<ME11_segment<<std::endl;
            }
          }
        }
        m_isME11.push_back(isME11);
        //BEGIN propagation code (STA TRACK)
        //if at least one CSC hit is found, perform propagation 
        if(isCSC)
        {
          //std::cout<<"Track HITS"<<std::endl;
          //htp.print(reco::HitPattern::TRACK_HITS);
          
          // CSC Hits
          int nME1_hits = 0;
          int nME2_hits = 0;
          int nME3_hits = 0;
          int nME4_hits = 0;
          
          for (int i = 0; i < htp.numberOfAllHits(htp.TRACK_HITS); i++)
          {
            uint32_t hit = htp.getHitPattern(htp.TRACK_HITS, i);
            int substructure = htp.getSubStructure(hit);
            int hittype = htp.getHitType(hit);
            
            if ( substructure == 2 && hittype == 0) // CSC Hits
            {
              int CSC_station = htp.getMuonStation(hit);
              
              switch(CSC_station) {
                case 1 :
                    nME1_hits++;
                    break;
                case 2 :
                    nME2_hits++;
                    break;
                case 3 :
                    nME3_hits++;
                    break;
                case 4 :
                    nME4_hits++;
                    break;
                default: 
                    std::cout<<"Invalid station " <<std::endl;
              }
            }
          }
          
          for (const GEMRegion* gem_region : gem->regions())
          {
            bool is_opposite_region = muon.eta() * gem_region->region() < 0;
            if (is_incoming xor is_opposite_region) continue;
            
            for (const GEMStation* station : gem_region->stations())
            {
              for (const GEMSuperChamber* super_chamber : station->superChambers())
              {
                for (const GEMChamber* chamber : super_chamber->chambers())
                {
                  for (const GEMEtaPartition* eta_partition : chamber->etaPartitions())
                  {
                    const BoundPlane& bound_plane = eta_partition->surface();
                    
                    // PROPAGATION ON ETAP SURFACE
                    // The Z of the dest_state is fixed one the boundplane. x,y are actually evaluated by the propagator at that Z
                    const auto& dest_state = propagator_any->propagate(start_state,bound_plane);
                    //END PROPAGATION ON ETAP SURFACE
                    
                    // // PROPAGATION IN THE DRIFT GAP
                    // BoundPlane& etaPSur_translated_to_drift = const_cast<BoundPlane&>(bound_plane);
                    
                    // int ch = eta_partition->id().chamber();
                    // int re = eta_partition->id().region();
                    // double displacement = 0;
                    
                    // if (ch % 2 == 0)
                    //     {
                    //         displacement = -0.55*re;
                    //     }
                    // if (ch % 2 == 1)
                    //     {
                    //         displacement = 0.55*re;
                    //     }
                    
                    // etaPSur_translated_to_drift.move(GlobalVector(0.,0.,displacement));
                    // const auto& dest_state = propagator_any->propagate(start_state,etaPSur_translated_to_drift);
                    // etaPSur_translated_to_drift.move(GlobalVector(0.,0.,-displacement));
                    // // END PROPAGATION IN THE DRIFT GAP
                    
                    if (not dest_state.isValid())
                    {
                      std::cout << "failed to propagate" << std::endl;
                      continue;
                    }
                    
                    const GlobalPoint&& dest_global_pos = dest_state.globalPosition();
                    const LocalPoint&& local_point = eta_partition->toLocal(dest_global_pos);
                    const LocalPoint local_point_2d(local_point.x(), local_point.y(), 0.0f);
                    
                    if (eta_partition->surface().bounds().inside(local_point_2d)) 
                    {
                      
                      const GEMDetId&& gem_id = eta_partition->id();
                      
                      //// PROPAGATED HIT ERROR EVALUATION
                      // X,Y FROM QC8 Code
                      double xx = dest_state.curvilinearError().matrix()(3,3);
                      double yy = dest_state.curvilinearError().matrix()(4,4);
                      double xy = dest_state.curvilinearError().matrix()(4,3);
                      double dest_glob_error_x = sqrt(0.5*(xx+yy-sqrt((xx-yy)*(xx-yy)+4*xy*xy)));
                      double dest_glob_error_y = sqrt(0.5*(xx+yy+sqrt((xx-yy)*(xx-yy)+4*xy*xy)));
                      // R,Phi From https://github.com/cms-sw/cmssw/blob/f77e926a1e98b3d9f1144caf3b83cb3667e23786/DQMOffline/Muon/src/GEMEfficiencyAnalyzer.cc
                      const LocalPoint&& dest_local_pos = chamber->toLocal(dest_global_pos);
                      const LocalError&& dest_local_err = dest_state.localError().positionError();
                      const GlobalError& dest_global_err = ErrorFrameTransformer().transform(dest_local_err, eta_partition->surface());
                      const double dest_global_r_err = std::sqrt(dest_global_err.rerr(dest_global_pos));
                      const double dest_global_phi_err = std::sqrt(dest_global_err.phierr(dest_global_pos));
                      
                      m_propagated_isME11.push_back(isME11);
                      
                      m_propagated_nME1hits.push_back(nME1_hits);
                      m_propagated_nME2hits.push_back(nME2_hits);
                      m_propagated_nME3hits.push_back(nME3_hits);
                      m_propagated_nME4hits.push_back(nME4_hits);
                      
                      m_propagated_Innermost_x.push_back(transient_track.innermostMeasurementState().globalPosition().x());
                      m_propagated_Innermost_y.push_back(transient_track.innermostMeasurementState().globalPosition().y());
                      m_propagated_Innermost_z.push_back(transient_track.innermostMeasurementState().globalPosition().z());
                      m_propagated_Outermost_x.push_back(transient_track.outermostMeasurementState().globalPosition().x());
                      m_propagated_Outermost_y.push_back(transient_track.outermostMeasurementState().globalPosition().y());
                      m_propagated_Outermost_z.push_back(transient_track.outermostMeasurementState().globalPosition().z());
                      
                      m_propagated_EtaPartition_centerX.push_back(eta_partition->position().x());
                      m_propagated_EtaPartition_centerY.push_back(eta_partition->position().y());
                      m_propagated_EtaPartition_rMin.push_back(eta_partition->surface().rSpan().first);
                      m_propagated_EtaPartition_rMax.push_back(eta_partition->surface().rSpan().second);
                      m_propagated_EtaPartition_phiMin.push_back(eta_partition->surface().phiSpan().first);
                      m_propagated_EtaPartition_phiMax.push_back(eta_partition->surface().phiSpan().second);
                      
                      m_propagatedGlb_x.push_back(dest_global_pos.x());
                      m_propagatedGlb_y.push_back(dest_global_pos.y());
                      m_propagatedGlb_z.push_back(dest_global_pos.z());
                      m_propagatedGlb_r.push_back(dest_global_pos.perp());
                      m_propagatedGlb_phi.push_back(dest_global_pos.phi());
                      
                      std::cout<<"STA propa"<<std::endl;  
                      std::cout<<"recoMu pt-eta: "<<muon.pt()<< " "<<muon.eta()<<std::endl;  
                      std::cout<<"x: "<<dest_global_pos.x()<<" y: "<<dest_global_pos.y()<<" z: "<<dest_global_pos.z()<<std::endl;  

                      m_propagated_pt.push_back(muon.pt());
                      m_propagated_phi.push_back(muon.phi());
                      m_propagated_eta.push_back(muon.eta());
                      m_propagated_charge.push_back(muon.charge());
                      m_propagated_TrackNormChi2.push_back(transient_track.normalizedChi2());
                      
                      m_propagatedLoc_x.push_back(dest_local_pos.x());
                      m_propagatedLoc_phi.push_back(dest_local_pos.phi());
                      m_propagatedLoc_r.push_back(dest_local_pos.perp());
                      m_propagatedLoc_y.push_back(dest_local_pos.y());
                      m_propagatedLoc_dirX.push_back(dest_state.localDirection().x());
                      m_propagatedLoc_dirY.push_back(dest_state.localDirection().y());
                      m_propagatedLoc_dirZ.push_back(dest_state.localDirection().z());
                      
                      m_propagatedLoc_errX.push_back(dest_local_err.xx());
                      m_propagatedLoc_errY.push_back(dest_local_err.yy());
                      m_propagated_isGEM.push_back(muon.isGEMMuon());
                      
                      m_propagatedGlb_errX.push_back(dest_glob_error_x);
                      m_propagatedGlb_errY.push_back(dest_glob_error_y);
                      m_propagatedGlb_rerr.push_back(dest_global_r_err);
                      m_propagatedGlb_phierr.push_back(dest_global_phi_err);
                      
                      m_propagated_region.push_back(gem_id.region());
                      m_propagated_layer.push_back(gem_id.layer());
                      m_propagated_chamber.push_back(gem_id.chamber());
                      m_propagated_etaP.push_back(gem_id.roll());
                      
                      m_propagated_isinsideout.push_back(is_insideout);
                      m_propagated_isincoming.push_back(is_incoming);
                      
                      if(!muon.innerTrack().isNull())
                      {
                        m_propagated_numberOfValidPixelHits.push_back(muon.innerTrack()->hitPattern().numberOfValidPixelHits());
                        m_propagated_innerTracker_ValidFraction.push_back(muon.innerTrack()->validFraction());
                        m_propagated_numberOfValidTrackerHits.push_back(muon.innerTrack()->hitPattern().numberOfValidTrackerHits());
                      }else{
                        m_propagated_numberOfValidPixelHits.push_back(-999);
                        m_propagated_innerTracker_ValidFraction.push_back(-999);
                        m_propagated_numberOfValidTrackerHits.push_back(-999);
                      }
                    }// Propagation is inside EtaPartition
                  }//Loop over Eta Partition
                }//Loop over chambers
              }//Loop over SuperChambers
            }//Loop over stations
          }//Loop over regions
          //END propagation code (STA TRACK)
          //
          //BEGIN propagation code (CSC Segment)
          if(isME11){ 
            std::cout << "IT IS ME11!" << std::endl;
            DetId segDetId = ME11_segment->geographicalId();
            const GeomDet* segDet = tracking_geometry->idToDet(segDetId);
  
            //N.B. no momentum for segments -> estimated from innerTrack, otherwise use segment direction
            const reco::Track* recoTrack = muon.track().get();
            LocalVector momentum_at_surface = ME11_segment->localDirection();
            if (recoTrack != nullptr) momentum_at_surface = momentum_at_surface*(recoTrack->outerP());
  
            LocalTrajectoryParameters param(ME11_segment->localPosition(), momentum_at_surface, muon.charge());
            AlgebraicSymMatrix mat(5,0);

            std::cout << "ABOUT TO PRINT STUFF!" << std::endl;
            std::cout << "ME11 SEGMENT: " << ME11_segment << std::endl;
            std::cout << "PROJECTION MATRIX: " << ME11_segment->projectionMatrix() << std::endl;
            //std::cout << "LOCAL POSITION ERROR: " << ME11_segment->localPositionError() << std::endl;
            std::cout << "ERRORS: " << ME11_segment->parametersError() << std::endl;
            std::cout << "DONE PRINTING ERRORS." << std::endl;
            mat = ME11_segment->parametersError().similarityT( ME11_segment->projectionMatrix() );
            std::cout << "PRINTING MATRIX " << mat << std::endl;
            LocalTrajectoryError error(asSMatrix<5>(mat));
      
            for (const GEMRegion* gem_region : gem->regions())
            {
              bool is_opposite_region = muon.eta() * gem_region->region() < 0;
              if (is_incoming xor is_opposite_region) continue;
              
              for (const GEMStation* station : gem_region->stations())
              {
                for (const GEMSuperChamber* super_chamber : station->superChambers())
                {
                  for (const GEMChamber* chamber : super_chamber->chambers())
                  {
                    for (const GEMEtaPartition* eta_partition : chamber->etaPartitions())
                    {
                      const BoundPlane& bound_plane = eta_partition->surface();
                      //TSOS at ME1/1 surface
                      TrajectoryStateOnSurface start_state_seg(param, error, segDet->surface(), &*(m_config->m_muonSP)->magneticField());
                      //TSOS at GE1/1 surface
                      TrajectoryStateOnSurface dest_state_seg = propagator_any->propagate(start_state_seg, eta_partition->surface());
  
                      if ( not dest_state_seg.isValid()){
                        std::cout << "failed to propagate" << std::endl;
                        continue;
                      }
                      if ((dest_state_seg.globalPosition().z() * dest_state_seg.globalPosition().z() < 0) or !(eta_partition->id().station() == 1 and eta_partition->id().ring() == 1)){
                        std::cout << "not matching region to propagate" << std::endl;
                        continue;
                      }
                      const GlobalPoint&& dest_global_pos_seg = dest_state_seg.globalPosition();
                      const GlobalPoint&& start_global_pos_seg = start_state_seg.globalPosition();
  
                      const LocalPoint&& local_point_seg = eta_partition->toLocal(dest_state_seg.globalPosition());
                      const LocalPoint local_point_2d_seg(local_point_seg.x(), local_point_seg.y(), 0);
  
                      if (eta_partition->surface().bounds().inside(local_point_2d_seg)) 
                      {
                          
                          const GEMDetId&& gem_id = eta_partition->id();
                          
                          //// PROPAGATED HIT ERROR EVALUATION
                          // X,Y FROM QC8 Code
                          double xx = dest_state_seg.curvilinearError().matrix()(3,3);
                          double yy = dest_state_seg.curvilinearError().matrix()(4,4);
                          double xy = dest_state_seg.curvilinearError().matrix()(4,3);
                          double dest_glob_error_x = sqrt(0.5*(xx+yy-sqrt((xx-yy)*(xx-yy)+4*xy*xy)));
                          double dest_glob_error_y = sqrt(0.5*(xx+yy+sqrt((xx-yy)*(xx-yy)+4*xy*xy)));
                          // R,Phi From https://github.com/cms-sw/cmssw/blob/f77e926a1e98b3d9f1144caf3b83cb3667e23786/DQMOffline/Muon/src/GEMEfficiencyAnalyzer.cc
                          const LocalPoint&& dest_local_pos = chamber->toLocal(dest_global_pos_seg);
                          const LocalError&& dest_local_err = dest_state_seg.localError().positionError();
                          const GlobalError& dest_global_err = ErrorFrameTransformer().transform(dest_local_err, eta_partition->surface());
                          const double dest_global_r_err = std::sqrt(dest_global_err.rerr(dest_global_pos_seg));
                          const double dest_global_phi_err = std::sqrt(dest_global_err.phierr(dest_global_pos_seg));
                          
                          m_propagatedSegGlb_x.push_back(dest_global_pos_seg.x());
                          m_propagatedSegGlb_y.push_back(dest_global_pos_seg.y());
                          m_propagatedSegGlb_z.push_back(dest_global_pos_seg.z());
                          m_propagatedSegGlb_r.push_back(dest_global_pos_seg.perp());
                          m_propagatedSegGlb_phi.push_back(dest_global_pos_seg.phi());
                          std::cout<<"ME11 propa"<<std::endl;  
                          std::cout<<"recoMu pt-eta: "<<muon.pt()<< " "<<muon.eta()<<std::endl;  
                          std::cout<<"x: "<<dest_global_pos_seg.x()<<" y: "<<dest_global_pos_seg.y()<<" z: "<<dest_global_pos_seg.z()<<std::endl;  
                          m_propagatedSegGlb_errX.push_back(dest_glob_error_x);
                          m_propagatedSegGlb_errY.push_back(dest_glob_error_y);
                          m_propagatedSegGlb_rerr.push_back(dest_global_r_err);
                          m_propagatedSegGlb_phierr.push_back(dest_global_phi_err);
  
                          m_propagatedSeg_pt.push_back(muon.pt());
                          m_propagatedSeg_phi.push_back(muon.phi());
                          m_propagatedSeg_eta.push_back(muon.eta());
                          m_propagatedSeg_charge.push_back(muon.charge());
                          
                          m_propagatedSegLoc_x.push_back(dest_local_pos.x());
                          m_propagatedSegLoc_phi.push_back(dest_local_pos.phi());
                          m_propagatedSegLoc_r.push_back(dest_local_pos.perp());
                          m_propagatedSegLoc_y.push_back(dest_local_pos.y());
                          m_propagatedSegLoc_dirX.push_back(dest_state_seg.localDirection().x());
                          m_propagatedSegLoc_dirY.push_back(dest_state_seg.localDirection().y());
                          m_propagatedSegLoc_dirZ.push_back(dest_state_seg.localDirection().z());
                          m_propagatedSegLoc_errX.push_back(dest_local_err.xx());
                          m_propagatedSegLoc_errY.push_back(dest_local_err.yy());
                      }
                    }//Loop over Eta Partition
                  }//Loop over chambers
                }//Loop over SuperChambers
              }//Loop over stations
            }//Loop over regions
          }//isME11 END propagation code (CSC Segment)  
        }//isCSC therefore perform propagation
      }//!muon.outerTrack().isNull()
    }//loop on reco muons
  }//muons.isValid()
  return;
  
}


const GEMEtaPartition* MuNtupleGEMMuonFiller::findEtaPartition(const GEMChamber* chamber, const GlobalPoint& global_point) {

  for (const GEMEtaPartition* eta_partition : chamber->etaPartitions()) {
    const LocalPoint&& local_point = eta_partition->toLocal(global_point);
    const LocalPoint local_point_2d(local_point.x(), local_point.y(), 0.0f);
    if (eta_partition->surface().bounds().inside(local_point_2d)) 
      return eta_partition;
  }

  return nullptr;
}

