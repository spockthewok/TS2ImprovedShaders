# TS2 Improved Shaders
## About
A patch for The Sims 2 that adds new shader parameters to support extended shader functionality and fixes several shader-related bugs.

This plugin was primarily developed for Christaskyy's Improved Shaders mod.

## Requirements
- The Sims 2: Ultimate Collection <ins>**OR**</ins> The Sims 2 disc version with all EPs and SPs.
- [Sims2RPC](https://modthesims.info/d/648220/sims2rpc-modded-sims-2-launcher-for-mansion-and-garden.html) <ins>**OR**</ins>
[Ultimate ASI Loader](https://github.com/ThirteenAG/Ultimate-ASI-Loader).

> [!NOTE]
> Christaskyy's mod is not required for this plugin to work.

## New Shader Parameters
### terrain.matShad
| Parameter | Type | Description |
| :-------: | :--: | :---------: |
| `lotXScale` | Integer | Width of the current lot in tiles. |
| `lotYScale` | Integer | Height of the current lot in tiles. |
| `isBeachLot` | Boolean | Whether the current lot is a beach lot. |

### lotSkirt.matShad
| Parameter | Type | Description |
| :-------: | :--: | :---------: |
| `lotZPos` | Float | How high the current lot is above sea level. |
| `lotXOffset` | Float | The current lot's x-coordinate relative the world origin (0, 0). |
| `lotYOffset` | Float | The current lot's y-coordinate relative the world origin (0, 0). |

## Bug Fixes
- Fixed building window lights failing to update when using a lighting mod that enables dawn/dusk lighting states, causing them get stuck
  either on or off.
- Also fixed a [similar issue](https://lowedeus.tumblr.com/post/158006347041/badchriss-hmsomething-weird-is-going-on-with) with the neighbourhood glow
  material added by [Better Nightlife](https://www.tumblr.com/criquette-was-here/157941568866/better-nightlife-ts2-custom-hood-deco-night).

## Thanks
[Christaskyy](https://www.tumblr.com/christaskyy), for asking me to create this.

[LazyDuchess](https://github.com/LazyDuchess), for the hooking code used in this mod, and for [RPCLib](https://github.com/LazyDuchess/RPCLib).