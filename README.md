A Monte-Carlo Dosimetry Tool for Radiation Therapy Treatment Planning
===================================================================================================

Introduction
-------------------
The dosimetry tool has been developed to estimate more accurate radiation dose to patient by using a particle-transport Monte Carlo C++ toolkit, [GEANT4 (GEomtry ANd Tracking)](http://geant4.cern.ch/).  This Monte Carlo toolkit was first developed for high-energy physics applications, e.g. hadron physics, but physicists have been pulling/losing hairs to  implement and vadliate physics models for this toolkit to produce realistic models that would help users answer interestng research questions.  On top of all the physics awesomeness in GEANT4, the toolkit takes advantages of object-oriented programming for users to develop applications customized to his/her own need and design, e.g. tons of APIs action!


Details
--------------------
okay, if introduction was not enough for you, here are something to help you see this software in its potential playing field!

External beam radiation therapy has been an effective option to treat cancer when it's first diagnosed or localized diseases (i.e. stand-alone lesions easier to plan, treat, and target with external beam radiation source.  For relapses or refractory diseases, there has been a momentum in developing targeted radionuclide thearpy or any form of targeted therapy to provide more treatment options in addition to chemotherapy and radiation therapy.  However, radiation dose to patients using targeted radionuclide therapy has not been standardized and been actively investigated!  That's where this doisimetry tool comes in and do its magic to estimate radiation dose in a patient-specific approach.

### The tool performs the following steps:
- Read in the voxelized volume of a reference human phantom or the patient's CT images to define simulation geometry (body size and tissue composition)
- Read in the voxelized probability distribution of the radiation source
- Simulate and track possible events of particles interacting in the defined geometry with the definied source distribution
- Generate the dose output in voxelized format

For more information, the below poster will put this tool in context and explain how one can use this tool in the realm of cancer diagnosis and therapy.









Future software improvement
----------------------------
- Optimize software for more efficient runtime
- Implement factory design pattern
- Migrate and optimize the code for multi-thread capability (GEANT4.10.xx)
- Welcome any comment for improving the software design


Active Team
------------------------

### Core developers
Shih-ying Clare Huang, Ph.D, UCSF

### Inspirators, helpers, etc.
- Jungwook Shin, Ph.D. for his great help in getting started using Geant4
- GEANT4 collaboration community for debugging and development support
- Awesome GEANT4 workshops and instructors at SLAC, Stanford






