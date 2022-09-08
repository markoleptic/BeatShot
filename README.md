# BeatShot

Developed with Unreal Engine 5, this project aims to create a standalone FPS aim trainer that spawns targets based on user loaded music by analyzing amplitude and frequency. There is an extensive amount of customization available to the player including:
- standard sensitivity settings (Same as Valorant sensitivity), video & audio settings
- two base gamemodes: continuously spawn targets (MultiBeat) and only one at a time (SingleBeat)
- customizing the gamemodes using:
  - headshot height only mode
  - time it takes for target to reach peak color (green) / this is also the point in time that syncs with a beat
  - max target lifespan
  - min distance between targets
  - spawn area height/width
  - target spawn cooldown
  - mininum and maximum size of targets
- customizing the audio analysis using:
  - 1-4 user defined frequency band channels (e.g. 0-87 Hz for a bass channel)
  - band limit threshold for each band channel (The threshold value changes how sensitive the analyzer is to beats within any given channel. Values greater than Threshold * Average trigger this beat. higher threshold -> lower sensitivity -> less targets spawned)
  
Download Development Build from Releases (BeatShot-Windows.rar) [here](https://github.com/markoleptic/BeatShot/releases/tag/v0.1.0)

## Collaboration / Copyright

I plan to release this as a free game on Steam in a few months. If you wish to collaborate, I can try to help provide you with more plugin files that might be necessary to run the game in the engine.

Note that copying my work and releasing a game based on my work is a violation of the Unreal Engine UELA and United States Copyright Laws.
