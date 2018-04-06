#ifndef Tile_h
#define Tile_h

#include <cstdio>

namespace Overseer{

    enum TileTerrain{
        buildAndPath,
        build,
        path,
        mineral,
        gas,
        nagaTower,
        destructable,
        flyOnly
    };
    
    /**
    * \class Tile Tile.h "Tile.h"
    * \brief A tile is area that has size 1x1 within SCII maps.
    */
    class Tile {
        public:
            /**
            * \brief check if a tile is buildable.
            *
            * \return true is tile is buildable, false otherwise
            */
            TileTerrain getTileTerrain();
            
            /**
            * \brief Get the z-axis value for the tile.
            *
            * \return z-axis value.
            */
            int GroundHeight();
            
            /**
            * \brief corently not used...
            */
            // bool Doodad();
            
            /**
            * \brief Set a tile to buildable.
            *
            * \param buildable boolean value, true if buildable and false otherwise.
            */
            void setTileTerrain(TileTerrain path);
            
            /**
            * \brief Set a distance to the nearest unpathable tile.
            *
            * \param dist The distance til unpathable.
            */
            void setDistNearestUnpathable(float dist);
            
            /**
            * \brief Get the distance to nearest unpathable.
            *
            * \return distance.
            */
            float getDistNearestUnpathable() const;
            
            /**
            * \brief Set the region id this tile belong to.
            *
            * \param regionId the id to set.
            */
            void setRegionId(size_t regionId);
            
            /**
            * \brief Get the region id this tile is in.
            *
            * \return regionid
            */
            size_t getRegionId();

            /**
            * \brief Check if the tile contain neutral unit, e.g. gas, miniral and destroable
            */
            bool isNeutral();
            
        private:
            struct m_tileInfo {
                m_tileInfo();
                TileTerrain     terrain;
                size_t          groundHeight;
            };

            size_t m_regionId;
            float m_distNearestUnpathable;

    };
}

#endif /* Tile_h */
