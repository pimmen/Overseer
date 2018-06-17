## This project has been [moved to Gitlab](https://gitlab.com/OverStarcraft/Overseer). Thanks everyone for the attention we've gotten here, we hope you'll follow and stay updated! /mejan, pimmen

# Overseer

![alt text][img1]

Library for analyzing Starcraft 2 maps by region decomposition. Based on the [Brood War Easy Map architecture][1] created by Igor Dimitrijevic. It uses the MIT license.

## Getting started

Demo for Commandcenter [is found here.][2] Which is a very short demo just to get started.

Include the file `MapImpl.h` into your project to get started.

`#include "Overseer/src/MapImpl.h"`

You need to pass a pointer to your Agent to the map to have it fully configured. Then you need to call `Intialize()` to construct the map.
Now you're good to go! This is how it would look on Interloper LE

```c++
{
	Overseer::MapImpl map;

	map.setBot(&bot); //Pass a pointer to your sc2::Agent
	map.Initialize(); //Intialize the map

	std::cout << "Number of tiles on map: " << map.size() << std::endl;
	std::cout << "Number of regions: " << map.getRegions().size() << std::endl;
}
```

Example output:

```
Number of tiles on map: 26752
Number of regions: 18
```

If you want the number of `ChokePoint` you have to check for each region pair since a pair of regions could have multiple `ChokePoint`

## Project status

Overseer is currently under construction. Feel free to make a pull request!

## Documentation

The documentation is for local use only and can be [found here.][3] To open it on your local machine go into the doc/html/ and open index.html in your webbrowser.

## License

The license for this software (Overseer) can be found [here.][4]

### Third party software license

* [Boost Software License Version 1.0](http://www.boost.org/LICENSE_1_0.txt): Used in Spatial c++ library

[1]: http://bwem.sourceforge.net/
[2]: https://github.com/pimmen89/Overseer/blob/master/demo/commandcenter.md
[3]: https://mejan.github.io
[4]: https://github.com/pimmen89/Overseer/blob/master/LICENSE.md
[img1]: http://images.ctrustnetwork.com/static_pages/gaming/starcraft/unit_images_white/starcraft.2.overseer.png "Image of overseer"
