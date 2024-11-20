#include <TFile.h>
#include <TH1F.h>
#include <TCanvas.h>
#include <TLegend.h>
#include <TString.h>
#include <vector>

TString year="2023_CMSSW_14";
// TString rootFileName = "/eos/lyoeos.in2p3.fr/grid/cms/store/user/damram/EGamma/2022postEE/241030.root";
// TString rootFileName= "/eos/lyoeos.in2p3.fr/grid/cms/store/user/damram/DYto2E_M-50_NNPDF31_TuneCP5_13p6TeV-powheg-pythia8/DYto2E_2022_preEE/241030_154401/mc.root";
// TString rootFileName = "/eos/lyoeos.in2p3.fr/grid/cms/store/user/damram/DYto2E_M-50_NNPDF31_TuneCP5_13p6TeV-powheg-pythia8/DYto2E_2022_postEE/241030_154734/mc.root";
TString rootFileName = "/eos/lyoeos.in2p3.fr/grid/cms/store/user/damram/DYTo2L_MLL-50_forCalib_TuneCP5_13p6TeV_madgraphMLM-pythia8/test2023DY/241030_215813/mc.root";

void plotsGen(TString histLoc, TString histAod) {
    // Ouvrir le fichier ROOT
    TFile* file = new TFile("../python/mc2024miniAod.root", "READ");

    // Accéder au dossier "demo"
    // TDirectory* demoFolder = file->GetDirectory("demo");
    // TList* keys = demoFolder->GetListOfKeys();

    // //Parcourir la liste des clés et afficher les noms des objets
    // TIter next(keys);
    // TKey* key;
    // while ((key = dynamic_cast<TKey*>(next()))) {
    //     TObject* obj = key->ReadObj();
    //     std::cout << "Nom de l'objet : " << obj->GetName() << "Et la classe :"<<obj->ClassName()<< std::endl;
    // }

    // Lire les histogrammes à l'intérieur du dossier
    TH1D* histo1 = dynamic_cast<TH1D*>(file->Get(histLoc));
    histo1->SetLineColor(kBlue);
    TH1D* histo2 = dynamic_cast<TH1D*>(file->Get(histAod));
    histo1->SetLineColor(kRed);
    std::cout<<histLoc<<" / "<<histAod<<std::endl;

    // Créer une nouvelle toile
    TCanvas* canvas = new TCanvas("canvas", "Hcal Tower Isolation", 800, 600);

    // Activer l'échelle logarithmique sur l'axe des y
    canvas->SetLogy();

    // Dessiner le premier histogramme
    histo1->Draw();
    histo1->SetStats(0);
    histo1->SetTitle("Hcal Tower Isolation");
    histo1->GetXaxis()->SetTitle("Isolation [GeV]");
    histo1->GetYaxis()->SetTitle("Events (log)");

    // Superposer le deuxième histogramme
    histo2->Draw("SAME");
    histo2->SetStats(0);

    // Ajouter une légende
    TLegend* legend = new TLegend(0.6, 0.7, 0.8, 0.8);
    legend->AddEntry(histo1, "Recomputed Isolation", "l");
    legend->AddEntry(histo2, "MiniAod value", "l");
    legend->SetBorderSize(0);
    legend->SetTextSize(0.04);
    legend->SetLineWidth(5);
    legend->Draw();

    // Afficher la toile
    canvas->Update();
    canvas->SaveAs(histLoc+".png");
    canvas->SaveAs(histLoc+".pdf");

    // Attendre que l'utilisateur ferme la fenêtre graphique
    canvas->WaitPrimitive();
}

void plotsGen1(TString histLoc) {
    // Ouvrir le fichier ROOT
    TFile* file = new TFile("../python/mc2024miniAod.root", "READ");

    // Accéder au dossier "demo"
    TDirectory* demoFolder = file->GetDirectory("demo");
    TList* keys = demoFolder->GetListOfKeys();

    //Parcourir la liste des clés et afficher les noms des objets
    TIter next(keys);
    TKey* key;
    while ((key = dynamic_cast<TKey*>(next()))) {
        TObject* obj = key->ReadObj();
        std::cout << "Nom de l'objet : " << obj->GetName() << "Et la classe :"<<obj->ClassName()<< std::endl;
    }

    // Lire les histogrammes à l'intérieur du dossier
    TH1D* histo1 = dynamic_cast<TH1D*>(demoFolder->Get(histLoc));
    histo1->SetLineColor(kBlue);

    // Créer une nouvelle toile
    TCanvas* canvas = new TCanvas("canvas", "Neutral Isolation (No cut)", 800, 600);

    // Activer l'échelle logarithmique sur l'axe des y
    canvas->SetLogy();

    // Dessiner le premier histogramme
    histo1->Draw();
    histo1->SetStats(0);
    histo1->SetTitle("Neutral Isolation");
    histo1->GetXaxis()->SetTitle("Isolation");
    histo1->GetYaxis()->SetTitle("Events (log)");

    // Ajouter une légende
    TLegend* legend = new TLegend(0.6, 0.7, 0.8, 0.8);
    legend->AddEntry(histo1, "RecHit", "l");
    legend->SetBorderSize(0);
    legend->SetTextSize(0.04);
    legend->SetLineWidth(5);
    legend->Draw();

    // Afficher la toile
    canvas->Update();
    canvas->SaveAs(histLoc+".png");
    canvas->SaveAs(histLoc+".pdf");

    // Attendre que l'utilisateur ferme la fenêtre graphique
    canvas->WaitPrimitive();
}

