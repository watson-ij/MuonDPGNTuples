/** \class MuNtupleBaseFiller MuNtupleBaseFiller.cc MuDPGAnalysis/MuonDPGNtuples/src/MuNtupleBaseFiller.cc
 *  
 * Helper class defining the generic interface of a filler
 *
 * \author C. Battilana (INFN BO)
 *
 *
 */

#include "MuDPGAnalysis/MuonDPGNtuples/src/MuNtupleTrackBaseFiller.h" 

MuNtupleTrackBaseFiller::MuNtupleTrackBaseFiller(const std::shared_ptr<MuNtupleConfig> config, 
				       std::shared_ptr<TTree> tree, const std::string & label) : 
  m_config(config), m_tree(tree), m_label(label) 
{ 
  
}

MuNtupleTrackBaseFiller::~MuNtupleTrackBaseFiller()
{ 

}
