#include <iostream>
#include <vector>
#include <TFile.h>
#include <TTree.h>
#include <TH1D.h>
#include <TCanvas.h>

const string crystalType = "4_4";
vector<int> inEnergy = {50, 100, 200, 300, 500, 1000, 2000, 3000, 4000, 5000};

using namespace std;

double FitHist(TH1D * &h, TF1 * &f1, TF1 * &f2) {
	double norm = h->GetBinContent(h->GetMaximumBin());
	double mean = h->GetMean();
	double rms = h->GetRMS();
	double leftLimit = mean - 10 * rms;
	double rightLimit = mean + 10 * rms;
	f1->SetParameters(norm, mean, rms);
	h->Fit(f1, "0QR", "", leftLimit, rightLimit);
	
	norm = f1->GetParameter(0);
	mean = f1->GetParameter(1);
	rms = f1->GetParameter(2);
	leftLimit = mean - 10 * rms;
	rightLimit = mean + 3 * rms;
	f2->SetParameters(norm, mean, rms, 1, 10000);
	h->Fit(f2, "R", "", leftLimit, rightLimit);

	mean = f2->GetParameter(1);
	rms = f2->GetParameter(2);
	double result = rms / mean;
	return result;
}

void DrawEnergy() {
	//=============================================================
	vector<string> inFileName = {};
	vector<double> energyResolution = {};
	for (int i = 0; i < inEnergy.size(); ++i) {
		string tempName = "../result/" + crystalType + "/gamma_" + to_string(inEnergy[i]) + "MeV_10000_" + crystalType + ".root";
		inFileName.push_back(tempName);
	}
	const int inFileNum = inEnergy.size();
	//=====================Read Data===============================
	TH1D * hEnergy[inFileNum];
	TGraph * grEnergyResolution = new TGraph();
	grEnergyResolution->SetName("grER");
	TF1 * fGaus[inFileNum];
	TF1 * fCry[inFileNum];
	string outFileName = "Energy_Resolution_" + crystalType + ".root";
	TFile * outFile = new TFile(outFileName.c_str(), "RECREATE");
	for (int i_file = 0; i_file < inFileNum; ++i_file) {
	//for (int i_file = 4; i_file < 5; ++i_file) {
		const int primaryEnergy = inEnergy[i_file];
		int binLeft = 0.5 * primaryEnergy;
		int binRight = 1.05 * primaryEnergy;
		int binNum = primaryEnergy > 200 ? 200 : 1000;
	    hEnergy[i_file] = new TH1D(Form("hEnergy_%d", inEnergy[i_file]), "Energy", binNum, binLeft, binRight);
		fGaus[i_file] = new TF1(Form("fGaus_%d", i_file), "gaus", binLeft, binRight);
		fCry[i_file] = new TF1(Form("fCry_%d", i_file), "crystalball", binLeft, binRight);
		
		TFile* file = TFile::Open(inFileName.at(i_file).c_str(), "READ");
		if (!file || file->IsZombie()) {
			std::cerr << "Error: Cannot open the file!" << std::endl;
			return 1;
		}

		TTree* tree = (TTree*)file->Get("MC_Truth");  // 替换为实际的 TTree 名称
		if (!tree) {
			std::cerr << "Error: Cannot find the TTree!" << std::endl;
			return 1;
		}
		
		std::vector<double>* _hitE = nullptr;  // 假设 branch 类型为 std::vector<double>
		tree->SetBranchAddress("Hit_Energy", &_hitE);  // 替换为实际的 branch 名称
		// 检查事件总数
		Long64_t nEntries = tree->GetEntries();
		cout << "... ... File : " << inFileName.at(i_file) << " , Entries : " << nEntries << endl; 
		for (int i = 0; i < nEntries; i++) {
			if (i%10000==0) cout << "========== Entry : " << i << endl;
			tree->GetEntry(i);  
			double sum = 0.0;
			if (i==0)	cout << "=========== Crystal Number : " << _hitE->size() << endl; 
			for (double value : *_hitE) {
				sum += value;
			}
			hEnergy[i_file]->Fill(sum);
		}
		energyResolution.push_back(FitHist(hEnergy[i_file], fGaus[i_file], fCry[i_file]));
		outFile->cd();
		hEnergy[i_file]->Write();
		file->Close();
	}
	
	for (int i_file = 0; i_file < inFileNum; i_file++) {
		grEnergyResolution->SetPoint(i_file, inEnergy[i_file], 100 * energyResolution[i_file]);
	}
	
	grEnergyResolution->SetTitle("Energy Resolution");
	grEnergyResolution->GetXaxis()->SetTitle("Energy (MeV)");
	grEnergyResolution->GetYaxis()->SetTitle("Energy Resolution");
	grEnergyResolution->GetYaxis()->SetRangeUser(0, 6);
	//grEnergyResolution->GetYaxis()->SetLabelFormat("%.2f%%");
	//grEnergyResolution->GetYaxis()->SetLabelOffset(0.9);
	grEnergyResolution->SetMarkerStyle(22);
	//grEnergyResolution->Draw("AP");
	outFile->cd();
	grEnergyResolution->Write();
}

