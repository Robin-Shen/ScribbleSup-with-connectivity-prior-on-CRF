/*
Mainfile of oneshot with output!

Version1: Uses AVGCOLOR and no probability map as unary term.

Naming: main_x_y.cpp
x: 1 for using average color, 2: for loading probability map from file
y: auto, for surpressed output and always with scribble file
y: manual, for output and with the possibility to draw scribbles

Usage: ./main_manual image.jpg superpixel.csv  brushwidth timelimit lambda sribble_option scribble_file (optional)

Specify models to run below in the main function (line 533).
*/

#include "graph.h"
#include "image.h"
#include "callback.h"
#include "gurobi_c++.h"
#include "l0_gradient_minimization.h"
#include <opencv2/imgproc/imgproc.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/video/tracking.hpp>
#include <iostream>
#include <string>
#include <sstream>
#include <fstream>
#include <cmath>
#include <algorithm> 
#include <array>

using namespace std;
using namespace cv;
#define EPS  0.0001
bool trigger;
Mat img;
std::vector<std::vector<CvPoint>> seeds;
int brushwidth;
int nclick = 0;
std::vector<bool> unconnected;
std::vector<int> labels;

//std::vector<std::pair<uint32_t, uint32_t>> master_pixels;

std::string itos(int i) {std::stringstream s; s << i; return s.str(); }

// Function to load scribbles from a csv file provided as input
void loadScribbles(std::string scribble_file) {
    ifstream scribble_csv;
    scribble_csv.open(scribble_file);
    std::string row_csv;
    while (std::getline(scribble_csv,row_csv)) {
        //cout << row_csv << endl;
        std::stringstream element_csv(row_csv);
        std:string cell;
        seeds.emplace_back();
        int i = 0, label,x,y;
        while (std::getline(element_csv, cell,',')) {
            if (i==0) {
                // We record the label
                label = std::stoi(cell);
                labels.push_back(label);
                nclick++;
                // and enforce connectivity if it is not background
                if (std::stoi(cell) == 0) {
                    unconnected.push_back(1);
                }
                else { // else we enforce connectivity
                    unconnected.push_back(0);
                }        
            }
            else {
                if (i % 2 == 0) { // Even, y
                    y = std::stoi(cell);
                    if (i > 2) {
                        if (label == 0) // OpenCV uses BGR not RGB!
                            line(img, cvPoint(x, y), cvPoint(x, y), Scalar(0, 0, 0), 2*brushwidth, CV_AA, 0);
                        if (label == 1)
                            line(img, cvPoint(x, y), cvPoint(x, y), Scalar(0, 0, 128), 2*brushwidth, CV_AA, 0);
                        if (label == 2)
                            line(img, cvPoint(x, y), cvPoint(x, y), Scalar(0, 128, 0), 2*brushwidth, CV_AA, 0);
                        if (label == 3)
                            line(img, cvPoint(x, y), cvPoint(x, y), Scalar(0, 128, 128), 2*brushwidth, CV_AA, 0);
                        if (label == 4)
                            line(img, cvPoint(x, y), cvPoint(x, y), Scalar(128, 0, 0), 2*brushwidth, CV_AA, 0);
                        if (label == 5)
                            line(img, cvPoint(x, y), cvPoint(x, y), Scalar(128, 0, 128), 2*brushwidth, CV_AA, 0);
                        if (label == 6)
                            line(img, cvPoint(x, y), cvPoint(x, y), Scalar(128, 128, 0), 2*brushwidth, CV_AA, 0);
                        if (label == 7)
                            line(img, cvPoint(x, y), cvPoint(x, y), Scalar(128, 128, 128), 2*brushwidth, CV_AA, 0);
                        if (label == 8)
                            line(img, cvPoint(x, y), cvPoint(x, y), Scalar(0, 0, 64), 2*brushwidth, CV_AA, 0);
                        if (label == 9)
                            line(img, cvPoint(x, y), cvPoint(x, y), Scalar(0, 0, 192), 2*brushwidth, CV_AA, 0);
                        if (label == 10)
                            line(img, cvPoint(x, y), cvPoint(x, y), Scalar(0, 128, 64), 2*brushwidth, CV_AA, 0);
                        if (label == 11)
                            line(img, cvPoint(x, y), cvPoint(x, y), Scalar(0, 128, 192), 2*brushwidth, CV_AA, 0);
                        if (label == 12)
                            line(img, cvPoint(x, y), cvPoint(x, y), Scalar(128, 0, 64), 2*brushwidth, CV_AA, 0);
                        if (label == 13)
                            line(img, cvPoint(x, y), cvPoint(x, y), Scalar(128, 0, 192), 2*brushwidth, CV_AA, 0);
                        if (label == 14)
                            line(img, cvPoint(x, y), cvPoint(x, y), Scalar(128, 128, 64), 2*brushwidth, CV_AA, 0);
                        if (label == 15)
                            line(img, cvPoint(x, y), cvPoint(x, y), Scalar(128, 128, 192), 2*brushwidth, CV_AA, 0);
                        if (label == 16)
                            line(img, cvPoint(x, y), cvPoint(x, y), Scalar(0, 64, 0), 2*brushwidth, CV_AA, 0);
                        if (label == 17)
                            line(img, cvPoint(x, y), cvPoint(x, y), Scalar(0, 64, 128), 2*brushwidth, CV_AA, 0);
                        if (label == 18)
                            line(img, cvPoint(x, y), cvPoint(x, y), Scalar(0, 192, 0), 2*brushwidth, CV_AA, 0);
                        if (label == 19)
                            line(img, cvPoint(x, y), cvPoint(x, y), Scalar(0, 192, 128), 2*brushwidth, CV_AA, 0);
                        if (label == 20)
                            line(img, cvPoint(x, y), cvPoint(x, y), Scalar(128, 64, 0), 2*brushwidth, CV_AA, 0);

                        for (int i= -brushwidth; i < brushwidth; i++) {
                            for (int j = -brushwidth; j < brushwidth; j++) {
                                if (x+ i >= 0 & x + i < img.size().width & y + i >= 0 & y + j < img.size().height) {
                                    seeds.back().push_back(cvPoint(x + i, y + j));
                                }
                            }
                        }
                    }
                }
                else { // odd, x
                    x = std::stoi(cell);
                }
            }
            i++;
        }
    }
}

