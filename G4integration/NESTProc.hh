#ifndef NESTPROC_h
#define NESTPROC_h 1

#include <functional>

#include "G4Track.hh"
#include "G4DynamicParticle.hh"
#include "G4Material.hh"
#include "G4OpticalPhoton.hh"
#include "G4Step.hh"
#include "G4ThreeVector.hh"
#include "G4VRestDiscreteProcess.hh"
#include "G4SystemOfUnits.hh"
#include "NEST.hh"

namespace NEST {

struct Hit {
 public:
  Hit(double _E, double _t, G4ThreeVector _xyz)
      : E(_E), t(_t), xyz(_xyz), result{0, 0, 0, 0, 0, 0} {};
  double E;
  double t;
  G4ThreeVector xyz;
  QuantaResult result;
};

struct Lineage {
 public:
  Lineage(INTERACTION_TYPE _type) : type(_type){};
  INTERACTION_TYPE type = NoneType;
  std::vector<Hit> hits;
  double density = -1;
  int A = -1;
  int Z = -1;
  NESTresult result;
  bool result_calculated = false;
};

class NoTimeParticleChange : public G4ParticleChange {
 public:
  NoTimeParticleChange() : G4ParticleChange() { debugFlag = false; }
};

class NESTProc : public G4VRestDiscreteProcess {
 public:  // constructor and destructor
  NESTProc(const G4String& processName, G4ProcessType type,
           VDetector* detector);
  NESTProc(const G4String& processName, G4ProcessType type,
           NESTcalc* customcalc, VDetector* detector);
  ~NESTProc();

 public:  // methods, with descriptions
  G4bool IsApplicable(const G4ParticleDefinition& aParticleType);
  // Returns true -> 'is applicable', for any particle type except for an
  // 'opticalphoton' and for short-lived particles

  G4double GetMeanFreePath(const G4Track& aTrack, G4double, G4ForceCondition*);
  // Returns infinity; i. e. the process does not limit the step, but
  // sets the 'StronglyForced' condition for the DoIt to be invoked at
  // every step.

  G4double GetMeanLifeTime(const G4Track& aTrack, G4ForceCondition*);
  // Returns infinity; i. e. the process does not limit the time, but
  // sets the 'StronglyForced' condition for the DoIt to be invoked at
  // every step.

  // For in-flight particles losing energy (or those stopped)
  G4VParticleChange* PostStepDoIt(const G4Track& aTrack, const G4Step& aStep);
  G4VParticleChange* AtRestDoIt(const G4Track& aTrack, const G4Step& aStep);

  void TryPopLineages(const G4Track& aTrack, const G4Step& aStep);

  Lineage GetChildType(const G4Track* aTrack, const G4Track* sec) const;
  void SetDetailedSecondaries(bool detailed) {
    detailed_secondaries = detailed;
  }

  void SetGamma_break(double _gamma_break) { this->gamma_break = _gamma_break; }

  double GetGamma_break() const { return gamma_break; }

  void SetNESTcalc(std::unique_ptr<NEST::NESTcalc> newcalc) {
    fNESTcalc.reset(newcalc.release());
    fDetector.reset(newcalc->GetDetector());
  }

 protected:
  // bools for tracking some special particle cases

  std::unique_ptr<NEST::NESTcalc> fNESTcalc = NULL;
  std::vector<NEST::Lineage> lineages;
  std::map<std::tuple<int, CLHEP::Hep3Vector, CLHEP::Hep3Vector>, uint64_t>
      track_lins;
  std::unique_ptr<VDetector> fDetector;
  NoTimeParticleChange fParticleChange;

  bool detailed_secondaries = true;
  double gamma_break =
      9 * mm;  // Gammas will not pass on their lineage (if they have one, e.g.
               // bremsstrahlung) if they are this far from their origin.

};

////////////////////
// Inline methods
////////////////////

inline G4bool NESTProc::IsApplicable(
    const G4ParticleDefinition& aParticleType) {
  if (aParticleType.GetParticleName() == "opticalphoton") return false;
  if (aParticleType.IsShortLived()) return false;
  // if(abs(aParticleType.GetPDGEncoding())==2112 || //neutron (no E-dep.)
  return true;
}

}  // namespace NEST
#endif /* NESTPROC_h */
