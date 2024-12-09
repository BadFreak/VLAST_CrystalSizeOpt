#include <TFile.h>
#include <TCanvas.h>
#include <TGraph.h>
#include <TLegend.h>
#include <vector>
#include <string>
#include <iostream>

void DrawERTogether() {
    std::vector<std::string> filePaths = {
        "~/HEP/VLAST/VLAST_CrystalSizeOpt/generate/plot/Energy_Resolution_4_4.root",
        "~/HEP/VLAST/VLAST_CrystalSizeOpt/generate/plot/Energy_Resolution_5_5.root",
        "~/HEP/VLAST/VLAST_CrystalSizeOpt/generate/plot/Energy_Resolution_6_6.root"
    };
    std::vector<TGraph*> graphs;  // 存储所有的TGraph指针
    std::vector<std::string> fileTag; // 用于图例标注

    // 遍历文件路径，打开文件并提取TGraph
    for (const auto &filePath : filePaths) {
        TFile *file = TFile::Open(filePath.c_str());
        if (!file || file->IsZombie()) {
            std::cerr << "Error: Unable to open file " << filePath << std::endl;
            continue;
        }

        TGraph * graph = (TGraph*)file->Get("grER");
        if (!graph) {
            std::cerr << "Error: TGraph not found in file " << filePath << std::endl;
            continue;
        }

        // 添加到TGraph数组
        graphs.push_back(graph);
        std::string tagTemp(filePath);
        tagTemp = tagTemp.substr(tagTemp.find_last_of("/") + 1);  // 提取文件名
        tagTemp = tagTemp.substr(tagTemp.find("Resolution") + 11);  // 提取文件名
        tagTemp = tagTemp.substr(0, tagTemp.find(".root"));
        tagTemp += "_crystal_array";
        fileTag.push_back(tagTemp);  // 保存文件名作为图例
    }

    // 如果没有有效的TGraph，则直接退出
    if (graphs.empty()) {
        std::cerr << "No valid TGraphs found." << std::endl;
        return;
    }

    // 创建新的画布
    TCanvas *combinedCanvas = new TCanvas("combinedCanvas", "Combined Graphs", 800, 600);

    // 设置颜色和线型
    int colors[] = {kRed, kBlue, kGreen, kMagenta, kCyan, kOrange, kYellow, kBlack};
    int styles[] = {1, 2, 3, 4, 5, 6, 7, 8}; // 不同的线型
    int nColors = sizeof(colors) / sizeof(colors[0]);

    // 图例
    TLegend *legend = new TLegend(0.15, 0.65, 0.45, 0.85);
    //legend->SetHeader("Energy_Resolution", "C");  // 图例标题

    // 绘制每个TGraph
    bool firstGraph = true;
    for (size_t i = 0; i < graphs.size(); ++i) {
        combinedCanvas->cd();
        combinedCanvas->SetMargin(0.1, 0.1, 0.1, 0.1);
        gPad->SetLogx();
        gPad->SetGrid();
        TGraph *graph = graphs[i];
        graph->SetMarkerStyle(20 + i);
        graph->SetMarkerColor(colors[i % nColors]);
        graph->SetMarkerSize(1.5);
        //graph->SetLineColor(colors[i % nColors]);
        //graph->SetLineStyle(styles[i % nColors]);
        graph->SetLineWidth(2);

        if (firstGraph) {
            graph->GetXaxis()->SetTitle("Energy (GeV)");
            graph->GetYaxis()->SetTitle("Energy Resolution (%)");
            graph->GetYaxis()->SetRangeUser(0, 5);
            graph->GetXaxis()->SetTitleSize(0.045);
            graph->GetXaxis()->SetLabelSize(0.045);
            graph->GetYaxis()->SetTitleSize(0.045);
            graph->GetYaxis()->SetLabelSize(0.045);
            graph->GetYaxis()->SetTitleOffset(0.9);
            graph->Draw("AP");  // 绘制第一个图，并设置坐标轴
            firstGraph = false;
        } else {
            graph->Draw("P SAME");  // 叠加绘制其余图
        }

        // 添加到图例
        legend->AddEntry(graph, fileTag[i].c_str(), "p");
    }

   
    // 绘制图例
    legend->Draw("same");

    // 保存输出
    combinedCanvas->SaveAs("combined_graphs.png");
}
