#include "callback.h"
#include <queue>
#include <boost/graph/connected_components.hpp>
#include <algorithm>
namespace b = boost;

#define EPS 0.00001

// for adding constrains lazily
// we use the gurobi callback mechanism
void myGRBCallback::callback() {
try {
    if (problem != 2 && where == GRB_CB_MIPSOL) {
    	/* MIP solution callback
      	int nodecnt = (int) getDoubleInfo(GRB_CB_MIPSOL_NODCNT); // node number
      	double obj = getDoubleInfo(GRB_CB_MIPSOL_OBJ); // objective value
      	double obj_bnd = getDoubleInfo(GRB_CB_MIPSOL_OBJBND); // current error
      	int solcnt = getIntInfo(GRB_CB_MIPSOL_SOLCNT); // solution number
      	*/
      	int nodecnt = (int) getDoubleInfo(GRB_CB_MIPSOL_NODCNT);
      	//std::cout << "We arrive at one integer node # of model "<<problem + 1<<std::endl;
      	for (int itr=0; itr<root_nodes.size(); itr++) {
      		// generate a subgraph to store commponents
        	Graph& subgraph = graph.create_subgraph();
			// if (backg == 1 && itr == 0 && problem == 4), then model 5 can skip label 0 from being connected
			if (unconnected_layers[itr] && problem == 4) {
	  			continue;
			}
			// component records component number of each vertex in graph
        	std::vector<int> component(num_vertices(graph), -1);	
        	// add active vetex to subgraph
        	for (auto p = vertices(graph); p.first != p.second; ++p.first) {
          		//root_nodes
	  			if (problem == 2) {
	    			if(getSolution(graph[*p.first].var[problem][itr]) > EPS) {
			 			add_vertex(*p.first, subgraph);
			 			//std::cout << *p.first << " is equal to 1;" << std::endl;
					}
	  			}
	  			else {
	    			if(std::abs(getSolution(graph[*p.first].var[problem][itr]) - 1.0) < EPS) {
			 			add_vertex(*p.first, subgraph);
			 			//std::cout << *p.first << " is equal to 1;" << std::endl;
					}
	  			}
        	}
			std::vector<int> local_component(num_vertices(subgraph), 0);
        	size_t num_components = connected_components(subgraph, &local_component[0]);
			// local to global
			for (auto p = vertices(subgraph); p.first != p.second; ++p.first) {
          		component[subgraph.local_to_global(*p.first)] = local_component[*p.first];
        	}
        	if (num_components == 1) {
	  			// model 5 comes here for label 0
	        }
        	else {
				//std::cout << "# of components is: " << num_components << ", start to find violated constratints for segmemt "<<itr+1<<std::endl;
				//record the component # of each component
				std::vector<int> component_num;
				// this is the component_num of root_itr
  				component_num.push_back(component[root_nodes[itr]]);
				// record component_num for all local_commponents
  				for (auto p = vertices(subgraph); p.first != p.second; ++p.first) {
                	if (std::find(component_num.begin(), component_num.end(), local_component[*p.first]) == component_num.end()) {
		  				component_num.push_back(local_component[*p.first]);
					}
          		}
				// find cuts for all components
        		find_cuts(graph, subgraph, component, itr, num_components, component_num, problem); //store in graph, error tell if it's a bad cut
        	} 
      	}
    }
#if 1
    //if (problem == 2 && (where == GRB_CB_SIMPLEX || where == GRB_CB_BARRIER)) 
    if (problem == 2 && where == GRB_CB_MIPSOL) {
      	/* MIP solution callback
      	int nodecnt = (int) getDoubleInfo(GRB_CB_MIPSOL_NODCNT); // node number
      	double obj = getDoubleInfo(GRB_CB_MIPSOL_OBJ); // objective value
      	double obj_bnd = getDoubleInfo(GRB_CB_MIPSOL_OBJBND); // current error
      	int solcnt = getIntInfo(GRB_CB_MIPSOL_SOLCNT); // solution number
      	*/
      	//std::cout << "Inside simplex or barrier callback!!!!!!!!!!!!!!!!!!! "<<std::endl;
      	for (int itr=0; itr<root_nodes.size(); itr++) {
      		// generate a subgraph to store commponents
			//std::cout << "# of Root nodes: " << root_nodes.size() <<std::endl;
        	Graph& subgraph = graph.create_subgraph();
			// component records component number of each vertex in graph
        	std::vector<int> component(num_vertices(graph), -1);
			//std::vector<int> component;
        	// add vetex to subgraph
			//std::cout << "Works here"<<std::endl;
        	for (auto p = vertices(graph); p.first != p.second; ++p.first) {
          		//root_nodes
	  			if(getSolution(graph[*p.first].var[problem][itr]) > EPS) {
		  			add_vertex(*p.first, subgraph);
			 		//std::cout << *p.first << " is equal to 1;" << std::endl;
				}	  
        	}
			//std::cout << "Works here2"<<std::endl;
			std::vector<int> local_component(num_vertices(subgraph), 0);
        	size_t num_components = connected_components(subgraph, &local_component[0]);
			//std::cout << "# of local_component: "<< num_components <<std::endl;
			//for (auto p = vertices(subgraph); p.first != p.second; ++p.first)
			// local to global
			for (auto p = vertices(subgraph); p.first != p.second; ++p.first) {
          		component[subgraph.local_to_global(*p.first)] = local_component[*p.first];
        	}
			// printout all component number
			//std::cout << "# of component: "<< num_components <<std::endl;
			//for (auto p = vertices(graph); p.first != p.second; ++p.first)
        	if (num_components == 1) {
          		//std::cout << "Segment " << itr+1 <<" is connected for now."<<std::endl;
				//continue;
        	}
        	else {
				//record the component # of each component
				std::vector<int> component_num;
				// this is the component_num of root_itr
  				component_num.push_back(component[root_nodes[itr]]);
				// record component_num for all local_commponents
  				for (auto p = vertices(subgraph); p.first != p.second; ++p.first) {
                	if (std::find(component_num.begin(), component_num.end(), local_component[*p.first]) == component_num.end()) {
		  				component_num.push_back(local_component[*p.first]);
					}
          		}
				// find cuts for all components
				//std::cout << "Start finding cuts..."<<std::endl;
        		find_cuts(graph, subgraph, component, itr, num_components, component_num, problem); //store in graph, error tell if it's a bad cut
        	} 
      	}
     }
#endif     
     
	} // END try
	catch (GRBException e) {
    	std::cout << "Error number: " << e.getErrorCode() << std::endl;
        std::cout << e.getMessage() << std::endl;
    }
	catch (...) {
    	std::cout << "Error during callback" << std::endl;
    }
} // END myGRBCallback

