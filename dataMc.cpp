#include <iostream>
#include <sstream>
#include <TFile.h>
#include <TH1.h>
#include <TCanvas.h>
#include <TLegend.h>
#include <TPad.h>
#include <TLatex.h>
#include <TLine.h>
#include <TString.h>

void plotTwoHistograms(const char* fileMC_name, const char* fileData_name, const char* histMC_path, const char* histData_path, float lumi, TString era, const char* xtitle, const char* htitle, float xsec) {
    TFile fileMC(fileMC_name);
    TFile fileData(fileData_name);

    // TDirectory* demoFolder = file->GetDirectory("demo");
    // TList* keys = demoFolder->GetListOfKeys();
    // TIter next(keys);
    // TKey* key;
    // while ((key = dynamic_cast<TKey*>(next()))) {
    //     TObject* obj = key->ReadObj();
    //     std::cout << "Nom de l'objet : " << obj->GetName() << ". Classe :"<<obj->ClassName()<< std::endl;
    // }

    TH1D *histMC = (TH1D*)fileMC.Get(histMC_path);
    TH1D *histData = (TH1D*)fileData.Get(histData_path);

    // histMC->Rebin(histMC->GetNbinsX()/30);
    // histData->Rebin(histData->GetNbinsX()/30);
    std::cout<<histMC->GetName()<<std::endl;

    if(strcmp(histMC->GetName(), "Mll") == 0){
        histMC->GetXaxis()->SetRangeUser(75, 105);
        histData->GetXaxis()->SetRangeUser(75, 105);
    }
    
    if(strcmp(histMC->GetName(), "hnPV") == 0){
        histMC->Scale(1/histMC->Integral());
        histData->Scale(1/histData->Integral());
    }
    else{
        histMC->Scale(xsec*lumi);
    }

    // histMC->GetXaxis()->SetRangeUser(70, 110);
    // histData->GetXaxis()->SetRangeUser(70, 110);

    histMC->SetStats(0);
    histData->SetStats(0);

    TH1D *ratioHist = new TH1D("Ratio Plot", "Ratio Plot", histMC->GetNbinsX(), histMC->GetXaxis()->GetXmin(), histMC->GetXaxis()->GetXmax());

    ratioHist->GetYaxis()->SetTitle("Ratio");

    for (int i = 1; i <= histMC->GetNbinsX(); ++i) {
        double binContenthistMC = histMC->GetBinContent(i);
        double binContenthistData = histData->GetBinContent(i);
        double binErrorhistData = histData->GetBinError(i);
        if (binContenthistData != 0 && binContenthistMC!=0) {
            ratioHist->SetBinContent(i, binContenthistData / binContenthistMC);
            ratioHist->SetBinError(i, binErrorhistData / binContenthistMC);
        }
    }

    TH1D *ratiohistData = new TH1D("ratiohistData", "Ratio histData/histData", histMC->GetNbinsX(), histMC->GetXaxis()->GetXmin(), histMC->GetXaxis()->GetXmax());

    for (int i = 1; i <= histMC->GetNbinsX(); ++i) {
        double binContenthistMC = histMC->GetBinContent(i);
        double binErrorhistMC = histMC->GetBinError(i);
        if (binContenthistMC != 0) {
            ratiohistData->SetBinContent(i, 1.0);
            ratiohistData->SetBinError(i, binErrorhistMC / binContenthistMC);
        }
    }

    TCanvas *canvas = new TCanvas("canvas", "Histogram Comparison", 800, 800);

    TPad *pad1 = new TPad("pad1", "pad1", 0, 0.3, 1, 1);
    TPad *pad2 = new TPad("pad2", "pad2", 0, 0, 1, 0.3);
    pad1->SetBottomMargin(0.02);
    pad2->SetTopMargin(0.02);
    pad2->SetBottomMargin(0.3); 
    pad1->Draw();
    pad2->Draw();

    
    pad1->cd();
    histMC->Draw("HIST");
    histMC->SetLineColor(kBlue);
    histMC->SetFillColor(kBlue);
    histMC->SetXTitle(xtitle);

    float binWidth = histMC->GetBinWidth(1);
    std::ostringstream oss;
    oss << "Events / " << binWidth<<" GeV";
    std::string formattedBinWidth = oss.str();
    histMC->SetYTitle((formattedBinWidth).c_str());

    histMC->GetXaxis()->SetLabelSize(0);
    histMC->SetTitle(htitle);
    histMC->GetYaxis()->SetTitleSize(0.06);
    histMC->GetYaxis()->SetTitleOffset(0.8);

    histData->SetLineColor(kBlack);
    histData->SetMarkerStyle(kFullDotLarge);
    histData->SetMarkerSize(1);
    histData->Draw("P0 SAME");
    

    /
    double max_value = std::max(histMC->GetMaximum(), histData->GetMaximum());
    histMC->SetMaximum(max_value * 1.1);
    histData->SetMaximum(max_value * 1.1);

    // Legend
    TLegend *legend = new TLegend(0.7, 0.7, 0.9, 0.9);
    legend->AddEntry(histMC, "D-Y", "l");
    std::stringstream legendName;
    legendName << "Data ("<< era << ")";
    legend->AddEntry(histData, legendName.str().c_str(), "P");
    legend->Draw("SAME");

    TLatex latex;
    latex.SetNDC();
    latex.SetTextSize(0.04);
    latex.DrawLatex(0.8, 0.92, Form("%.2f fb^{-1}", lumi));
    latex.DrawLatex(0.15, 0.92, "Private work");


    pad2->cd();
    // ratioHist->GetXaxis()->SetRangeUser(70, 110);
    // ratiohistData->GetXaxis()->SetRangeUser(70, 110);
    ratioHist->SetStats(0);
    ratiohistData->SetStats(0);

    ratioHist->SetXTitle(xtitle);
    ratioHist->GetXaxis()->SetTitleSize(0.12);
    ratioHist->GetXaxis()->SetLabelSize(0.1);  
    ratioHist->GetYaxis()->SetTitleSize(0.1);   
    ratioHist->GetYaxis()->SetLabelSize(0.08); 

    ratioHist->SetLineColor(kBlack);
    ratioHist->SetMarkerStyle(kFullDotLarge);
    ratioHist->SetMarkerSize(1);
    ratioHist->Draw("P0");
    ratioHist->SetMinimum(0); 
    TLine *line = new TLine(ratioHist->GetXaxis()->GetXmin(), 1.0, ratioHist->GetXaxis()->GetXmax(), 1.0);
    line->SetLineColor(kBlack);
    line->SetLineStyle(2); 
    line->Draw("same");

    ratiohistData->SetFillColor(kGray+2);  
    ratiohistData->SetFillStyle(1001);    
    ratiohistData->SetLineColor(kGray+2);  
    ratiohistData->Draw("E2 same");       

    if(strcmp(histMC->GetName(), "Mll") == 0){
        ratioHist->GetXaxis()->SetRangeUser(75, 105);
        ratiohistData->GetXaxis()->SetRangeUser(75, 105);
    }

    if(strcmp(histMC->GetName(), "HcalTowerIsoThresholddr02") == 0){
        ratioHist->SetMaximum(3);
        ratiohistData->SetMaximum(3);
    }


    std::stringstream outputDir;
    outputDir << era << "/";

    canvas->SaveAs((outputDir.str()+(std::string)histMC->GetName()+".pdf").c_str());
    canvas->SaveAs((outputDir.str()+(std::string)histMC->GetName()+".svg").c_str());

    delete ratioHist;
    delete ratiohistData;
    delete line;
    delete legend;
    delete canvas;
}

