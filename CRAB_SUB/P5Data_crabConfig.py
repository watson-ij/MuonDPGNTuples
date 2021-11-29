import sys
import CRABClient
from WMCore.Configuration import Configuration 
from CRABAPI.RawCommand import crabCommand

config = Configuration()

config.section_("General")
config.General.requestName = '346247_ZeroBias19'
config.General.workArea = "/afs/cern.ch/user/f/fivone/Documents/NTuplizer/CRAFT/CMSSW_12_0_0/src/MuDPGAnalysis/"
config.General.transferOutputs = True
config.General.transferLogs = True

config.section_("JobType")
config.JobType.pluginName = 'Analysis'
config.JobType.psetName = '/afs/cern.ch/user/f/fivone/Documents/NTuplizer/CRAFT/CMSSW_12_0_0/src/MuDPGAnalysis/MuonDPGNtuples/test/muDpgNtuples_cfg.py'
config.JobType.allowUndistributedCMSSW = True
config.JobType.pyCfgParams = ['isMC=False','nEvents=-1']

config.section_("Data")
config.Data.inputDataset = '/ZeroBias19/Commissioning2021-PromptReco-v1/AOD'
config.Data.inputDBS = 'global'
config.Data.splitting = 'FileBased'
config.Data.unitsPerJob = 180
config.Data.publication = False
config.Data.outLFNDirBase = '/store/group/dpg_gem/comm_gem/P5_Commissioning/2021/GEM'
#config.Data.outLFNDirBase = '/store/user/fivone/GEMMuonNtuplizerZmumu'
config.Data.allowNonValidInputDataset = True

config.section_("Site")
config.Site.storageSite = 'T2_CH_CERN'
#config.Site.storageSite = 'T2_DE_RWTH'

#config.section_("User")
#config.User.voGroup = 'dcms'

run_list =[346919]# [346774, 346775, 346777, 346778, 346850, 346853, 346856, 346860, 346861, 346866, 346871, 346874, 346876, 346919, 346920, 346922, 346923, 346924, 346975, 346978, 346979, 346980, 347072, 347073, 347143, 347148, 347152, 347206, 347209, 347220, 347222, 347258, 347270, 347272]#[346512,346307,346494,346452,346396]# [346375, 346377, 346378, 346379, 346380, 346381, 346382, 346395, 346396, 346398, 346446, 346447, 346450, 346452, 346453, 346455, 346483, 346484, 346485, 346486, 346488, 346490, 346494, 346507, 346509, 346512, 346307, 346374]



for run_number in run_list:

    config.Data.inputDataset = '/Cosmics/Commissioning2021-PromptReco-v1/AOD'
    config.Data.outputDatasetTag = str(run_number)+'_Prompt_CosmicsPrompt'
    config.General.requestName = str(run_number)+'_Prompt_CosmicsPrompt'
    config.Data.runRange = str(run_number)
    crabCommand('submit', config = config)
sys.exit(0)
