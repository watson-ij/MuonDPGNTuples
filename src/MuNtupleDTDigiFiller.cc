/** \class MuNtupleDigiFiller MuNtupleDigiFiller.cc MuDPGAnalysis/MuonDPGNtuples/src/MuNtupleDTDigiFiller.cc
 *  
 * Helper class : the digi filler for Phase-1 / Phase2 DT digis (the DataFormat is the same)
 *
 * \author C. Battilana (INFN BO)
 *
 *
 */

#include "MuDPGAnalysis/MuonDPGNtuples/src/MuNtupleDTDigiFiller.h"

#include "FWCore/Framework/interface/Event.h"
#include "FWCore/Framework/interface/EDAnalyzer.h"
#include "FWCore/Framework/interface/ESHandle.h"

MuNtupleDTDigiFiller::MuNtupleDTDigiFiller(edm::ConsumesCollector && collector,
				       const std::shared_ptr<MuNtupleConfig> config, 
				       std::shared_ptr<TTree> tree, const std::string & label,
				       Tag tag) : 
  MuNtupleBaseFiller(config, tree, label), m_tag(tag)
{

  edm::InputTag & iTag = m_tag == Tag::PH1 ?
                                  m_config->m_inputTags["ph1DtDigiTag"] :
                                  m_config->m_inputTags["ph2DtDigiTag"];

  if (iTag.label() != "none") m_dtDigiToken = collector.consumes<DTDigiCollection>(iTag);

}

MuNtupleDTDigiFiller::~MuNtupleDTDigiFiller() 
{ 

};

void MuNtupleDTDigiFiller::initialize()
{
  
  m_tree->Branch((m_label + "_nDigis").c_str(), &m_nDigis, (m_label + "_nDigis/i").c_str());
  
  m_tree->Branch((m_label + "_wheel").c_str(),   &m_digi_wheel);
  m_tree->Branch((m_label + "_sector").c_str(),  &m_digi_sector);
  m_tree->Branch((m_label + "_station").c_str(), &m_digi_station);

  m_tree->Branch((m_label + "_superLayer").c_str(), &m_digi_superLayer);
  m_tree->Branch((m_label + "_layer").c_str(),      &m_digi_layer);
  m_tree->Branch((m_label + "_wire").c_str(),       &m_digi_wire);

  m_tree->Branch((m_label + "_time").c_str(), &m_digi_time);
  
}

void MuNtupleDTDigiFiller::clear()
{

  m_nDigis = 0;

  m_digi_wheel.clear();
  m_digi_sector.clear();
  m_digi_station.clear();

  m_digi_superLayer.clear();
  m_digi_layer.clear();
  m_digi_wire.clear();

  m_digi_time.clear();

}

void MuNtupleDTDigiFiller::fill(const edm::Event & ev)
{

  clear();

  auto dtDigis = conditionalGet<DTDigiCollection>(ev, m_dtDigiToken,"DTDigiCollection");

  if (dtDigis.isValid()) 
    {
      
      auto dtLayerIdIt  = dtDigis->begin();
      auto dtLayerIdEnd = dtDigis->end();
      
      for (; dtLayerIdIt != dtLayerIdEnd; ++dtLayerIdIt)
	{

	  const auto & dtLayerId = (*dtLayerIdIt).first;

	  auto digiIt  = (*dtLayerIdIt).second.first;
	  auto digiEnd = (*dtLayerIdIt).second.second;
	  
	  for (; digiIt != digiEnd; ++digiIt)
	    {
	      m_digi_wheel.push_back(dtLayerId.wheel());
	      m_digi_sector.push_back(dtLayerId.sector());
	      m_digi_station.push_back(dtLayerId.station());

	      m_digi_superLayer.push_back(dtLayerId.superLayer());
	      m_digi_layer.push_back(dtLayerId.layer());
	      m_digi_wire.push_back(digiIt->wire());

	      m_digi_time.push_back(digiIt->time());
	      
	      m_nDigis++;

	    }
	}
    }
  
  return;

}

