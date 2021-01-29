# MuonDPGNtuples

 Preliminary version of the GE11 ntuples, developed as a part of the CMS Muon DPGO common ntuples: https://gitlab.cern.ch/cms-muon-dpgo/muondpgntuples

## Install and run the ntuples:

```
cmsrel CMSSW_11_1_4
cd CMSSW_11_1_4/src/
cmsenv

git clone https://github.com/gmilella12/MyMuonDPGNTuples.git MuDPGAnalysis/MuonDPGNtuples

scram b -j 5

cd MuDPGAnalysis/MuonDPGNtuples/test/
cmsRun muDpgNtuples_cfg.py
```

## Overview of the ntuple structure:

These ntuples are intended to consist mostly of flat collection of `std::vectors<>`, with the exception for few `TClonesArrays` which handle "vectors of vectors".

The class steering ntuple production is called `MuNtupleProducer`. It runs a list of helper classes, called _fillers_ inheriting from `MuNtupleBaseFiller` or from `MuNtupleTrackBaseFiller`.

Each _filler_ must include the `initialize()`, `clear()` and `fill()` functions, which are called within the `MuNtupleProducer` `beginJob()` and `analyze()` functions to perform the ntuple filling logic.

Many fillers of the same type may exist, for example in the DT slice-test the same data format for phase-1 and phase-2 digis is used, implying that the same _filler_ helper class can be reused to reduce code duplication. Different flavours of a filler are configured using _labels_ and _tags_. A _label_ specifies the initial part of the name of the branches that a _filler_ writes into the output `TTree`, for example `dtDigi_time` versus `ph2DtDigi_time`. A _tag_ is used to configure the specific behaviour of a given filler, for example phase-1 and phase-2 digis from the DT slice-test are produced with different `InputTags`, hence, in the DT digi _filler_ case, the _tag_ will specify what input collection should be used.

The _fillers_ included in the ntuple are defined in the `MuNtupleProducer` constructor with as in the following:

```
[...]

  m_fillers.push_back(std::make_unique<MuNtupleDTDigiFiller>(consumesCollector(), m_config, m_tree, "dtDigi",    MuNtupleDTDigiFiller::Tag::PH1));
  m_fillers.push_back(std::make_unique<MuNtupleDTDigiFiller>(consumesCollector(), m_config, m_tree, "ph2DtDigi", MuNtupleDTDigiFiller::Tag::PH2));

[...]
```

`dtDigi` or `ph2DtDigi` are _labels_ , and `MuNtupleDTDigiFiller::Tag::PH1` or `MuNtupleDTDigiFiller::Tag::PH1` are _tags_ of the same _filler_ base class.

A configuration helper class, called `MuNtupleConfig` also exists.

It deals with: _(i)_ the list of `InputTags` used by all _fillers_, _(ii)_ geometry, calibration and other `EventSetup` quantities.

The configuration of the ntuple producer is available under `python/muNtupleProducer_cfi.py`.

## How to include quantities from your subsystem into the ntuple:

1. Create one (or more) filler(s), which inherit from `src/MuNtupleBaseFiller.{h,cc}` under the `src/` dircetory;
1. Add all relevant `InputTag` and `EventSetup` quantities into `src/MuNtupleConfig.{h,cc}`;
1. Include your filler into the `m_fillers` vector of the `plugins/MuNtupleProducer.{h,cc}` steering class;
1. Include all the needed configuration parameters in `python/muNtupleProducer_cfi.py;
1. Adjust `Buildfile.xml` as needed;
1. Compile, run ` cmsRun test/muDpgNtuples_cfg.py` and check that the ntuple includes your new quantities.

**NOTEs**: 
- a _filler_ .h file is the best place where to document the quantities that are filled (e.g. mention units, ranges, etc), look [`src/ MuNtupleDTDigiFiller.h`]() as an example;
- the `MuNtupleBaseFiller` class includes placeholders for default values in the ntuple (e.g. `DEFAULT_INT_VAL`), please use those (and not "magic" numbers) when filling with default values is needed;
- the `MuNtupleBaseFiller` class includes a `conditionalGet()` function that provides a default dump in case a collection is missing in the input file, please use it.
- the `MuNtupleTrackBaseFiller` class includes, in addition, also information on the event setup.  

A complete example showing how to include phase-1 and phase-2 DT digis is available [here]().



