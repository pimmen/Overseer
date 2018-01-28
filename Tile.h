#ifndef Tile_h
#define Tile_h
#include "Definitions.h"

namespace Overseer{
    
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
            bool Buildable(){return m_tileInfo.buildable;}
            
            /**
            * \brief Get the z-axis value for the tile.
            *
            * \return z-axis value.
            */
            int GroundHeight(){return m_tileInfo.groundHeight;}
            
            /**
            * \brief corently not used...
            */
            bool Doodad(){return m_tileInfo.doodad;}
            
            /**
            * \brief Set a tile to buildable.
            *
            * \param buildable boolean value, true if buildable and false otherwise.
            */
            void setBuildable(bool buildable){m_tileInfo.buildable = buildable;}
            
            /**
            * \brief Set a distance to the nearest unpathable tile.
            *
            * \param dist The distance til unpathable.
            */
            void setDistNearestUnpathable(float dist){m_distNearestUnpathable = dist;}
            
            /**
            * \brief Get the distance to nearest unpathable.
            *
            * \return distance.
            */
            float getDistNearestUnpathable() const {return m_distNearestUnpathable;}
            
            /**
            * \brief Set the region id this tile belong to.
            *
            * \param regionId the id to set.
            */
            void setRegionId(size_t regionId) {
                m_regionId = regionId;
            }
            
            /**
            * \brief Get the region id this tile is in.
            *
            * \return regionid
            */
            size_t getRegionId(){return m_regionId;}
            
        private:
            struct TileInfo {
                TileInfo() : buildable(false), groundHeight(0), doodad(false) {}
                bool            buildable:true;
                size_t          groundHeight:2;
                bool            doodad:true;
            };
            
            size_t m_regionId;
            float m_distNearestUnpathable;
            TileInfo m_tileInfo;
    };
}

#endif /* Tile_h */