// single click on image to select seed, f? void*?
void onMouse(int event, int x, int y, int flag, void*) {
    // if the left mouse button is pressed we enforce connectivity
    if (event == CV_EVENT_LBUTTONDOWN) {
        seeds.emplace_back();
        unconnected.push_back(0);
        trigger = true;
        nclick++;
        // We record the label
        labels.push_back(nclick);
        //cout << "Left button of the mouse is clicked - position (" << x << ", " << y << ")" << endl;
    }
    
    if (event == CV_EVENT_LBUTTONUP) {
        trigger = false;
    }

    // right button pressed means we do not enforce connectivity
    if  (event == CV_EVENT_RBUTTONDOWN) {
        seeds.emplace_back();
        unconnected.push_back(1);
        nclick++;
        trigger = true;

    }

    if(event == CV_EVENT_RBUTTONUP) {
        trigger = false;
    }

    if (event == EVENT_MOUSEMOVE) {
        if (trigger) {
            if (nclick == 1) // OpenCV uses BGR not RGB!
                line(img, cvPoint(x, y), cvPoint(x, y), Scalar(0, 0, 0), 2*brushwidth, CV_AA, 0);
            if (nclick == 2)
                line(img, cvPoint(x, y), cvPoint(x, y), Scalar(0, 0, 128), 2*brushwidth, CV_AA, 0);
            if (nclick == 3)
                line(img, cvPoint(x, y), cvPoint(x, y), Scalar(0, 128, 0), 2*brushwidth, CV_AA, 0);
            if (nclick == 4)
                line(img, cvPoint(x, y), cvPoint(x, y), Scalar(0, 128, 128), 2*brushwidth, CV_AA, 0);
            if (nclick == 5)
                line(img, cvPoint(x, y), cvPoint(x, y), Scalar(128, 0, 0), 2*brushwidth, CV_AA, 0);
            if (nclick == 6)
                line(img, cvPoint(x, y), cvPoint(x, y), Scalar(128, 0, 128), 2*brushwidth, CV_AA, 0);
            if (nclick == 7)
                line(img, cvPoint(x, y), cvPoint(x, y), Scalar(128, 128, 0), 2*brushwidth, CV_AA, 0);
            if (nclick == 8)
                line(img, cvPoint(x, y), cvPoint(x, y), Scalar(128, 128, 128), 2*brushwidth, CV_AA, 0);
            if (nclick == 9)
                line(img, cvPoint(x, y), cvPoint(x, y), Scalar(0, 0, 64), 2*brushwidth, CV_AA, 0);
            if (nclick == 10)
                line(img, cvPoint(x, y), cvPoint(x, y), Scalar(0, 0, 192), 2*brushwidth, CV_AA, 0);
            if (nclick == 11)
                line(img, cvPoint(x, y), cvPoint(x, y), Scalar(0, 128, 64), 2*brushwidth, CV_AA, 0);
            if (nclick == 12)
                line(img, cvPoint(x, y), cvPoint(x, y), Scalar(0, 128, 192), 2*brushwidth, CV_AA, 0);
            if (nclick == 13)
                line(img, cvPoint(x, y), cvPoint(x, y), Scalar(128, 0, 64), 2*brushwidth, CV_AA, 0);
            if (nclick == 14)
                line(img, cvPoint(x, y), cvPoint(x, y), Scalar(128, 0, 192), 2*brushwidth, CV_AA, 0);
            if (nclick == 15)
                line(img, cvPoint(x, y), cvPoint(x, y), Scalar(128, 128, 64), 2*brushwidth, CV_AA, 0);
            if (nclick == 16)
                line(img, cvPoint(x, y), cvPoint(x, y), Scalar(128, 128, 192), 2*brushwidth, CV_AA, 0);
            if (nclick == 17)
                line(img, cvPoint(x, y), cvPoint(x, y), Scalar(0, 64, 0), 2*brushwidth, CV_AA, 0);
            if (nclick == 18)
                line(img, cvPoint(x, y), cvPoint(x, y), Scalar(0, 64, 128), 2*brushwidth, CV_AA, 0);
            if (nclick == 19)
                line(img, cvPoint(x, y), cvPoint(x, y), Scalar(0, 192, 0), 2*brushwidth, CV_AA, 0);
            if (nclick == 20)
                line(img, cvPoint(x, y), cvPoint(x, y), Scalar(0, 192, 128), 2*brushwidth, CV_AA, 0);
            if (nclick == 21)
                line(img, cvPoint(x, y), cvPoint(x, y), Scalar(128, 64, 0), 2*brushwidth, CV_AA, 0);

            for (int i= -brushwidth; i < brushwidth; i++) {
                for (int j = -brushwidth; j < brushwidth; j++) {
                    if (x + i >= 0 & x + i < img.size().width & y + i >= 0 & y + j < img.size().height) {
                        seeds.back().push_back(cvPoint(x + i, y + j));
                    }
                }
            }
            imshow("nregion", img);
        }
    }
} // END onMouse