void plotsGenList(std::vector <TString> list, std::vector <TString> legendname, TString title) {
    // Ouvrir le fichier ROOT
    EColor color[5] = {kBlue, kRed, kGreen, kBlack, kCyan};
    TFile* file = new TFile(rootFileName, "READ");

    // Accéder au dossier "demo"
    TDirectory* demoFolder = file->GetDirectory("demo");
    TList* keys = demoFolder->GetListOfKeys();

    //Parcourir la liste des clés et afficher les noms des objets
    TIter next(keys);
    TKey* key;
    while ((key = dynamic_cast<TKey*>(next()))) {
        TObject* obj = key->ReadObj();
        std::cout << "Nom de l'objet : " << obj->GetName() << "Et la classe :"<<obj->ClassName()<< std::endl;
    }

    std::vector <TH1D*> histo;

    for(int i=0; i<list.size(); i++){
        // Lire les histogrammes à l'intérieur du dossier
        std::cout<<"Doing "<<list[i]<<std::endl;
        histo.push_back(dynamic_cast<TH1D*>(demoFolder->Get(list[i])));
        histo[i]->SetLineColor(color[i]);

        // std::cout<<"Histo "<<list[i]<<"integral is : "<<histo[i]->Integral()<<std::endl;
    }

    double maxVal = 0;
    for (int i = 0; i < histo.size(); i++) {
        double histMax = histo[i]->GetMaximum();
        if (histMax > maxVal) {
            maxVal = histMax;
        }
    }

    // Créer une nouvelle toile
    TCanvas* canvas = new TCanvas("canvas", "Hcal Tower Isolation", 800, 600);

    // Activer l'échelle logarithmique sur l'axe des y
    canvas->SetLogy();

    // Dessiner le premier histogramme
    TH1D* histo1 = histo[0];
    histo1->Draw("HIST");
    histo1->SetStats(0);
    histo1->SetTitle(title);
    histo1->GetXaxis()->SetTitle("Isolation [GeV]");
    histo1->GetYaxis()->SetTitle("Events (log)");
    histo1->SetMaximum(maxVal * 3);
    histo1->SetFillStyle(3001);  // Style de remplissage avec transparence (3001 est un style de remplissage)
    histo1->SetFillColorAlpha(color[0], 0.2);
    // histo1->SetFillColor(color[0]); // Remplit avec la couleur spécifiée
    TLegend* legend = new TLegend(0.4, 0.75, 0.9, 0.85);
    legend->SetFillStyle(0);   // Transparence du fond
    legend->SetTextFont(42);   // Police claire et lisible
    legend->SetLineWidth(30);           // Augmente l'épaisseur des traits
    legend->SetLineStyle(1);           // Style de ligne plein
    legend->AddEntry(histo1, legendname[0], "l");

    // Superposer le deuxième histogramme
    for (int i=1; i<histo.size(); i++){
        histo[i]->Draw("HIST SAME");
        histo[i]->SetStats(0);
        histo[i]->SetFillStyle(3001);  // Style de remplissage avec transparence (3001 est un style de remplissage)
        histo[i]->SetFillColorAlpha(color[i], 0.2);
        legend->AddEntry(histo[i], legendname[i], "l");
    }

    // Ajouter une légende
    legend->SetBorderSize(0);
    legend->SetTextSize(0.04);
    legend->SetLineWidth(5);
    legend->Draw();

    // Afficher la toile
    canvas->Update();
    canvas->SaveAs("StoredVsComputed/"+year+"_"+title+".png");
    canvas->SaveAs("StoredVsComputed/"+year+"_"+title+".pdf");
    canvas->SaveAs("StoredVsComputed/"+year+"_"+title+".svg");

    // Attendre que l'utilisateur ferme la fenêtre graphique
    canvas->WaitPrimitive();
}


void plots(){
    // std::cout<<"Starting"<<std::endl;
    // plotsGen("PfNeutralIso", "PfNeutralAodIso");
    // plotsGen("HcalNeutralIso", "HcalNeutralAodIso");
    // plotsGen1("NofRecHit");
    std::cout<<"Starting Hcal Iso"<<std::endl;
    std::vector<TString> name = {"HcalTowerIsodr03", "HcalIsoThresholddr03", "HcalNeutralAodIso"};
    std::vector<TString> legend = {"Recomputed without threshold", "Recomputed with threshold", "Inbuilt MiniAOD value"};
    TString title = "HcalTowerIsolation";
    plotsGenList(name, legend, title);

    // std::cout<<"Starting RecHits ET"<<std::endl;
    // std::vector<TString> name2 = {"HB_Et", "HEDepth1Et", "HEDepth2Et"};
    // std::vector<TString> legend2 = {"Barrel Hcal RecHits' ET", "Depth 1 Endcap Hcal RecHits' ET", "Depth > 1 Endcap Hcal RecHits' ET"};
    // TString title2 = "RecHitsET";
    // plotsGenList(name2, legend2, title2);

    // std::cout<<"Starting Pf Neutral Iso"<<std::endl;
    // std::vector<TString> name3 = {"PfNeutralIso", "PfNeutralAodIso"};
    // std::vector<TString> legend3 = {"Pf Neutral Iso (recomputed)", "pf Neutral Iso (stored)"};
    // TString title3 = "PfNeutralIso";
    // plotsGenList(name3, legend3, title3);
    
}
