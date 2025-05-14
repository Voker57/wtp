# But The People - civ4col mod

Fork of We The People, intended to keep expectations from ages old engine and busy developers low.

Planned changes:

* Cut down the number of resources to the minumum. To be removed:
* * Cassava -> Food
* * Charcoal -> Lumber
* * Cochineal -> Dyes
* * Flax
* * Fruit, Fruit brandy -> Food
* * Geese + Chicken -> Poultry
* * Goats
* * Hardwood -> More lumber
* * Indigo -> Dyes
* * Logwood
* * Luxury fur -> just more fur
* * Luxury goods
* * Peanuts -> Food
* * Peat
* * Pigs -> Cattle
* * Pottery
* * Rapeseed/oil
* * Rice -> Food
* * Roasted Peanuts
* * Ropes -> Riggery?
* * Trade goods: replace w/household goods
* * Vanilla pods
* * Wild Bird Feathers
* * Yerba leaves/mate
* Simplfy remaining chains:
* * Provisions: Made from food (lossy), made into food (lossy).
* * Food cannot be transported
* * Ships are made of lumber + riggery
* * Empty places are taken by similar goods, produced at worse rates.


## Important Installation Instructions
This branch uses Intel Threading Building blocks to achieve concurrent AI calculations in order to speed up the AI (inter)turn. 
Before you start:

**copy tbb.dll and tbbmalloc.dll from "Project Files\tbb" to the directory where Colonization.exe resides!**

Otherwise the mod will not work!

## Known Issues
* Combat Forecast sometimes not shown ([#557](https://github.com/We-the-People-civ4col-mod/Mod/issues/557))
* On very large maps, zooming out might cause the terrain appear completely black ([#723](https://github.com/We-the-People-civ4col-mod/Mod/issues/723))


