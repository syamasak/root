/// \file
/// \ingroup tutorial_pythia
/// pythia8 basic example
///
/// to run, do:
///
/// ~~~{.cpp}
///  root > .x pythia8.C
/// ~~~
///
/// Note that before executing this script,
///
///  - the env variable PYTHIA8 must point to the pythia8100 (or newer) directory
///  - the env variable PYTHIA8DATA must be defined and it must point to $PYTHIA8/xmldoc
///
/// \macro_code
///
/// \author Andreas Morsch

#include "TSystem.h"
#include "TH1F.h"
#include "TClonesArray.h"
#include "TPythia8.h"
#include "TParticle.h"
#include "TDatabasePDG.h"
#include "TCanvas.h"

void pythia8(Int_t nev  = 100, Int_t ndeb = 1)
{
   const char *p8dataenv = gSystem->Getenv("PYTHIA8DATA");
   if (!p8dataenv) {
      const char *p8env = gSystem->Getenv("PYTHIA8");
      if (!p8env) {
         Error("pythia8.C",
               "Environment variable PYTHIA8 must contain path to pythia directory!");
         return;
      }
      TString p8d = p8env;
      p8d += "/xmldoc";
      gSystem->Setenv("PYTHIA8DATA", p8d);
   }

   const char* path = gSystem->ExpandPathName("$PYTHIA8DATA");
   if (gSystem->AccessPathName(path)) {
         Error("pythia8.C",
               "Environment variable PYTHIA8DATA must contain path to $PYTHIA8/xmldoc directory !");
      return;
   }

// Load libraries
   gSystem->Load("libEG");
   gSystem->Load("libEGPythia8");
// Histograms
   TH1F* etaH = new TH1F("etaH", "Pseudorapidity", 120, -12., 12.);
   TH1F* ptH  = new TH1F("ptH",  "pt",              50,   0., 10.);


// Array of particles
   TClonesArray* particles = new TClonesArray("TParticle", 1000);
// Create pythia8 object
   TPythia8* pythia8 = new TPythia8();

// Configure
   pythia8->ReadString("HardQCD:all = on");


// Initialize

   pythia8->Initialize(2212 /* p */, 2212 /* p */, 14000. /* TeV */);

// Event loop
   for (Int_t iev = 0; iev < nev; iev++) {
      pythia8->GenerateEvent();
      if (iev < ndeb) pythia8->EventListing();
      pythia8->ImportParticles(particles,"All");
      Int_t np = particles->GetEntriesFast();
// Particle loop
      for (Int_t ip = 0; ip < np; ip++) {
         TParticle* part = (TParticle*) particles->At(ip);
         Int_t ist = part->GetStatusCode();
         // Positive codes are final particles.
         if (ist <= 0) continue;
         Int_t pdg = part->GetPdgCode();
         Float_t charge = TDatabasePDG::Instance()->GetParticle(pdg)->Charge();
         if (charge == 0.) continue;
         Float_t eta = part->Eta();
         Float_t pt  = part->Pt();

         etaH->Fill(eta);
         if (pt > 0.) ptH->Fill(pt, 1./(2. * pt));
      }
   }

   pythia8->PrintStatistics();

   TCanvas* c1 = new TCanvas("c1","Pythia8 test example",800,800);
   c1->Divide(1, 2);
   c1->cd(1);
   etaH->Scale(5./Float_t(nev));
   etaH->Draw();
   etaH->SetXTitle("#eta");
   etaH->SetYTitle("dN/d#eta");

   c1->cd(2);
   gPad->SetLogy();
   ptH->Scale(5./Float_t(nev));
   ptH->Draw();
   ptH->SetXTitle("p_{t} [GeV/c]");
   ptH->SetYTitle("dN/dp_{t}^{2} [GeV/c]^{-2}");
 }
