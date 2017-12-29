#ifndef Graph_h
#define Graph_h
#include "Map.h"

class Graph {
private:
    Map *p_map;
    std::vector<ChokePoint> m_chokePoints;
public:
    Graph(){}
    Graph(Map *map):p_map(map){}
    
    std::vector<ChokePoint> getChokePoints() { return m_chokePoints; }
    
    void setMap(Map *map){p_map = map;}
    
    void CreateChokePoints() {
        for(auto const & frontier : p_map->getRawFrontier()) {
            
        }
    }
};

#endif /* Graph_h */
