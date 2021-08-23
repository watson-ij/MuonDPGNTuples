import CRABClient
from CRABClient.UserUtilities import config
config = config()

#from WMCore.Configuration import Configuration

#config = Configuration()

#amkaur = getUsernameFromSiteDB()
config.section_("General")
#config.General.instance = 'preprod'
#config.General.requestName   = 'Efficiency_GEM'
config.General.transferLogs = True
config.General.transferOutputs = True


config.section_("JobType")
config.JobType.pluginName  = 'Analysis'
# Name of the CMSSW configuration file
config.JobType.psetName    = 'muDpgNtuples_cfg.py'
#config.JobType.outputFiles = ['Efficiency_XXXXXX_Express.root']
#config.JobType.outputFiles = ['test.root']
config.JobType.numCores = 4
config.section_("Data")
config.Data.inputDataset = '/ExpressCosmics/Commissioning2021-Express-v1/FEVT'

config.Data.inputDBS = 'global'
config.Data.splitting = 'LumiBased'
config.Data.unitsPerJob = 30
config.Data.outLFNDirBase = '/store/group/dpg_hcal/comm_hcal/AmanKaur'
config.Data.publication = False
# This string is used to construct the output dataset name
config.Data.outputDatasetTag = 'Efficiency_GEM_step1_340116'
config.Data.ignoreLocality = False
config.Data.runRange = '340116'
# These values only make sense for processing data
#    Select input data based on a lumi mask
#config.Data.lumiMask = 'Collisions18_324564_325172_json.txt'
#    Select input data based on run-ranges
#config.Data.runRange = '234407'

config.section_("Site")
# Where the output files will be transmitted to
config.Site.storageSite = 'T2_CH_CERN'
#config.Site.whitelist = ['T2_CH_CERN']


if __name__ == '__main__':
    from CRABAPI.RawCommand import crabCommand
    config.General.workArea = 'crabProjects_340116_muons'
    for myRunRange in ['340116']:
        config.General.requestName = 'run'+myRunRange
        config.Data.runRange = myRunRange
        crabCommand('submit', config = config)

