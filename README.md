# BeatShot

Developed with Unreal Engine 5, this project aims to create a standalone FPS aim trainer that spawns targets based on user loaded music (in .mp3 or .ogg format) by analyzing the frequency of the music. There is an extensive amount of customization available to the player including:
- standard sensitivity settings (Same as Valorant sensitivity), video & audio settings
- two base gamemodes: continuously spawn targets (MultiBeat) or only one at a time (SingleBeat)
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
  - band limit threshold for each band channel
    - The threshold value changes how sensitive the analyzer is to beats within any given channel. Values greater than Threshold * Average trigger a target to attempt to spawn. higher threshold -> lower analyzer sensitivity -> less targets spawned

## **Download Development Build here (BeatShot-Windows.rar) [here](https://github.com/markoleptic/BeatShot/releases/download/v0.1.1/BeatShot-Windows-v0.1.1) or go to Releases**

## Getting .mp3/.ogg files:

1. To convert Spotify playlists into Youtube playlists, I use [Soundiiz](https://soundiiz.com/) (also works with Apple Music, etc.)
2. You can use [this application](https://github.com/shaked6540/YoutubePlaylistDownloader) to download entire playlists from YouTube. Download and install. *You might need restart the application after you open it for the first time for it to work correctly.*
3. Click on the cog wheel next to the "Insert a youtube link here" text.
4. Select where you want the files to be downloaded to at the top.
5. Under the Audio heading, make sure the box with "Convert videos to" is checked, then select either ogg or mp3.

## Collaboration / Copyright

I plan to release this as a free game on Steam in a few months. If you wish to collaborate, I can try to help provide you with more plugin files that might be necessary to run the game in the engine.

Note that copying my work and releasing a game based on my work is a violation of the Unreal Engine UELA and United States Copyright Laws.
