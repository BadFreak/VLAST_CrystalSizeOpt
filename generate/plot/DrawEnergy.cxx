#include <iostream>
#include <vector>
#include <TFile.h>
#include <TTree.h>
#include <TH1D.h>
#include <TCanvas.h>

using namespace std;
void DrawEnergy() {
	string inFileName = "../root/gamma/1000MeV/1.root";
	TFile* file = TFile::Open(inFileName.c_str(), "READ");
    if (!file || file->IsZombie()) {
        std::cerr << "Error: Cannot open the file!" << std::endl;
        return 1;
    }

    TTree* tree = (TTree*)file->Get("MC_Truth");  // 替换为实际的 TTree 名称
    if (!tree) {
        std::cerr << "Error: Cannot find the TTree!" << std::endl;
        return 1;
    }
    
	std::vector<double>* branchData = nullptr;  // 假设 branch 类型为 std::vector<double>
    tree->SetBranchAddress("Hit_Energy", &branchData);  // 替换为实际的 branch 名称

    // 检查事件总数
	TH1D * h1 = new TH1D("h1","h1",100,0,1000);
    Long64_t nEntries = tree->GetEntries();
	for (int i = 0; i < nEntries; i++) {
		if (i%10000==0) cout << "Entry : " << i << endl;
    	tree->GetEntry(i);  
		double sum = 0.0;
		for (double value : *branchData) {
			sum += value;
    	}
		h1->Fill(sum);
	}
	TCanvas * c1 = new TCanvas();
	c1->cd();
	h1->Draw();

}

