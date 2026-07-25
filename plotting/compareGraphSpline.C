// plotting/compareGraphSpline.C
//
// Compare TGraph (linear) vs TSpline3 (natural cubic) interpolation for the
// per-event systematic response-function knots stored in mydataset.root.
//
// Task-3 mode — three-pad horizontal canvas, one pad per dial:
//   root -l -q 'plotting/compareGraphSpline.C(12)'
//   Output: output/plots/compareGraphSpline_event<N>.pdf
//
// Task-4 mode — multi-pad layout PLUS a clean single-pad spline overlay:
//   root -l -q 'plotting/compareGraphSpline.C(12, true)'
//   Output 1: output/plots/compareGraphSpline_overlay_event<N>.pdf  (3-pad)
//   Output 2: output/plots/compareGraphSpline_alloverlay_event<N>.pdf (1-pad)
//
// Run from the REPOSITORY ROOT.

#include <TCanvas.h>
#include <TClonesArray.h>
#include <TFile.h>
#include <TGraph.h>
#include <TLatex.h>
#include <TLegend.h>
#include <TMultiGraph.h>
#include <TSpline.h>
#include <TStyle.h>
#include <TSystem.h>
#include <TTree.h>
#include <iostream>

// ── Static configuration ──────────────────────────────────────────────────────

static const char*  kDataFile     = "example/advanced/inputs/datasets/mydataset.root";
static const char*  kTreeName     = "events";
static const char*  kBranch[]     = {"par1_TGraph",    "par2_TGraph",    "par3_TGraph"};
static const char*  kLabel[]      = {"par1  (smooth)", "par2  (smooth)", "par3  (non-smooth)"};
static const int    kColor[]      = {kRed+1, kBlue+1, kGreen+2};
static const int    kDialMarker[] = {20, 21, 22};   // filled circle / square / triangle
static const int    kNDials       = 3;
static const double kXMin         = -3.5;
static const double kXMax         =  3.5;
static const int    kNSteps       = 300;

// ── Curve builders ────────────────────────────────────────────────────────────

TGraph* makeCurve(TGraph* raw, TSpline3* sp) {
    auto* g = new TGraph(kNSteps);
    for (int i = 0; i < kNSteps; ++i) {
        double x = kXMin + i * (kXMax - kXMin) / (kNSteps - 1);
        g->SetPoint(i, x, sp ? sp->Eval(x) : raw->Eval(x));
    }
    return g;
}

// ── Per-pad drawing helper ────────────────────────────────────────────────────
//
// Draws one dial comparison into the current pad:
//   - knot markers (black)
//   - TGraph linear interpolation (color, dashed)
//   - TSpline3 cubic interpolation (color, solid)
// The pad title is drawn via TLatex so it survives SetOptTitle(0).

void drawDialPad(TGraph* raw, TSpline3* sp,
                 const char* label, int color, int dialMarker) {

    gPad->SetLeftMargin(0.17);
    gPad->SetBottomMargin(0.17);
    gPad->SetTopMargin(0.15);
    gPad->SetRightMargin(0.04);

    TGraph* linG   = makeCurve(raw, nullptr);
    TGraph* splG   = makeCurve(raw, sp);
    TGraph* knotsG = new TGraph(*raw);

    // Line styles
    linG->SetLineColor(color);  linG->SetLineWidth(2); linG->SetLineStyle(2);
    splG->SetLineColor(color);  splG->SetLineWidth(2); splG->SetLineStyle(1);
    // Knots: black markers, no connecting line
    knotsG->SetMarkerColor(kBlack); knotsG->SetMarkerStyle(dialMarker);
    knotsG->SetMarkerSize(1.3);     knotsG->SetLineColor(kBlack);

    TMultiGraph* mg = new TMultiGraph();
    mg->Add(linG,   "L");
    mg->Add(splG,   "L");
    mg->Add(knotsG, "P");
    mg->SetTitle(";Parameter shift [#sigma];Weight");
    mg->Draw("A");

    // Apply x-range and axis styles AFTER Draw() so the axes exist
    mg->GetXaxis()->SetRangeUser(kXMin, kXMax);
    mg->GetXaxis()->SetTitleSize(0.068);
    mg->GetXaxis()->SetLabelSize(0.060);
    mg->GetXaxis()->SetTitleOffset(0.90);
    mg->GetYaxis()->SetTitleSize(0.068);
    mg->GetYaxis()->SetLabelSize(0.060);
    mg->GetYaxis()->SetTitleOffset(1.10);
    gPad->Modified();
    gPad->Update();

    // Pad title drawn manually so it is NOT inside the legend box
    TLatex* tl = new TLatex();
    tl->SetNDC();
    tl->SetTextFont(62);        // bold Helvetica
    tl->SetTextSize(0.072);
    tl->SetTextAlign(22);       // center+middle
    // x center = left_margin + half of plot width
    double xC = 0.17 + (1.0 - 0.17 - 0.04) / 2.0;   // ≈ 0.565
    tl->DrawLatex(xC, 0.930, label);

    // Legend — compact, 3 entries, no border, no header
    TLegend* leg = new TLegend(0.19, 0.64, 0.62, 0.87);
    leg->SetBorderSize(0);
    leg->SetFillStyle(0);
    leg->SetTextSize(0.052);
    leg->AddEntry(knotsG, "Knots",            "P");
    leg->AddEntry(linG,   "TGraph (linear)",  "L");
    leg->AddEntry(splG,   "TSpline3 (cubic)", "L");
    leg->Draw();
}

// ── Main entry point ──────────────────────────────────────────────────────────