// find violated constraints for each segment l
void myGRBCallback::find_cuts(Graph& graph, Graph& subgraph, std::vector<int> component, int itr, int num_components, std::vector<int> component_num, int problem)
{

	// find all nodes in root_component
  	std::vector<Graph::vertex_descriptor> root_component;

  	root_component.push_back(root_nodes[itr]);
  
  	for (auto p = vertices(graph); p.first != p.second; ++p.first) {
        if (*p.first != root_nodes[itr] && component[*p.first] == component[root_nodes[itr]]) { //no duplicate 
                root_component.push_back(*p.first);
            }
    }

  	//std::cout << "Root component : " <<std::endl;
  	//for (int i =0; i< root_component.size(); i++)
	//std::cout << " " << root_component[i] << std::endl;
        
  	//std::cout << "Component number : " <<std::endl;
  	//for (int i =0; i< component_num.size(); i++)
	//std::cout << " " << component_num[i] << std::endl;

  	// loop for non_root_component, and add corresponding cuts
  	for (int itr_component = 1; itr_component < num_components; itr_component++) {
	    // find all nodes in this component， store in superpixels
        std::vector<Graph::vertex_descriptor> superpixels; 
		//std::cout << "Superpixel # " << itr_component <<std::endl;
        for (auto p = vertices(graph); p.first != p.second; ++p.first) {
            if (component[*p.first] == component_num[itr_component]) {
                superpixels.push_back(*p.first);
				//std::cout << " "<< *p.first;
            }
	    }
		//std::cout <<std::endl<< "We are in segment " << itr+1 <<" component "<<itr_component+1<< " out of "<< num_components <<std::endl;
	    // start to run BFS from s, it stops when reaches t
	    std::queue<Graph::vertex_descriptor> Q;
	    Graph::vertex_descriptor s = superpixels[(Graph::vertex_descriptor)0];
	    Graph::vertex_descriptor t = root_component[(Graph::vertex_descriptor)0];
	    Q.push(s);
	    int distance[num_vertices(graph)] = {0};
	    int visited [num_vertices(graph)] = {0};
	    visited [s] = 1;
  		//std::cout << "Checks if distance[2] is indeed 0? = " << distance[1] <<std::endl;

		// Here, we can stop even earlier
	    while(!Q.empty()) {
			s = Q.front();
			Q.pop();
			Graph::adjacency_iterator ai, ai_end;
      
			for(b::tie(ai, ai_end) = b::adjacent_vertices(s, graph); ai != ai_end; ++ai) {
		  		if(visited[*ai] == 0) {
		    		if (*ai == t) {
		      			goto TerminateBFS;
					}
		    		visited[*ai] = 1;
		    		if ( std::find(superpixels.begin(), superpixels.end(), *ai) != superpixels.end()) {
						distance[*ai] = 0;
					}
		    		else {
						distance[*ai] = distance[s] + 1;
					}
			    	Q.push(*ai);
		  		}
		 	}
	    }
            
	    TerminateBFS:    
  		// output distance
		//std::cout <<std::endl<< "Distance: "<<std::endl;
		//for (auto p = vertices(graph); p.first != p.second; ++p.first)
	    	//std::cout << " " <<distance[*p.first];

	    // check every vertx the distance <= # of component
	    int upper_bound = superpixels.size();
	    std::vector <GRBLinExpr> cons(superpixels.size(), 0.0);
		//<GRBLinExpr> cons[superpixels.size()] = {0};
	    for (int vertex=0; vertex< num_vertices(graph); vertex++) {
		  	//std::cout << "In segment " << itr+1 <<", vertex "<<vertex<<std::endl;
	      	if (distance[vertex] > 0.1 && distance[vertex] < upper_bound+.1) {
				if ( std::find(component_num.begin(), component_num.end(), component[vertex]) != component_num.end() ) {
		  			if ( upper_bound >= distance[vertex]) {
						upper_bound = distance[vertex] -1 ;
					}
				}
				else {
				 	cons[distance[vertex]-1] += graph[(Graph::vertex_descriptor)vertex].var[problem][itr];
				 	//std::cout <<vertex<< " added to cut "<< distance[vertex]-1 <<std::endl;
				}
	      	}
	    }
  
  
	    // start to add violated constratints
		//std::cout << "start to add violated constratints for segment "<<itr+1<<std::endl;
		//std::cout << "Upper bound is "<<upper_bound<<std::endl;
	    for (int i=0; i < upper_bound; i++) {
	    	for (int j=0; j < superpixels.size(); j++) {
		  		addLazy( cons[i]>= graph[superpixels[j]].var[problem][itr]);
	      	}
		}
    }
}

/*
int myGRBCallback::vertex_find(Graph& subgraph, int vertex)
{
  for (auto p = vertices(subgraph); p.first != p.second; ++p.first)
	    {
        	if (*p.first == vertex)
				{
					std::cout << "Found same vertex as " << vertex <<std::endl;
		  			return 1;
				}
	    }
  return 0;
}
*/
#undef DEBUG