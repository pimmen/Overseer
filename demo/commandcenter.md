# Command center compile

So due to [commandcenter](https://github.com/davechurchill/commandcenter) is a very common bot to start out with we will do a demonstration of how we can use Overseer in it. 

Guide limitation:

* Will **not** go through the process of getting and testing commandcenter.
* Will **not** show for visual studio.

Guide will show:

* Where and how to make the CMake in commandcenter compile Overseer

So we updated the Overseer to use CPP's file this has a lot of reasons that we will not go through.

## Compile settings

This will be done in the terminal

```bash
$ cd /path/to/commandcenter/ #Change path to the commandcenter root directory
$ cd src/ #Change into the src directory in the commandcenter folder.
$ git clone https://github.com/pimmen89/Overseer/tree/master #clone overseer
$ vi CMakeLists.txt # You can use any text-editor I (mejan) personly like vi
```

In the CMakeLists.txt file edit the line 

```
file(GLOB BOT_SOURCES "*.cpp"  "*.h" "*.hpp")
```

to

```
file(GLOB BOT_SOURCES "*.cpp" "*.h" "*.hpp" "Overseer/*.cpp" "Overseer/*.h" "Overseer/*.hpp")
```

then save the file. **Note:** Now we have change so cmake will look within the *Overseer* directory for file to compile as well.

## Demo

Now it is time to start and use this Overseer, to do this we will use it within the files MapTools.h and MapTools.cpp (commandcenter/src/MapTools.h and commandcenter/src/MapTools.cpp)

Open with your text-editor/IDE the file MapTools.h and add the lines shown by comments:

```c++
#pragma once

#include "Overseer/MapImpl.h" //ADD THIS LINE (OVERSEER)
#include <vector>
#include "DistanceMap.h"
#include "UnitType.h"

class CCBot;

class MapTools
{
    CCBot & m_bot;
    int     m_width;
    int     m_height;
    float   m_maxZ;
    int     m_frame;
    Overseer::MapImpl overseerMap; //ADD THIS LINE (OVERSEER)
```

Save the file and then open MapTools.cpp with your text-editor/IDE and add the lines shown by comment:

```c++
#include "MapTools.h"
#include "Util.h"
#include "CCBot.h"

#include <iostream>
#include <sstream>
#include <fstream>
#include <array>

const size_t LegalActions = 4;
const int actionX[LegalActions] ={1, -1, 0, 0};
const int actionY[LegalActions] ={0, 0, 1, -1};

typedef std::vector<std::vector<bool>> vvb;
typedef std::vector<std::vector<int>>  vvi;
typedef std::vector<std::vector<float>>  vvf;

#ifdef SC2API
    #define HALF_TILE 0.5f
#else
    #define HALF_TILE 0
#endif

// constructor for MapTools
MapTools::MapTools(CCBot & bot)
    : m_bot     (bot)
    , m_width   (0)
    , m_height  (0)
    , m_maxZ    (0.0f)
    , m_frame   (0)
    , overseerMap () //ADD THIS LINE (OVERSEER)
{

}

void MapTools::onStart()
{
#ifdef SC2API
    m_width  = m_bot.Observation()->GetGameInfo().width;
    m_height = m_bot.Observation()->GetGameInfo().height;
#else
    m_width  = BWAPI::Broodwar->mapWidth();
    m_height = BWAPI::Broodwar->mapHeight();
#endif
    overseerMap.setBot(&m_bot);
    overseerMap.Initialize(); //ADD THIS LINE (OVERSEER)
    std::cout << "Number of tiles on map: " << overseerMap.size() << std::endl; //ADD THIS LINE (OVERSEER)
    std::cout << "Number of regions: " << overseerMap.getRegions().size() << std::endl; //ADD THIS LINE (OVERSEER)
```

That is it, now commandcenter is ready to be compiled with overseer:

* Go to commandcenter root directory
* Compile commandcenter `$ cmake . && make`.
* Run commandcenter

Wanted output is something like:

```
WaitJoinGame finished successfully.
Number of tiles on map: 26752
Number of regions: 18
```

## Overseer

Overseer has a lot more to offer then this and documentation for overseer can be found [here.](https://github.com/pimmen89/Overseer/tree/master/doc/html)