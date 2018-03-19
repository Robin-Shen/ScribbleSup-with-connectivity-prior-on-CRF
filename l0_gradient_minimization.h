#ifndef L0_GRADIENT_MINIMIZATION_H
#define L0_GRADIENT_MINIMIZATION_H

//#include "graph.h"
#include <vector>
#include <set>
#include <map>
#include <cmath>

#include "boost/utility.hpp"                // for boost::tie
#include "boost/graph/graph_traits.hpp"
#include "boost/graph/adjacency_list.hpp"
#include "boost/graph/graphviz.hpp"
#include "boost/graph/copy.hpp"


inline double non_linear_beta(int iter, int iteration_number, double lambda) {
  double gamma = 2.2;
  return pow((double)iter/(double) iteration_number, gamma)*lambda;
}

// performs l0 gradient minimization as described in
// Fast and Effective L0 Gradient Minimization by Region Fusion - by
// Rang M. H. Nguyen
// Michael S. Brownon
// modefied so that we get..
// for a general Graph, which has to have 'vecS'  as vertices type ande a vertex property named 'value' of type Vector
// returns number of groups
template <typename Graph>
int l0_gradient_minimization(Graph& graph, const std::vector<std::vector<typename Graph::vertex_descriptor>>& init_segments, double lambda) {
  struct group {
    int special_segment_flag;	
    std::vector<int> elements;
    std::set<int> neighbours;
    std::map<int, int> number_of_connections;
    int number_of_elements;
    double average_value;
    group() : elements(0), special_segment_flag(-1){}
  };

  // std::cout << "l0 gradient method ... " << std::endl;

  // the groups are saved here
  std::vector<group> groups(boost::num_vertices(graph));
  // when two groups merge, one of them will not be used anymore, which is indicated by flagging it here
  std::vector<bool> skipover(boost::num_vertices(graph), false);
  // keeps track on the number of groups
  int group_count = boost::num_vertices(graph);
  /*
  typename Graph::vertex_iterator ei, ei_end;
  int i = 0;
  for(boost::tie(ei, ei_end) =  boost::vertices(graph); ei != ei_end; ++ei, ++i) {
	if(*ei != i) {std::cout << "error" << std::endl;}  	
  }
  */
  // the initial segments are represented in groups[init_segments[i][0]], the elements groups[init_segments[i][j]] for j >= 1 are flagged to be skipped 
  for(int i = 0; i < init_segments.size(); ++i) {
	  int root_segment_id = init_segments[i][0];
	  groups[root_segment_id].number_of_elements = graph[(typename Graph::vertex_descriptor)root_segment_id].pixels.size();	
	  groups[root_segment_id].special_segment_flag = i;
	  groups[root_segment_id].elements.insert(groups[root_segment_id].elements.end(), init_segments[i].begin(), init_segments[i].end());
    groups[root_segment_id].average_value = graph[(typename Graph::vertex_descriptor)root_segment_id].color;

	  for(int j = 1; j < init_segments[i].size(); ++j) {
  		int segment_id = init_segments[i][j];
		  skipover[segment_id] = true;
    	groups[root_segment_id].average_value += graph[(typename Graph::vertex_descriptor)segment_id].color;
		  groups[root_segment_id].number_of_elements = graph[(typename Graph::vertex_descriptor)segment_id].pixels.size();	
	  }
    groups[root_segment_id].average_value /= init_segments[i].size();
	  group_count -= init_segments[i].size()-1;
  }

  for(int i = 0; i < init_segments.size(); ++i) {
  	int root_segment_id = init_segments[i][0];
  	for(int j = 0; j < init_segments[i].size(); ++j) {
		  int segment_id = init_segments[i][j];
    	typename Graph::adjacency_iterator ai, ai_end;
		  for(boost::tie(ai, ai_end) = boost::adjacent_vertices(segment_id, graph); ai != ai_end; ++ai) {
  			// if it is in this init_segment, skip
			  // else setup neighbours maps for both
			  if ( std::find(groups[root_segment_id].elements.begin(), groups[root_segment_id].elements.end(), *ai) == groups[root_segment_id].elements.end()) {
  				if(groups[root_segment_id].neighbours.find(*ai) != groups[root_segment_id].neighbours.end()) {
					  //groups[root_segment_id].number_of_connections[*ai] += 1;
            int weight = boost::get(boost::edge_weight, graph, boost::edge(segment_id, *ai, graph).first);
					  groups[root_segment_id].number_of_connections[*ai] += weight;
				  }
				  else {
					  //groups[root_segment_id].number_of_connections[*ai] = 1;
            int weight = boost::get(boost::edge_weight, graph, boost::edge(segment_id, *ai, graph).first);
					  groups[root_segment_id].number_of_connections[*ai] = weight;
				  }
				  groups[*ai].number_of_connections[root_segment_id] = groups[root_segment_id].number_of_connections[*ai];
		      //groups[*ai].neighbours.erase(segment_id);
				  groups[*ai].neighbours.insert(root_segment_id);
				  groups[root_segment_id].neighbours.insert(*ai);
			  }
	    }
	  }
  }
  
  // the rest of the groups are initialised 
  for(int i = 0; i < boost::num_vertices(graph); ++i) {
    if(!skipover[i] && groups[i].special_segment_flag == -1) {
	    groups[i].average_value = graph[(typename Graph::vertex_descriptor)i].color;
	    groups[i].elements.push_back(i);
	    groups[i].number_of_elements = graph[(typename Graph::vertex_descriptor)i].pixels.size();
	    typename Graph::adjacency_iterator ai, ai_end;
	    for(boost::tie(ai, ai_end) = boost::adjacent_vertices(i, graph); ai != ai_end; ++ai){
    		if(!skipover[*ai] && groups[*ai].special_segment_flag == -1) {
	      	groups[i].neighbours.insert(*ai);
          int weight = boost::get(boost::edge_weight, graph, boost::edge(i, *ai, graph).first);
	      	groups[i].number_of_connections[*ai] = weight;
        }
	    }
    }
  }

  // the algorithm
  double beta = 0.0;
  int beta_iter = 0;
  int iteration_number = 100;
  do {
    //std::cout << "beta = " << beta << std::endl;
    //for(auto i = 0; i < groups.size(); ++i) {
    for(int i = 0; i < boost::num_vertices(graph); ++i) {
      if(skipover[i]) {
        continue;
      }
      auto neighbour_it = groups[i].neighbours.begin();
      while(neighbour_it != groups[i].neighbours.end()){
        const int j = *neighbour_it; // no ref or else it wont work
	      // if special_segment_flag is for both greater -1 we would possible merge to init segmets. So we just skip
	      if(groups[i].special_segment_flag > -1 && groups[j].special_segment_flag > -1) {
		      neighbour_it++;
		      continue;
	      }
        int& c_i_j = groups[i].number_of_connections[j];
        int& w_i = groups[i].number_of_elements;
        int& w_j = groups[j].number_of_elements;
        std::set<int>& N_i = groups[i].neighbours;
        std::set<int>& N_j = groups[j].neighbours;
        std::vector<int>& G_i = groups[i].elements;
        std::vector<int>& G_j = groups[j].elements;
        double& Y_i = groups[i].average_value;
        double& Y_j = groups[j].average_value;
	 
        double merge_left_hand_term = w_i*w_j*fabs(Y_i-Y_j)*fabs(Y_i-Y_j);
        double merge_right_hand_term = beta*c_i_j*(w_i+w_j);
        double ERR = 0.0;
        if(merge_left_hand_term <= merge_right_hand_term + ERR) {
	        if(groups[j].special_segment_flag > -1) {
		        groups[i].special_segment_flag = groups[j].special_segment_flag;
            }
          std::vector<int> grp;
          std::set_union(G_i.begin(), G_i.end(),
          G_j.begin(), G_j.end(),
          std::back_inserter(grp));
          G_i = grp;
          Y_i = (1/(double)(w_i+w_j))*(w_i*Y_i + w_j*Y_j);
          w_i = w_i + w_j;
          neighbour_it = N_i.erase(neighbour_it);//N_i.erase(j);

          for(auto neighbour_neighbour_it = N_j.begin(); neighbour_neighbour_it != N_j.end(); ++neighbour_neighbour_it) {
            const int k = *neighbour_neighbour_it;
            if(k == i) {
              continue;
            }
            std::set<int>& N_k = groups[k].neighbours;
            int& c_j_k = groups[j].number_of_connections[k];
            int& c_i_k = groups[i].number_of_connections[k];
            int& c_k_i = groups[k].number_of_connections[i];
            //auto it = N_i.find(k);
            if(N_i.find(k) != N_i.end()) {
              c_i_k += c_j_k;
              //c_k_i += c_j_k
              c_k_i = c_i_k; // wrong in text?
            }
            else {
              N_i.insert(k); // problem since we iterate over it?
              N_k.insert(i);
              c_i_k = c_j_k;
              c_k_i = c_j_k;
            }
            
            N_k.erase(j);
            groups[k].number_of_connections.erase(j);
          }
          skipover[j] = true;
	        group_count -= 1;
          //groups.erase(groups.begin()+j); // delete j'th group PROBLEMOOO dadurch ändert sich der index
        }
        else {
          ++neighbour_it;
        }
      }
    }

    beta_iter++;
    beta = non_linear_beta(beta_iter, iteration_number-1, lambda);
    //std::cout << "beta " << beta << std::endl;
    //std::cout << group_count << std::endl;
  }
  while(group_count!= init_segments.size()); {
  //reconstruct the output signal
    for(int i = 0; i < boost::num_vertices(graph); ++i) {
      if(skipover[i]) {
        continue;
      }
      for(auto element_it = groups[i].elements.begin(); element_it != groups[i].elements.end(); ++element_it) {
        if(groups[i].special_segment_flag == -1) {
	        // std::cout << "bad flag at superpixel: " << *element_it << std::endl;
        }
        graph[(typename Graph::vertex_descriptor)(*element_it)].merge_label = groups[i].special_segment_flag;
        //graph[(typename Graph::vertex_descriptor)(*element_it)].color = groups[i].average_value;
      }
    }
  }
  
  // std::cout << " ... done" << std::endl;
  return group_count;
}
#endif
