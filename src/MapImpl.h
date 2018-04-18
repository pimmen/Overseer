#ifndef _MAPIMPL_H_
#define _MAPIMPL_H_

#include "Graph.h"
#include "Map.h"
#include "neutralSetObj.h"
#include "spatial/box_multimap.hpp"
#include "spatial/neighbor_iterator.hpp"
#include "spatial/ordered_iterator.hpp"

namespace Overseer{

    /**
    * \class MapImpl MapImpl.h "MapImpl.h"
    * \brief The interface towards the map.
    */
    class MapImpl : public Map {
        public:
            
            ~MapImpl();

            /**
            * \brief constructor which adds the bot for temp control of map.
            *
            * \param bot Is the starcraft II bot currently run.
            */
            MapImpl(sc2::Agent* bot);

            MapImpl();
            
            /**
            * \brief Initialize overseer, should be done after the map been loaded.
            */
            void Initialize();
            
            /**
            * \brief get the graph representation of the map.
            */
            Graph getGraph();

        private:
            
            //brief Create the tiles from the map.
            void CreateTiles();
            
            //brief Iterate over the tiles and compute the distance to nearest unpathable tile
            void ComputeAltitudes();

            // Used to check the type of "object" on the possition.
            TileTerrain checkTerrainType(std::unique_ptr<NeutralImpl>& checkWith, sc2::Point2D& pos, sc2::ObservationInterface* obs);
            
            //Iterate over all tiles, starting with those furthest away from unpathables (probable candidates for region centers), and add to neighboring region
            //Create new region if no neighboring region is found, if two are found merge the smaller into the larger or create frontier
            std::vector<Region> ComputeTempRegions();
            
            //Find the regions with a real area and add them to map, resolve the frontiers
            void CreateRegions(std::vector<Region> tmp_regions);
            
            // TODO: PIMMEN NEED TO COMMENT THIS ONE!
            void CreateFrontiers();

            Graph m_graph;
            static const size_t min_region_area = 80;
    };
}
#endif /* _MAPIMPL_H_ */