void plotTwoHistogramsEE(const char* fileMC_name, const char* fileMC_name_EE, const char* fileData_name, const char* fileData_name_EE, const char* histMC_path, const char* histData_path, float lumi, float lumi_EE, const char* xtitle, const char* htitle, TString era, float xsec) {
    TFile fileMC(fileMC_name);
    TFile fileData(fileData_name);
    TFile fileMC_EE(fileMC_name_EE);
    TFile fileData_EE(fileData_name_EE);

     // Print the name of the hists availables
    // TDirectory* demoFolder = file->GetDirectory("demo");
    // TList* keys = demoFolder->GetListOfKeys();
    // TIter next(keys);
    // TKey* key;
    // while ((key = dynamic_cast<TKey*>(next()))) {
    //     TObject* obj = key->ReadObj();
    //     std::cout << "Nom de l'objet : " << obj->GetName() << ". Classe :"<<obj->ClassName()<< std::endl;
    // }

    TH1D *histMC = (TH1D*)fileMC.Get(histMC_path);
    TH1D *histData = (TH1D*)fileData.Get(histData_path);
    TH1D *histMC_EE = (TH1D*)fileMC_EE.Get(histMC_path);
    TH1D *histData_EE = (TH1D*)fileData_EE.Get(histData_path);
    histMC->Scale(xsec*lumi);
    histMC_EE->Scale(xsec*lumi_EE);
    histMC->Add(histMC_EE);
    histData->Add(histData_EE);


    // histMC->Rebin(histMC->GetNbinsX()/30);
    // histData->Rebin(histData->GetNbinsX()/30);
    std::cout<<histMC->GetName()<<std::endl;

    if(strcmp(histMC->GetName(), "Mll") == 0){
        histMC->GetXaxis()->SetRangeUser(75, 105);
        histData->GetXaxis()->SetRangeUser(75, 105);
    }

    // histMC->GetXaxis()->SetRangeUser(70, 110);
    // histData->GetXaxis()->SetRangeUser(70, 110);

    histMC->SetStats(0);
    histData->SetStats(0);

    TH1D *ratioHist = new TH1D("Ratio Plot", "Ratio Plot", histMC->GetNbinsX(), histMC->GetXaxis()->GetXmin(), histMC->GetXaxis()->GetXmax());

    ratioHist->GetYaxis()->SetTitle("Ratio");

    for (int i = 1; i <= histMC->GetNbinsX(); ++i) {
        double binContenthistMC = histMC->GetBinContent(i);
        double binContenthistData = histData->GetBinContent(i);
        double binErrorhistData = histData->GetBinError(i);
        if (binContenthistData != 0) {
            ratioHist->SetBinContent(i, binContenthistData / binContenthistMC);
            ratioHist->SetBinError(i, binErrorhistData / binContenthistMC);
        }
    }

    TH1D *ratiohistData = new TH1D("ratiohistData", "Ratio histData/histData", histMC->GetNbinsX(), histMC->GetXaxis()->GetXmin(), histMC->GetXaxis()->GetXmax());

    for (int i = 1; i <= histMC->GetNbinsX(); ++i) {
        double binContenthistMC = histMC->GetBinContent(i);
        double binErrorhistMC = histMC->GetBinError(i);
        if (binContenthistMC != 0) {
            ratiohistData->SetBinContent(i, 1.0);
            ratiohistData->SetBinError(i, binErrorhistMC / binContenthistMC);
        }
    }

    TCanvas *canvas = new TCanvas("canvas", "Histogram Comparison", 800, 800);

    TPad *pad1 = new TPad("pad1", "pad1", 0, 0.3, 1, 1);
    TPad *pad2 = new TPad("pad2", "pad2", 0, 0, 1, 0.3);
    pad1->SetBottomMargin(0.02); 
    pad2->SetTopMargin(0.02);    
    pad2->SetBottomMargin(0.3); 
    pad1->Draw();
    pad2->Draw();

    
    pad1->cd();
    histMC->Draw("HIST");
    histMC->SetLineColor(kBlue);
    histMC->SetFillColor(kBlue);
    histMC->SetXTitle(xtitle);

    float binWidth = histMC->GetBinWidth(1);
    std::ostringstream oss;
    oss << "Events / " << binWidth<<" GeV";
    std::string formattedBinWidth = oss.str();
    histMC->SetYTitle((formattedBinWidth).c_str());

    histMC->GetXaxis()->SetLabelSize(0);
    histMC->SetTitle(htitle);
    histMC->GetYaxis()->SetTitleSize(0.06);
    histMC->GetYaxis()->SetTitleOffset(0.8);

    histData->SetLineColor(kBlack);
    histData->SetMarkerStyle(kFullDotLarge);
    histData->SetMarkerSize(1);
    histData->Draw("P0 SAME");
    

    double max_value = std::max(histMC->GetMaximum(), histData->GetMaximum());
    histMC->SetMaximum(max_value * 1.1);
    histData->SetMaximum(max_value * 1.1);

    // Legend
    TLegend *legend = new TLegend(0.7, 0.7, 0.9, 0.9);
    legend->AddEntry(histMC, "D-Y", "l");
    std::stringstream legendName;
    legendName << "Data ("<<era<<")";
    legend->AddEntry(histData, legendName.str().c_str(), "P");
    legend->Draw("SAME");

    TLatex latex;
    latex.SetNDC();
    latex.SetTextSize(0.04);
    latex.DrawLatex(0.8, 0.92, Form("%.2f fb^{-1}", lumi+lumi_EE));
    latex.DrawLatex(0.15, 0.92, "Private work");


    pad2->cd();
    // ratioHist->GetXaxis()->SetRangeUser(70, 110);
    // ratiohistData->GetXaxis()->SetRangeUser(70, 110);
    ratioHist->SetStats(0);
    ratiohistData->SetStats(0);

    ratioHist->SetXTitle(xtitle);
    ratioHist->GetXaxis()->SetTitleSize(0.12);
    ratioHist->GetXaxis()->SetLabelSize(0.1);  
    ratioHist->GetYaxis()->SetTitleSize(0.1);   
    ratioHist->GetYaxis()->SetLabelSize(0.08);  

    ratioHist->SetLineColor(kBlack);
    ratioHist->SetMarkerStyle(kFullDotLarge);
    ratioHist->SetMarkerSize(1);
    ratioHist->Draw("P0");
    ratioHist->SetMaximum(2.0); 
    TLine *line = new TLine(ratioHist->GetXaxis()->GetXmin(), 1.0, ratioHist->GetXaxis()->GetXmax(), 1.0);
    line->SetLineColor(kBlack);
    line->SetLineStyle(2);
    line->Draw("same");

    ratiohistData->SetFillColor(kGray+2); 
    ratiohistData->SetFillStyle(1001);    
    ratiohistData->SetLineColor(kGray+2);  
    ratiohistData->Draw("E2 same");       

    if(strcmp(histMC->GetName(), "Mll") == 0){
        ratioHist->GetXaxis()->SetRangeUser(75, 105);
        ratiohistData->GetXaxis()->SetRangeUser(75, 105);
    }


    std::stringstream outputDir;
    outputDir << era << "/";

    canvas->SaveAs((outputDir.str()+(std::string)histMC->GetName()+".pdf").c_str());
    canvas->SaveAs((outputDir.str()+(std::string)histMC->GetName()+".svg").c_str());

    delete ratioHist;
    delete ratiohistData;
    delete line;
    delete legend;
    delete canvas;
}