void compareGraphSpline(int eventNum = 12, bool overlayAll = false) {
    gStyle->SetOptStat(0);
    gStyle->SetOptTitle(0);

    TFile* f = TFile::Open(kDataFile, "READ");
    if (!f || f->IsZombie()) {
        std::cerr << "ERROR: cannot open " << kDataFile << std::endl;
        return;
    }
    TTree* t = (TTree*)f->Get(kTreeName);
    if (!t || eventNum >= (int)t->GetEntries()) {
        std::cerr << "ERROR: tree not found or eventNum out of range" << std::endl;
        f->Close(); return;
    }

    // Read one event from all three dial branches
    TClonesArray* arr[kNDials] = {};
    for (int d = 0; d < kNDials; ++d)
        t->SetBranchAddress(kBranch[d], &arr[d]);
    t->GetEntry(eventNum);

    // Copy TGraphs out of the TClonesArray and build TSpline3 objects
    TGraph*   raw[kNDials];
    TSpline3* spl[kNDials];
    for (int d = 0; d < kNDials; ++d) {
        TGraph* src = (TGraph*)arr[d]->At(0);
        if (!src) {
            std::cerr << "ERROR: no TGraph in " << kBranch[d]
                      << " for event " << eventNum << std::endl;
            f->Close(); return;
        }
        raw[d] = new TGraph(*src);
        spl[d] = new TSpline3(kBranch[d], raw[d]);
    }

    gSystem->mkdir("output/plots", kTRUE);

    // ── Task-3: three-pad horizontal canvas (1500 × 520) ──────────────────
    if (!overlayAll) {
        TCanvas* c = new TCanvas("c3", "Graph vs Spline per dial", 1500, 520);
        c->Divide(3, 1, 0.003, 0.003);
        for (int d = 0; d < kNDials; ++d) {
            c->cd(d + 1);
            drawDialPad(raw[d], spl[d], kLabel[d], kColor[d], kDialMarker[d]);
        }
        TString out = TString::Format(
            "output/plots/compareGraphSpline_event%d.pdf", eventNum);
        c->Print(out);
        std::cout << "\n[Task-3]  Saved => " << out << std::endl;

    // ── Task-4: multi-pad + clean single-pad overlay ───────────────────────
    } else {

        // --- Primary: same three-pad layout as Task-3 ----------------------
        TCanvas* c_mp = new TCanvas("c4m", "All dials multi-pad", 1500, 520);
        c_mp->Divide(3, 1, 0.003, 0.003);
        for (int d = 0; d < kNDials; ++d) {
            c_mp->cd(d + 1);
            drawDialPad(raw[d], spl[d], kLabel[d], kColor[d], kDialMarker[d]);
        }
        TString out_mp = TString::Format(
            "output/plots/compareGraphSpline_overlay_event%d.pdf", eventNum);
        c_mp->Print(out_mp);
        std::cout << "\n[Task-4 multi-pad]   Saved => " << out_mp << std::endl;

        // --- Secondary: single-pad, TSpline3 only (clean cross-dial view) --
        TCanvas* c_sp = new TCanvas("c4s", "All dials spline overlay", 920, 680);
        c_sp->SetLeftMargin(0.13);
        c_sp->SetBottomMargin(0.12);
        c_sp->SetTopMargin(0.07);
        c_sp->SetRightMargin(0.04);

        TMultiGraph* mg = new TMultiGraph();
        // Store pointers for legend (only splines + knots per dial)
        TGraph* splGs[kNDials], *knotsGs[kNDials];
        for (int d = 0; d < kNDials; ++d) {
            splGs[d]   = makeCurve(raw[d], spl[d]);
            knotsGs[d] = new TGraph(*raw[d]);

            splGs[d]->SetLineColor(kColor[d]);    splGs[d]->SetLineWidth(2);
            knotsGs[d]->SetMarkerColor(kColor[d]);
            knotsGs[d]->SetMarkerStyle(kDialMarker[d]);
            knotsGs[d]->SetMarkerSize(1.3);

            mg->Add(splGs[d],   "L");
            mg->Add(knotsGs[d], "P");
        }
        mg->SetTitle(";Parameter shift [#sigma];Weight");
        mg->Draw("A");
        mg->GetXaxis()->SetRangeUser(kXMin, kXMax);
        mg->GetXaxis()->SetTitleSize(0.052);
        mg->GetXaxis()->SetLabelSize(0.046);
        mg->GetXaxis()->SetTitleOffset(0.90);
        mg->GetYaxis()->SetTitleSize(0.052);
        mg->GetYaxis()->SetLabelSize(0.046);
        mg->GetYaxis()->SetTitleOffset(1.20);
        gPad->Modified();
        gPad->Update();

        // Canvas-level title via TLatex
        TLatex* tl = new TLatex();
        tl->SetNDC();
        tl->SetTextFont(62);
        tl->SetTextSize(0.040);
        tl->SetTextAlign(22);
        tl->DrawLatex(0.50, 0.965,
            TString::Format("Event %d  TSpline3 (suppl.)", eventNum));

        // Legend: one row per dial, colour + marker identifies it
        TLegend* leg = new TLegend(0.14, 0.68, 0.52, 0.92);
        leg->SetBorderSize(0);
        leg->SetFillStyle(0);
        leg->SetTextSize(0.042);
        for (int d = 0; d < kNDials; ++d)
            leg->AddEntry(splGs[d], kLabel[d], "LP");
        leg->Draw();

        TString out_sp = TString::Format(
            "output/plots/compareGraphSpline_alloverlay_event%d.pdf", eventNum);
        c_sp->Print(out_sp);
        std::cout << "[Task-4 single-pad]  Saved => " << out_sp << std::endl;
    }

    f->Close();
}
