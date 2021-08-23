import CRABClient
from WMCore.Configuration import Configuration 

config = Configuration()

config.section_("General")
config.General.requestName = 'GEMMuonNTuplizerOverMC'
config.General.workArea = "/afs/cern.ch/user/f/fivone/Documents/NTuplizer/MWGR4/CMSSW_11_3_1_patch1/src/MuDPGAnalysis/MuonDPGNtuples/"
config.General.transferOutputs = True
config.General.transferLogs = True

config.section_("JobType")
config.JobType.pluginName = 'Analysis'
config.JobType.psetName = '/afs/cern.ch/user/f/fivone/Documents/NTuplizer/MWGR4/CMSSW_11_3_1_patch1/src/MuDPGAnalysis/MuonDPGNtuples/test/muDpgNtuples_cfg.py'
config.JobType.allowUndistributedCMSSW = True
# MWGR
# config.JobType.pyCfgParams = ['isMC=False','nEvents=-1']

# Zmumu
config.JobType.pyCfgParams = ['isMC=False','nEvents=-1']

config.section_("Data")
# MWGR
# config.Data.inputDataset = '/ExpressCosmics/Commissioning2021-Express-v1/FEVT'
# config.Data.inputDBS = 'global'
# config.Data.splitting = 'FileBased'
# config.Data.unitsPerJob = 180
# config.Data.publication = False
# config.Data.outLFNDirBase = '/store/user/fivone/GEMMuonNtuplizerZmumu'
# config.Data.outputDatasetTag = 'MWGR3_properLatencyOnGEM'
# config.Data.runRange = '341343'

# Zmumu
config.Data.inputDataset = "/Cosmics2021/caruta-Cosmics2021_112X_mcRun3_2021cosmics_realistic_deco_v13_RECO-7cd9c17a284f0ac1b81e68557875ecb2/USER"
config.Data.inputDBS = 'phys03'
config.Data.splitting = 'Automatic'
config.Data.unitsPerJob = 180
config.Data.publication = False
config.Data.outLFNDirBase = '/store/user/fivone/GEMMuonNtuplizerZmumu'
config.Data.outputDatasetTag = 'MC_Cosmic'

config.section_("Site")
config.Site.storageSite = 'T2_DE_RWTH'

config.section_("User")
config.User.voGroup = 'dcms'