void master_problem(
  Graph& rag, ///< the graph of superpixels
  int numseg,
  std::vector<vector<Graph::vertex_descriptor>> root_nodes, ///< master nodes of all segments 
  std::vector<std::vector<Graph::vertex_descriptor>>& segments, ///< the l0 heuristic segments
  std::vector<std::vector<std::vector<Graph::vertex_descriptor>>>& final_segments, ///< the final segments will be stored in here
  double tlimit,
  double lambda,
  ofstream& ofp,
  std::vector<int> models,
  int nmodels) {
    int l=0;

    std::vector<GRBEnv> envs(nmodels);
    std::vector<GRBModel> model;

    for(auto it = envs.begin(); it!=envs.end();++it) {
        auto env = *it;
        model.emplace_back(env);
    }
    
    // Turn off display and heuristics and enable adding constraints in our callback function
    for (l=0; l<nmodels; l++) {
        model[l].set(GRB_IntParam_OutputFlag, 1); 
        model[l].set(GRB_DoubleParam_TimeLimit, tlimit);
        model[l].set(GRB_IntParam_LazyConstraints, 1);
    }
      //model[2].set(GRB_IntParam_Method, 1);
    
    double avgcolor[numseg];
    for (int i=0; i<numseg; i++) {
	    avgcolor[i] =0;
        for(int j=0; j < root_nodes[i].size(); j++) {
	        avgcolor[i] += rag[(Graph::vertex_descriptor)root_nodes[i][j]].color;
        }
	    avgcolor[i] = avgcolor[i]/double(root_nodes[i].size());
	}

    std::vector <GRBLinExpr> objective(nmodels);
    
    //add RAG node as variables, multiply by pixel size
    for (l=0; l<nmodels; l++) {
        objective[l] = 0.0;
        for(int i = 0; i< num_vertices(rag); ++i) {
    	    for (int j=0; j<numseg; j++) {
	            if (l == 2) {
    	            rag[(Graph::vertex_descriptor)i].var[l].push_back(model[l].addVar(0.0, 1.0, 0.0, GRB_CONTINUOUS, "x^"+itos(l)+"_"+itos(i)+"_"+itos(j)));
                }
	            else {
    	            rag[(Graph::vertex_descriptor)i].var[l].push_back(model[l].addVar(0.0, 1.0, 0.0, GRB_BINARY, "x^"+itos(l)+"_"+itos(i)+"_"+itos(j)));
	            }
	            // to account also the superpixel size
	            objective[l] += rag[(Graph::vertex_descriptor)i].pixels.size() * (1 - lambda) * std::abs(rag[(Graph::vertex_descriptor)i].color - avgcolor[j])*rag[(Graph::vertex_descriptor)i].var[l][j];
	        } 
        }   
    }

    // add a dummy binary variable for model 3
    // if (std::any_of(models.begin(), models.end(), [](int i){return i==2;})) {
        GRBVar dummy = model[2].addVar(0.0, 1.0, 0.0, GRB_BINARY, "dummy");
        dummy.set(GRB_DoubleAttr_Start, 1.0); 
        objective[2] += dummy;
    //}
    
    
    // loop over edge sets to add absolute varibles and objective
    graph_traits<Graph>::edge_iterator ei, ei_end;
    for (l=0; l<nmodels; l++) {
        for (tie(ei, ei_end) = edges(rag); ei != ei_end; ++ei) {
            int s = source(*ei, rag);
            int t = target(*ei, rag);
	        // boundary length between two superpixels
	        double w = boost::get(boost::edge_weight, rag, *ei);
	        for (int j=0; j<numseg; j++) {
	            GRBVar x_p = model[l].addVar(0.0, 1.0, 0.0, GRB_CONTINUOUS, "xp^"+itos(l)+"_"+itos(s)+"_"+itos(j));
	            GRBVar x_n = model[l].addVar(0.0, 1.0, 0.0, GRB_CONTINUOUS, "xn^"+itos(l)+"_"+itos(t)+"_"+itos(j));
    	        objective[l] += lambda * w * (x_p + x_n);
	            // add absolute constraints
	            GRBLinExpr left(0.0), right(0.0);
    	        left += rag[(Graph::vertex_descriptor)s].var[l][j] - rag[(Graph::vertex_descriptor)t].var[l][j];
    	        right += x_p - x_n;
    	        model[l].addConstr(left == right);
    	    }  
        }
    }   
    
    // create objective function
    for (l=0; l<nmodels; l++) {
        model[l].setObjective(objective[l], GRB_MINIMIZE);
    }
    
    // add = 1 constraints for every node
    for (l=0; l<nmodels; l++) {
        for(int i = 0; i< num_vertices(rag); ++i) {
	        GRBLinExpr constraints(0.0);
	        for (int j=0; j<numseg; j++) {
	            constraints += rag[(Graph::vertex_descriptor)i].var[l][j];
	        }  
	        model[l].addConstr(constraints == 1.0);
        }
    }

    // set all root nodes to be = 1
    for (l=0; l<nmodels; l++) {
        for (int itr=0; itr<numseg; itr++) {
	        for (int j=0; j< root_nodes[itr].size(); j++) {
	            rag[(Graph::vertex_descriptor)root_nodes[itr][j]].var[l][itr].set(GRB_DoubleAttr_LB, 1.0);
            }
        }
        model[l].update();
    } 
    
    #if 1
    // read solution from l0 heuristic, 
        for (l=0; l<nmodels; l++) {
	        for (int i=0; i<numseg; i++) {
    	        for(int j=0; j<segments[i].size(); ++j) {
	                int k = segments[i][j];
	                rag[(Graph::vertex_descriptor)k].var[l][i].set(GRB_DoubleAttr_Start, 1.0); 
	            }  
    	    }
        }
    #endif

    for (auto l: models) {
        model[l].write("model" + to_string(l+1) + ".lp");
    }

    // set callback, rootnode is the first node for every brush
    vector<Graph::vertex_descriptor> root_node;
    for (int itr=0; itr<root_nodes.size(); itr++) {
        root_node.push_back(root_nodes[itr][0]); 
    }
    
    //std::vector<myGRBCallback> cbs(4);
    
    // model 4 has no callback
    const clock_t begin_time = clock();
    if (std::any_of(models.begin(), models.end(), [](int i){return i==3;})) {
        model[3].optimize();
        ofp << " model 4: " << model[3].get(GRB_DoubleAttr_Runtime)<<endl;
    }

    // callback and solve gurobi
    std::vector<myGRBCallback> cbs;
    for (l=0; l<nmodels; l++){
        cbs.push_back(myGRBCallback(rag, root_node, l, unconnected));   
    }
    for (auto l: models) {
        model[l].setCallback(&(cbs[l]));
        const clock_t begin_time = clock();
        if (l == 3) {
	        continue;
        }
        model[l].optimize();
        ofp << " model " << l+1 <<": " << model[l].get(GRB_DoubleAttr_Runtime)<<endl;
    }


    
    int fraction = 0;
    // here ends solving gurobi, first check if LP's (model2) all soluions are binary
     if (std::any_of(models.begin(), models.end(), [](int i){return i==2;})) {
        for (int i=0; i<num_vertices(rag); ++i) {
            for (int j=0; j<numseg; ++j) {
	            if (std::abs(rag[(Graph::vertex_descriptor)i].var[2][j].get(GRB_DoubleAttr_X) - 1.0) > EPS && std::abs(rag[(Graph::vertex_descriptor)i].var[2][j].get(GRB_DoubleAttr_X) - 0) > EPS) {
	                fraction ++;
	            }
            }
        }
    }
  
    ofp << "\nEnergy for"<< endl;
    for (auto l: models) {
       ofp << " model " << l+1 << ": " << model[l].get(GRB_DoubleAttr_ObjVal)<<endl;
    }

    ofp << "\nMIP gap for \n model" << endl;
    for (auto l: models) {
        ofp << l << ": " << model[l].get(GRB_DoubleAttr_MIPGap)*100<<"%"<<endl;
    }
   
    std::cout << "Fractional solution is " << 100* fraction/double(numseg * num_vertices(rag)) << "%"<<endl;
    ofp << "\nFractional solution for model 3 is " << 100* fraction/double(numseg * num_vertices(rag)) << "%"<<endl;
   
    if (std::any_of(models.begin(), models.end(), [](int i){return i==4;})) { // backg
        ofp << "\nModel 5 (background) helps improving the time from "<< model[1].get(GRB_DoubleAttr_Runtime) << "to "<< model[4].get(GRB_DoubleAttr_Runtime) <<endl;
    }
   ofp << "-----------------------------------------------------------end \n\n";

   // initialize final_segments[l] // should be independent of model size
   for (l=0; l<nmodels; l++) {
        for (int j=0; j<numseg; ++j) {
	        final_segments[l].emplace_back();
        }
    }
    // additional label for problem ? 2?
    final_segments[2].emplace_back();

   // output final segment 
   for (auto l: models) {
        // for model 2 (lp relaxation), need additional label to store fractional pixel
        if (l == 2) {
	        for (int i=0; i<num_vertices(rag); ++i) {
	            for (int j=0; j<numseg; ++j) {
	                if (std::abs(rag[(Graph::vertex_descriptor)i].var[2][j].get(GRB_DoubleAttr_X) - 1.0) < EPS) {
	                    final_segments[2][j].push_back((Graph::vertex_descriptor)i);
                    }
	                else if (std::abs(rag[(Graph::vertex_descriptor)i].var[2][j].get(GRB_DoubleAttr_X) - 1.0) > EPS && std::abs(rag[(Graph::vertex_descriptor)i].var[2][j].get(GRB_DoubleAttr_X) - 0) > EPS) {
	                    // check for duplicates
	                    //std::cout << "Found fractional solution at node " << i <<" equals " <<rag[(Graph::vertex_descriptor)i].var[2][j].get(GRB_DoubleAttr_X)<<std::endl;
	                    if (std::find(final_segments[2][numseg].begin(), final_segments[2][numseg].end(), (Graph::vertex_descriptor)i) == final_segments[2][numseg].end()) {
	                        final_segments[2][numseg].push_back((Graph::vertex_descriptor)i);
	                    }
                    }
	            }
            }
        }
        else {
	        for (int i=0; i<num_vertices(rag); ++i) {
	            for (int j=0; j<numseg; ++j) {
	                if (std::abs(rag[(Graph::vertex_descriptor)i].var[l][j].get(GRB_DoubleAttr_X) - 1.0) < EPS) {
	                    final_segments[l][j].push_back((Graph::vertex_descriptor)i);
                    }
	            }
	        }
        }
    }    
} // END master_problem


