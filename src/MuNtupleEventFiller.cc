#include "MuDPGAnalysis/MuonDPGNtuples/src/MuNtupleEventFiller.h"

#include "FWCore/Framework/interface/Event.h"
#include "FWCore/Framework/interface/LuminosityBlock.h"

#include "FWCore/Framework/interface/EDAnalyzer.h"
#include "FWCore/Framework/interface/ESHandle.h"

MuNtupleEventFiller::MuNtupleEventFiller(edm::ConsumesCollector && collector,
					 const std::shared_ptr<MuNtupleConfig> config, 
					 std::shared_ptr<TTree> tree, const std::string & label) : 
  MuNtupleBaseFiller(config, tree, label)
{
}


MuNtupleEventFiller::~MuNtupleEventFiller() 
{ 

};

void MuNtupleEventFiller::initialize()
{
  
  m_tree->Branch((m_label + "_runNumber").c_str(), &m_runNumber, (m_label + "_runNumber/I").c_str());
  m_tree->Branch((m_label + "_lumiBlock").c_str(), &m_lumiBlock, (m_label + "_lumiBlock/I").c_str());
  m_tree->Branch((m_label + "_eventNumber").c_str(), &m_eventNumber, (m_label + "_eventNumber/L").c_str());
      
}

void MuNtupleEventFiller::clear()
{

  m_runNumber   = MuNtupleBaseFiller::DEFAULT_INT_VAL_POS;
  m_lumiBlock   = MuNtupleBaseFiller::DEFAULT_INT_VAL_POS;
  m_eventNumber = MuNtupleBaseFiller::DEFAULT_INT_VAL_POS;
    
}

void MuNtupleEventFiller::fill(const edm::Event & ev)
{

  clear();

  m_runNumber   = ev.run();
  m_lumiBlock   = ev.getLuminosityBlock().luminosityBlock();
  m_eventNumber = ev.eventAuxiliary().event();
  
  return;

}
