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
    int l, tn, p, b; std::cin >> l >> tn >> p >> b;
    graph G(l+tn);
    edge_adder adder(G);
    auto c_map = boost::get(boost::edge_capacity, G);
    //auto r_map = boost::get(boost::edge_reverse, G);
    auto rc_map = boost::get(boost::edge_residual_capacity, G);

    const v_desc source = boost::add_vertex(G);
    const v_desc sink = boost::add_vertex(G);
    std::map<std::string, int> names; //leiter, dann tns
    std::vector<std::string> rev_names; rev_names.reserve(l+tn);
    std::vector<std::vector<int>> possible(tn);
    int pers = tn+l;

    for(int i = 0; i < l; ++i){
      std::string l_name; int cap; std::cin >> l_name >> cap;
      names.insert({l_name, i});
      rev_names.push_back(l_name);
      //std::cout << "from: " << i << " to " << sink << " cap: " << cap << " cost:" << 0 << "\n";

      adder.add_edge(i, sink, cap, 0);
    }

    for(int i = 0; i < tn; ++i){
      std::string tn_name;
      std::cin >> tn_name;
      names.insert({tn_name, i+l});
      rev_names.push_back(tn_name);
      //std::cout << "from: " << source << " to " << i+l << " cap: " << "1" << " cost:" << 0 << "\n";

      adder.add_edge(source, i+l, 1, 0);
      possible[i] = std::vector<int>(l, p+1); // alle haben prio + 1
      for(int j = 1; j <= p; ++j){ //fange mit prio 1 an
        std::string p_name;
        std::cin >> p_name;
        if(p_name!="x"){
          int leiter_id = names[p_name];
          possible[i][leiter_id] = std::min(possible[i][leiter_id], j);
        }
      }
    }
    for(int i = 0; i < b; ++i){
      std::string l_name, tn_name; std::cin >> l_name >> tn_name;
      int tn_id = names[tn_name]-l; // für matrix
      int l_id = names[l_name];
      //std::cout << "name: " << l_name << " " << tn_name << "\n";
      possible[tn_id][l_id] = -1;
    }
/*
    for(int j = 0; j < l; ++j){
      std::cout << rev_names[j] << "|";
    }
    std::cout << "\n";
*/
    for(int i = 0; i < tn; ++i){
      for(int j = 0; j < l; ++j){
        int prio = possible[i][j];
        if(prio > 0){
          adder.add_edge(l+i, j, 1, prio);
        }
      }
    }

    std::vector<std::vector<int>> map(tn);
    int flow = boost::push_relabel_max_flow(G, source, sink);
    boost::successive_shortest_path_nonnegative_weights(G, source, sink);
    boost::find_flow_cost(G);
    //std::cout << "flow: " << flow << " flow: " << cost << "\n";
    out_edge_it e, eend;
    if (flow == tn) {
      std::cout << "optimale Lösung!\n";
    } else {
      std::cout << "XXX - KEINE LÖSUNG! \n";
    }
    std:: cout << "\n";
    for(int i = 0; i < l; ++i){
      std::cout << rev_names[i] << ": ";
      for(boost::tie(e, eend) = boost::out_edges(boost::vertex(i,G), G); e != eend; ++e) {
          //int src =  boost::source(*e, G);
          int tar = boost::target(*e, G);
          //std::cout << "from: " << src << " to " << tar << "\n";
          int flow = rc_map[*e] - c_map[*e];
          if(tar<pers && flow){
            std::cout << rev_names[tar] << " ";
             /*std::cout << "edge from " << rev_names[tar] << " to " << rev_names[src] 
              << " with capacity " << c_map[r_map[*e]] << " and residual capacity " << rc_map[r_map[*e]] << "\n";
              std::cout << "flow: " << rc_map[*e] - c_map[*e] << "\n";*/
              
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