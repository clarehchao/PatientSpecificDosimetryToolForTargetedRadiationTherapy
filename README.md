A Monte Carlo Dosimetry Tool for Radiation Therapy Treatment Planning
===================================================================================================

Introduction
-------------------
The field of radiology involved taking images of human body by x-ray CT (computed tomography), PET (positron emission tomography), and SPECT (single photon emission computed tomography) to help disease diagnosis and treatment.  While the use of radiation is helpful to detect diseases such as cancer, one should be mindful of the iimpact of radiation to human body when imaging patients using radiation such as x-ray (CT), gamma ray (SPECT), or positrons (PET).

Radiation dosimetry is the study of quantifying the amount of energy imparted to a material (e.g. human body) by ionizing radiation  So, the higher the dose, the more biological damage there might be to human body.  X-ray CT imaging systems impart radiation to human body froom radiation source OUTSIDE of the body while PET/SPECT systems impart radiation from source INSIDE the body.  One can measure the radiation dose by dosimeters, but these measurement could be crude when it comes to imaging patients complicated imaging systems.  Thus, the need to calculate radiation dose to human body accurately is crucial to design effective imaging and treatment procedures.


A dosimetry tool has been developed to estimate more accurate radiation dose to patient by using a particle-transport Monte Carlo C++ toolkit, [GEANT4 (GEomtry ANd Tracking)](http://geant4.cern.ch/).  This tool takes in the tissue composition and body/organ contour specific to a patient to estimate the radiation dose from a given distribution of radiation.  This Monte Carlo toolkit was first developed for high-energy physics applications, e.g. hadron physics, but physicists have been pulling/losing hairs to  implement and vadliate physics models for this toolkit to produce realistic models that would help users answer interestng research questions.  On top of all the physics awesomeness in GEANT4, the toolkit takes advantages of object-oriented programming for users to develop applications customized to his/her own need and design!


Details
--------------------
okay, if introduction was not enough for you, here are something to help you see this software in its potential playing field!

External beam radiation therapy has been an effective option to treat cancer when the disease is first diagnosed or localized (i.e. stand-alone lesions easier to plan, treat, and target with external beam radiation source.  For relapses or refractory diseases, there has been a momentum in developing targeted radionuclide thearpy or any form of targeted therapy to provide more treatment options in addition to chemotherapy and radiation therapy.  However, radiation dose to patients using targeted radionuclide therapy has not been standardized and been actively investigated!  That's where this doisimetry tool comes in and do its magic to estimate radiation dose in a patient-specific approach.

### The tool performs the following steps:
- Read in the voxelized volume of a reference human phantom or the patient's CT images to define simulation geometry (body size and tissue composition)
- Read in the voxelized probability distribution of the radiation source
- Simulate and track possible events of particles interacting in the defined geometry with the definied source distribution
- Generate the dose output in voxelized format

Please see [the software manual](SoftwareManual.txt) for detailed description and usage of the software.

Note: this tool was developed using Geant4 toolkit version 4.9.6.p02.  Full installation of Geant4 on your OS is assumed to use this tool properly.  See this link for [the Geant4 installation videos](http://geant4.in2p3.fr/spip.php?article84&lang=en) and [the Geant4 installation guide](http://geant4.web.cern.ch/geant4/UserDocumentation/UsersGuides/InstallationGuide/html/).


Findings
--------------------
This tool was found to be helpful in estimating near patient-specific dosimetry using a computation phantom as a surrogate of the patient's anatomy.  We were able to estimate radiation dose a patient underwent iodine-131 metaiodobenzylguanidine (MIBG) targeted radionuclide therapy with I-124-MIBG PET/CT pretherapy imaging.  We found that the I-124-MIBG PET/CT imaging was helpful to estimat the duration of radiation residing in the body (residence time) for the course of I-131 MIBG treatment.  Combining the patient-specific residence time with the dose estimate from the Monte Carlo dosimetry tool, the absorbed dose to healthy organs and lesions were estimated.  See more details and finding in [this poster](doc/I131MIBG_CologneANR2014_Huang.jpg) presented at Advanced Neuroblastoma Research, May 2014, Cologn, Germany.


Future software improvement
----------------------------
- Optimize software for more efficient runtime
- Implement factory design pattern
- Migrate and optimize the code for multi-thread capability (GEANT4.10.xx)
- Welcome any comment for improving the software design


Active Team
------------------------
### Principle Investigator of 
Prof. Youngho Seo, PhD, UCSF

### Collaborators:
Katherine K. Matthay, MD, UCSF
Randall A. Hawkins, MD, PhD, UCSF
Prof. Henry F. Van Brocklin, PhD, UCSF
Choonsik Lee, PhD, National Cancer Institutes
Westley E. Bolch, PhD, University of Florida

### Core developers
Shih-ying Clare Huang, Ph.D, UCSF

### Inspirators, helpers, etc.
- Jungwook Shin, Ph.D. for his great help in getting started using Geant4
- GEANT4 collaboration community for debugging and development support
- Awesome GEANT4 workshops and instructors at SLAC, Stanford