int main(int argc, char** argv) {
    if (!(argc == 7 || argc == 8 )) {
        std::cout << "Usage: program input.png input.csv brushwidth timelimit lambda scribble(bool) (input_scirrble.csv if scribble == 1 or 2). Rightclick: Unconnected. Leftclick: Connected" << std::endl;
        return 1;
    }

    //Image image("swan.png", "swan.csv");
    Image image(argv[1], argv[2]);

    brushwidth = atoi(argv[3]);
    double tlimit = atof(argv[4]);
    double lambda = atof(argv[5]);

    int loadscribble = atoi(argv[6]); // 0: draw scribble, 1: load scribble, 2: both

    if (loadscribble > 0 && argc != 8) {
        std::cout << "Error. If scribbles activated you MUST provide a scribble file!" << endl;
        return 1;
    }

    destroyAllWindows();
    string out_file = argv[1];
    out_file.erase(out_file.end() - 4, out_file.end());
    out_file.append("_sup.png");

    img = imread(out_file);
    namedWindow("nregion");
    if(loadscribble > 0) {
        loadScribbles(argv[7]);
        if(loadscribble == 2) {
            setMouseCallback("nregion", onMouse, NULL);
        }
        imshow("nregion", img);
        waitKey(0);
    }
    else {
        setMouseCallback("nregion", onMouse, NULL);
        imshow("nregion", img);
        waitKey(0);
    }

    out_file = argv[1];
    out_file.erase(out_file.end() - 4, out_file.end());
    out_file.append("_brush.png");
    cv::imwrite(out_file, img);
    //cvDestroyWindow("nregion");

    // master_nodes records the superpixel of seeds.
    std::vector<std::vector<Graph::vertex_descriptor>> master_nodes;
    for (int i = 0; i < seeds.size(); i++) {
        master_nodes.emplace_back();
        auto seed = seeds[i];
        for (int j = 0; j < seed.size(); j++) {
            Graph::vertex_descriptor superpixel = image.pixelToSuperpixel(seed[j].x, seed[j].y);
            if (std::find(master_nodes[i].begin(), master_nodes[i].end(), superpixel) == master_nodes[i].end()) // no duplicates
                master_nodes[i].push_back(superpixel);
        }
    }

    for (int i = 0; i < master_nodes.size(); i++) {
        std::cout << "Root node " << i << ": " << master_nodes[i][0] << std::endl;
    }
    Graph rag = image.graph();

    size_t n = num_vertices(rag);

    std::vector<std::vector<Graph::vertex_descriptor>> segments(master_nodes.size());

    ofstream ofp;

    out_file = argv[1];
    out_file.erase(out_file.end() - 4, out_file.end());
    out_file.append("_output.txt");

    ofp.open(out_file, std::ios_base::app);

    ofp << "This is statistics for " << out_file << ", brushwidth, tlimit, lambda equals: " << brushwidth << ", " << tlimit << ", " << lambda << std::endl;

    #if 1
        const clock_t begin_time = clock();
        l0_gradient_minimization(rag, master_nodes, 0.1);
        ofp << "Run time for \n l_0 heuristic: " << float(clock() - begin_time) / CLOCKS_PER_SEC << std::endl;

        for (int i = 0; i < num_vertices(rag); ++i) {
            for (int j = 0; j < segments.size(); ++j) {
                if (rag[(typename Graph::vertex_descriptor) i].merge_label == j) {
                    segments[j].push_back((Graph::vertex_descriptor) i);
                }
            }
        }
    #endif

    vector<Graph::vertex_descriptor> master_node;
    for (int i = 0; i < master_nodes.size(); i++) {
        master_node.push_back(master_nodes[i][0]);
    }
    cv::Mat output;
    #if 1
    // output l0 results


    output = image.writeSegments(argv[1], master_node, segments, rag, 0, labels);
    output.copyTo(img);
    imshow("lo_heuristic results", img);
    #endif

    // In order to just calculate certain models, we use a vector:
    std::vector<int> models {}; // 3 for comparison, manually SELECT MODELS TO RUN

    // Check if we have unconnected labels
    bool run_unconnected = false;
    for (int i = 0; i < unconnected.size(); i++) {
        if (unconnected[i]) {
            run_unconnected = true;
        }
    }
    if (run_unconnected) {
        models.push_back(4);
        cout << "We have unconnected labels so we will run the ILP-PC without enforced connectivity for respective labels (ILP-PB)" << endl;
    }
    else {
        models.push_back(1);
        cout << "We only have connected labels so we run ILP-PC" << endl;
    }

    // initialize 4+backg space for segmentation results of all models
    int nmodels = 5;   
    std::vector<std::vector<std::vector<Graph::vertex_descriptor>>> final_segments(nmodels);

    // gurobi model and solve
    master_problem(rag, master_nodes.size(),master_nodes, segments, final_segments, tlimit, lambda, ofp, models, nmodels);

    // Now save and display all solutions:
    for (auto l: models) {
        output = image.writeSegments(argv[1], master_node, final_segments[l], rag, l+1, labels);
        output.copyTo(img);
        if (l==0)
            imshow("ILP-C results 1", img); // not needed
        if (l==1)
            imshow("ILP-PC results 2", img); // we use this model // but actually only with background label
        if (l==2)
            imshow("LP-PC results 3", img); // not needed
        if (l==3)
            imshow("ILP-P results 4", img); // for comparison
        if (l==4)
            imshow("ILP-PB results 5", img); // ILP_PC with unconnected labels
    }

    waitKey(0);
    return 0;
}