int dataMc(TString era) {

    // EE MC sample not found
    // const char* fileMC_name = "/eos/lyoeos.in2p3.fr/grid/cms/store/user/damram/DYto2L_M-50_TuneCP5_13p6TeV_pythia8/EGamma_MC_22_DY/241009_185114/mc2024_run2022GminiAod_reducedEgamma.root";
    // const char* fileMC_name_EE = "/eos/lyoeos.in2p3.fr/grid/cms/store/user/damram/DYto2L_M-50_TuneCP5_13p6TeV_pythia8/EGamma_MC_22_DY/241009_185114/mc2024_run2022GminiAod_reducedEgamma.root";
    
    // const char* fileMC_name = "/eos/lyoeos.in2p3.fr/grid/cms/store/user/damram/DYto2E_M-50_NNPDF31_TuneCP5_13p6TeV-powheg-pythia8/EGamma_MC_22_DYE/241009_185941/mc2024_run2022GminiAod_reducedEgamma.root";
    // const char* fileMC_name_EE = "/eos/lyoeos.in2p3.fr/grid/cms/store/user/damram/DYto2E_M-50_NNPDF31_TuneCP5_13p6TeV-powheg-pythia8/EGamma_MC_22_DYE/241009_185941/mc2024_run2022GminiAod_reducedEgamma.root";

    // const char* fileMC_name = "/eos/lyoeos.in2p3.fr/grid/cms/store/user/damram/DYTo2L_MLL-50_TuneCP5_13p6TeV_pythia8/EGamma_MC_22G_TTbar/241008_223809/mc2024_run2022GminiAod_reducedEgamma.root";
    // const char* fileMC_name_EE = "/eos/lyoeos.in2p3.fr/grid/cms/store/user/damram/DYTo2L_MLL-50_TuneCP5_13p6TeV_pythia8/EGamma_MC_22G_TTbar/241008_223809/mc2024_run2022GminiAod_reducedEgamma.root";
    
    // const char* fileMC_name = "/eos/lyoeos.in2p3.fr/grid/cms/store/user/damram/DYJetsToLL_M-50_TuneCP5_13p6TeV-madgraphMLM-pythia8/EGamma_MC_22_DY_EE/241014_134523/mc2024_run2022GminiAod_reducedEgamma.root";
    // const char* fileMC_name_EE = "/eos/lyoeos.in2p3.fr/grid/cms/store/user/damram/DYJetsToLL_M-50_TuneCP5_13p6TeV-madgraphMLM-pythia8/EGamma_MC_22_DY/241014_132916/mc2024_run2022GminiAod_reducedEgamma.root";    
    
    float xsec = 2244; //xsdb
    // xsec=2067.5; //Atlas

    //Run2022
    const char* fileMC_name_pre_22 = "/eos/lyoeos.in2p3.fr/grid/cms/store/user/damram/DYto2E_M-50_NNPDF31_TuneCP5_13p6TeV-powheg-pythia8/DYto2E_M-50_13p6TeV_Run3Summer22MiniAODv4/241113_152412/mc.root";
    // fileMC_name_pre_22 = "/eos/lyoeos.in2p3.fr/grid/cms/store/user/damram/DYto2E_M-50_NNPDF31_TuneCP5_13p6TeV-powheg-pythia8/test2023DY/241101_115030/mc.root";
    const char* fileMC_name_post_22 = "/eos/lyoeos.in2p3.fr/grid/cms/store/user/damram/DYto2E_M-50_NNPDF31_TuneCP5_13p6TeV-powheg-pythia8/DYto2E_M-50_13p6TeV_Run3Summer22EEMiniAODv4/241113_152413/mc.root";
    const char* fileData_name_pre_22 = "/eos/lyoeos.in2p3.fr/grid/cms/store/user/damram/EGamma/2022preEE/241113.root";
    const char* fileData_name_post_22 = "/eos/lyoeos.in2p3.fr/grid/cms/store/user/damram/EGamma/2022postEE/241113.root";

    //Run2023
    const char* fileMC_name_pre_23 = "/eos/lyoeos.in2p3.fr/grid/cms/store/user/damram/DYto2E_MLL-50to120_TuneCP5_13p6TeV_powheg-pythia8/DY_2023/241029_131010/mc.root";
    const char* fileMC_name_post_23 = "/eos/lyoeos.in2p3.fr/grid/cms/store/user/damram/DYto2E_MLL-50to120_TuneCP5_13p6TeV_powheg-pythia8/DY_2023_BPix/241029_113257/mc.root";
    const char* fileData_name_pre_23 = "/eos/lyoeos.in2p3.fr/grid/cms/store/user/damram/EGamma/2023preBPix/241029.root";
    const char* fileData_name_post_23 = "/eos/lyoeos.in2p3.fr/grid/cms/store/user/damram/EGamma/2023postBPix/241029.root";

    //Run2022 lumi
    float lumi_22 = 34.6521; // should not be used
    float lumi_pre_22 = 7.9804;
    float lumi_post_22 = 26.6717;

    //Run2023 lumi
    float lumi_23 = 27.245; //should not be used
    float lumi_pre_23 = 17.794;
    float lumi_post_23 = 9.451;

    bool EE=false;
    bool BPix=false;

    //Generic name for function
    const char* fileData_name_pre;
    const char* fileData_name_post;
    const char* fileMC_name_pre;
    const char* fileMC_name_post;
    const char* fileData_name;
    const char* fileMC_name;
    float lumi;
    float lumi_pre;
    float lumi_post;

    // Forget ERA plot
    // if(era=="Run2022C"){
    //     std::cout<<"Do not use era per era. Not implemented correctly"<<std::endl;
    //     fileData_name = "/eos/lyoeos.in2p3.fr/grid/cms/store/user/damram/EGamma/Run2022C_22Sep2023/241024_150520/data.root";
    //     lumi = 5.0104;
    //     }
    // else if (era=="Run2022D")
    // {
    //     std::cout<<"Do not use era per era. Not implemented correctly"<<std::endl;
    //     fileData_name = "/eos/lyoeos.in2p3.fr/grid/cms/store/user/damram/EGamma/Run2022D_22Sep2023/241024_200358/data.root";
    //     lumi = 2.9700;
    // }
    // else if (era=="Run2022E"){
    //     std::cout<<"Do not use era per era. Not implemented correctly"<<std::endl;
    //     fileData_name = "/eos/lyoeos.in2p3.fr/grid/cms/store/user/damram/EGamma/Run2022E_22Sep2023/241024_200916/data.root";
    //     lumi = 5.8070;
    //     EE=true;
    // }
    // else if (era=="Run2022E"){
    //     std::cout<<"Do not use era per era. Not implemented correctly"<<std::endl;
    //     fileData_name = "/eos/lyoeos.in2p3.fr/grid/cms/store/user/damram/EGamma/Run2022F_22Sep2023/241024_201658/data.root";
    //     lumi = 17.7818;
    //     EE=true;
    // }
    // else if (era=="Run2022G"){
    //     std::cout<<"Do not use era per era. Not implemented correctly"<<std::endl;
    //     fileData_name = "/eos/lyoeos.in2p3.fr/grid/cms/store/user/damram/EGamma/Run2022G_22Sep2023/241024_202015/data.root";
    //     lumi = 3.0828;
    //     EE=true;
    // }
    if(era=="Run2022preEE"){
        fileData_name=fileData_name_pre_22;
        fileMC_name=fileMC_name_pre_22;
        lumi = lumi_pre_22;
    }
    else if(era=="Run2022postEE"){
        fileData_name = fileData_name_post_22;
        fileMC_name=fileMC_name_post_22;
        lumi = lumi_post_22;
        EE=true;
    }
    else if(era=="Run2023preBPix"){
        fileData_name = fileData_name_pre_23;
        fileMC_name=fileMC_name_pre_23;
        lumi = lumi_pre_23;
    }
    else if(era=="Run2023postBPix"){
        fileData_name = fileData_name_post_23;
        fileMC_name=fileMC_name_post_23;
        lumi = lumi_post_23;
        BPix=true;
    }
    else if(era=="Run2022"){
        fileData_name_pre = fileData_name_pre_22;
        fileData_name_post = fileData_name_post_22;
        fileMC_name_pre = fileMC_name_pre_22;
        fileMC_name_post = fileMC_name_post_22;
        lumi_pre=lumi_pre_22;
        lumi_post=lumi_post_22;
    }
    else if (era=="Run2023"){
        fileData_name_pre = fileData_name_pre_23;
        fileData_name_post = fileData_name_post_23;
        fileMC_name_pre = fileMC_name_pre_23;
        fileMC_name_post = fileMC_name_post_23;
        lumi_pre=lumi_pre_23;
        lumi_post=lumi_post_23;
    }
    else{
    std::cout<<"Not implemented yet."<<std::endl;
    return -1;
    }

    const char* hist_paths[] = {"demo/HcalTowerIsodr03", "demo/HcalTowerIsodr04", "demo/HcalIsoThresholddr03", "demo/HcalTowerIsoThresholddr04", "demo/Mll", "demo/IsoHBDepth1", "demo/IsoHBDepth2", "demo/IsoHBDepth3", "demo/IsoHBDepth4", "demo/IsoHEDepth1", "demo/IsoHEDepth2", "demo/IsoHEDepth3", "demo/IsoHEDepth4", "demo/IsoHEDepth5", "demo/IsoHEDepth6", "demo/IsoHEDepth7", "demo/PfNeutralIso", "demo/HB_Energy", "demo/HcalNeutralAodIso", "demo/HcalTowerIsoThresholddr02", "demo/hnPV", "demo/HoverE", "demo/relIsoEA"};

    const char* hist_titles[] = {"Hcal Tower Isolation", "Hcal Tower Isolation", "Hcal Tower Isolation (Threshold)", "Hcal Tower Isolation (Threshold)", "Invariant mass M(ee)", "Hcal Isolation Barrel depth1", "Hcal Isolation Barrel depth2", "Hcal Isolation Barrel depth3", "Hcal Isolation Barrel depth4", "Hcal Isolation Endcap depth1", "Hcal Isolation Endcap depth2", "Hcal Isolation Endcap depth3", "Hcal Isolation Endcap depth4", "Hcal Isolation Endcap depth5", "Hcal Isolation Endcap depth6", "Hcal Isolation Endcap depth7", "Pf Neutral Iso", "Barrel RecHits Energy", "Hcal Inbuilt Isolation", "Hcal Tower Isolation", "Number of vertex", "H/E", "relIsoEA"};

    const char* x_titles[] = {"Hcal Isolation [GeV]", "Hcal Isolation [GeV]", "Hcal Isolation [GeV]", "Hcal Isolation [GeV]", "Invariant mass [GeV]", "Hcal Isolation [GeV]", "Hcal Isolation [GeV]", "Hcal Isolation [GeV]", "Hcal Isolation [GeV]", "Hcal Isolation [GeV]", "Hcal Isolation [GeV]", "Hcal Isolation [GeV]", "Hcal Isolation [GeV]", "Hcal Isolation [GeV]", "Hcal Isolation [GeV]", "Hcal Isolation [GeV]", "Pf Neutral Iso [GeV]", "Barrel RecHits Energy [GeV]", "Hcal Isolation [GeV]", "Hcal Isolation [GeV]", "Number of vertex", "H/E", "relIsoEA"};
    
    const int num_hist = sizeof(hist_paths) / sizeof(hist_paths[0]);

    for (int i = 0; i < num_hist; ++i) {
        if (era == "Run2022" || era == "Run2023") {
            plotTwoHistogramsEE(fileMC_name_pre, fileMC_name_post, fileData_name_pre, fileData_name_post, hist_paths[i], hist_paths[i], lumi_pre, lumi_post, x_titles[i], hist_titles[i], era, xsec);
        } else {
            plotTwoHistograms(fileMC_name, fileData_name, hist_paths[i], hist_paths[i], lumi, era, x_titles[i], hist_titles[i], xsec);
        }
    }
    
    // const char* histMC_path = "demo/Mll";
    // const char* histData_path = "demo/Mll";
    // const char* histMC_path = "demo/HcalTowerIsodr03";
    // const char* histData_path = "demo/HcalTowerIsodr03";

    // plotTwoHistograms(fileMC_name, fileData_name, histMC_path, histData_path, lumi, era, "Isolation [GeV]", "Hcal Tower Isolation\n with threshold");

    // histMC_path = "demo/HcalTowerIsodr04";
    // histData_path = "demo/HcalTowerIsodr04";

    // plotTwoHistograms(fileMC_name, fileData_name, histMC_path, histData_path, lumi, era, "Isolation [GeV]", "Hcal Tower Isolation");

    // histMC_path = "demo/HcalTowerIsodr028";
    // histData_path = "demo/HcalTowerIsodr028";

    // plotTwoHistograms(fileMC_name, fileData_name, histMC_path, histData_path, lumi, era, "Isolation [GeV]", "Hcal Tower Isolation");

    // histMC_path = "demo/HcalTowerIsodr026";
    // histData_path = "demo/HcalTowerIsodr026";

    // plotTwoHistograms(fileMC_name, fileData_name, histMC_path, histData_path, lumi, era, "Isolation [GeV]", "Hcal Tower Isolation");

    // histMC_path = "demo/HcalTowerIsodr024";
    // histData_path = "demo/HcalTowerIsodr024";

    // plotTwoHistograms(fileMC_name, fileData_name, histMC_path, histData_path, lumi, era, "Isolation [GeV]", "Hcal Tower Isolation");

    // histMC_path = "demo/HcalTowerIsodr022";
    // histData_path = "demo/HcalTowerIsodr022";

    // plotTwoHistograms(fileMC_name, fileData_name, histMC_path, histData_path, lumi, era, "Isolation [GeV]", "Hcal Tower Isolation");

    // histMC_path = "demo/HcalTowerIsodr02";
    // histData_path = "demo/HcalTowerIsodr02";

    // plotTwoHistograms(fileMC_name, fileData_name, histMC_path, histData_path, lumi, era, "Isolation [GeV]", "Hcal Tower Isolation");

    // histMC_path = "demo/HcalIsoThresholddr03";
    // histData_path = "demo/HcalIsoThresholddr03";

    // plotTwoHistograms(fileMC_name, fileData_name, histMC_path, histData_path, lumi, era, "Isolation [GeV]", "Hcal Tower Isolation");

    // histMC_path = "demo/HcalTowerIsoThresholddr04";
    // histData_path = "demo/HcalTowerIsoThresholddr04";

    // plotTwoHistograms(fileMC_name, fileData_name, histMC_path, histData_path, lumi, era, "Isolation [GeV]", "Hcal Tower Isolation");

    // histMC_path = "demo/HcalTowerIsoThresholddr028";
    // histData_path = "demo/HcalTowerIsoThresholddr028";

    // plotTwoHistograms(fileMC_name, fileData_name, histMC_path, histData_path, lumi, era, "Isolation [GeV]", "Hcal Tower Isolation");

    // histMC_path = "demo/HcalTowerIsoThresholddr026";
    // histData_path = "demo/HcalTowerIsoThresholddr026";

    // plotTwoHistograms(fileMC_name, fileData_name, histMC_path, histData_path, lumi, era, "Isolation [GeV]", "Hcal Tower Isolation");

    // histMC_path = "demo/HcalTowerIsoThresholddr024";
    // histData_path = "demo/HcalTowerIsoThresholddr024";

    // plotTwoHistograms(fileMC_name, fileData_name, histMC_path, histData_path, lumi, era, "Isolation [GeV]", "Hcal Tower Isolation");

    // histMC_path = "demo/HcalTowerIsoThresholddr022";
    // histData_path = "demo/HcalTowerIsoThresholddr022";

    // plotTwoHistograms(fileMC_name, fileData_name, histMC_path, histData_path, lumi, era, "Isolation [GeV]", "Hcal Tower Isolation");

    // histMC_path = "demo/HcalTowerIsoThresholddr02";
    // histData_path = "demo/HcalTowerIsoThresholddr02";

    // plotTwoHistograms(fileMC_name, fileData_name, histMC_path, histData_path, lumi, era, "Isolation [GeV]", "Hcal Tower Isolation");

    // histMC_path = "demo/PfNeutralIso";
    // histData_path = "demo/PfNeutralIso";

    // plotTwoHistograms(fileMC_name, fileData_name, histMC_path, histData_path, lumi, era, "Neutral Isolation [GeV]", "Pf Neutral Isolation");

    // histMC_path = "demo/HB_Energy";
    // histData_path = "demo/HB_Energy";

    // plotTwoHistograms(fileMC_name, fileData_name, histMC_path, histData_path, lumi, era, "Et [GeV]", "Barrel RecHits Et");

    // histMC_path = "demo/HEDepth1Energy";
    // histData_path = "demo/HEDepth1Energy";

    // plotTwoHistograms(fileMC_name, fileData_name, histMC_path, histData_path, lumi, era, "Et [GeV]", "Endcap RecHits Et (depth 1)");

    // histMC_path = "demo/HEDepth2Energy";
    // histData_path = "demo/HEDepth2Energy";

    // plotTwoHistograms(fileMC_name, fileData_name, histMC_path, histData_path, lumi, era, "Et [GeV]", "Endcap RecHits Et (depth > 1)");

    // histMC_path = "demo/Mll";
    // histData_path = "demo/Mll";

    // plotTwoHistograms(fileMC_name, fileData_name, histMC_path, histData_path, lumi, era, "Invariant mass [GeV]", "Invariant mass M(ee)");

    return 0;
}
