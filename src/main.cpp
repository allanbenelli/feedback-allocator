#include <iostream>
// BGL includes
#include <boost/graph/adjacency_list.hpp>
#include <boost/graph/cycle_canceling.hpp>
#include <boost/graph/push_relabel_max_flow.hpp>
#include <boost/graph/successive_shortest_path_nonnegative_weights.hpp>
#include <boost/graph/find_flow_cost.hpp>
#include <string>
#include <map>
#include <vector>
#include <algorithm>

// Graph Type with nested interior edge properties for Cost Flow Algorithms
typedef boost::adjacency_list_traits<boost::vecS, boost::vecS, boost::directedS> traits;
typedef boost::adjacency_list<boost::vecS, boost::vecS, boost::directedS, boost::no_property,
    boost::property<boost::edge_capacity_t, long,
        boost::property<boost::edge_residual_capacity_t, long,
            boost::property<boost::edge_reverse_t, traits::edge_descriptor,
                boost::property <boost::edge_weight_t, long> > > > > graph; // new! weightmap corresponds to costs

typedef boost::graph_traits<graph>::edge_descriptor             edge_desc;
typedef boost::graph_traits<graph>::out_edge_iterator           out_edge_it; // Iterator
typedef traits::vertex_descriptor v_desc;

// Custom edge adder class
class edge_adder {
 graph &G;

 public:
  explicit edge_adder(graph &G) : G(G) {}
  void add_edge(int from, int to, long capacity, long cost) {
    auto c_map = boost::get(boost::edge_capacity, G);
    auto r_map = boost::get(boost::edge_reverse, G);
    auto w_map = boost::get(boost::edge_weight, G); // new!
    const edge_desc e = boost::add_edge(from, to, G).first;
    const edge_desc rev_e = boost::add_edge(to, from, G).first;
    c_map[e] = capacity;
    c_map[rev_e] = 0; // reverse edge has no capacity!
    r_map[e] = rev_e;
    r_map[rev_e] = e;
    w_map[e] = cost;   // new assign cost
    w_map[rev_e] = -cost;   // new negative cost
  }
};

void run(){
    int trainer_count, participant_count, number_of_preferences, number_of_nogos; 
    std::cin >> trainer_count >> participant_count >> number_of_preferences >> number_of_nogos;
    graph G(trainer_count+participant_count);
    edge_adder adder(G);
    auto c_map = boost::get(boost::edge_capacity, G);
    auto rc_map = boost::get(boost::edge_residual_capacity, G);

    const v_desc source = boost::add_vertex(G);
    const v_desc sink = boost::add_vertex(G);
    std::map<std::string, int> name_to_index; //first trainers, than participants
    std::vector<std::string> index_to_name; 
    index_to_name.reserve(trainer_count+participant_count);
    std::vector<std::vector<int>> preference_matrix(participant_count);
    int total_count = participant_count+trainer_count;

    //add trainers vertices -> sink; cap == max number of feedbacks
    for(int i = 0; i < trainer_count; ++i){
      std::string trainer_name; int cap; std::cin >> trainer_name >> cap;
      name_to_index.insert({trainer_name, i});
      index_to_name.push_back(trainer_name);
      adder.add_edge(i, sink, cap, 0);
    }

    // read participant, add source -> participant
    for(int i = 0; i < participant_count; ++i){
      std::string participant_name;
      std::cin >> participant_name;
      int participant_index = i + trainer_count;
      name_to_index.insert({participant_name, participant_index});
      index_to_name.push_back(participant_name);
      adder.add_edge(source, participant_index, 1, 0);
      preference_matrix[i] = std::vector<int>(trainer_count, INT16_MAX); // set default high = low preference; preferences are given more importance
      for(int j = 1; j <= number_of_preferences; ++j){ 
        std::string preference_name;
        std::cin >> preference_name;
        if(preference_name!="x"){ // no preferences = default
          int trainer_id = name_to_index[preference_name];
          preference_matrix[i][trainer_id] = std::min(preference_matrix[i][trainer_id], j); // prefer lower preference score over default
        }
      }
    }
    // handle no-go's
    for(int i = 0; i < number_of_nogos; ++i){
      std::string trainer_name, participant_name; std::cin >> trainer_name >> participant_name;
      int participant_id = name_to_index[participant_name]-trainer_count; // für matrix
      int trainer_id = name_to_index[trainer_name];
      preference_matrix[participant_id][trainer_id] = -1;
    }

    // add participant -> trainer edges, capacity = 1, weight = preference score
    for(int i = 0; i < participant_count; ++i){
      for(int j = 0; j < trainer_count; ++j){
        int preference = preference_matrix[i][j];
        if(preference > 0){
          adder.add_edge(trainer_count+i, j, 1, preference);
        }
      }
    }

    std::vector<std::vector<int>> map(participant_count);
    int max_flow = boost::push_relabel_max_flow(G, source, sink);
    boost::successive_shortest_path_nonnegative_weights(G, source, sink);
    out_edge_it e, eend;
    if (max_flow == participant_count) {
      std::cout << "Allocation found!\n";
    } else {
      std::cout << "XXX - NO SOLUTION! \n";
    }
    std:: cout << "\n";
    for(int i = 0; i < trainer_count; ++i){
      std::cout << index_to_name[i] << ": ";
      for(boost::tie(e, eend) = boost::out_edges(boost::vertex(i,G), G); e != eend; ++e) {
          int tar = boost::target(*e, G);
          int flow = rc_map[*e] - c_map[*e];
          if(tar<total_count && flow){
            std::cout << index_to_name[tar] << " ";
          }
      }
      std::cout << "\n";
    }
}

int main(){
    std::ios_base::sync_with_stdio(false);
    int t; std::cin >> t;
    for(int i = 0; i < t; ++i){
      std::cout << "run: " << i << " ";
      run();
      std::cout << "\n";
    }
}