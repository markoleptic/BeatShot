# BeatShot

This project is a standalone FPS aim trainer that controls target behavior by analyzing user selected music in real time.

**[PURCHASE ON STEAM](https://store.steampowered.com/app/2126580/BeatShot/)**

In Game                    |  Customize Game Mode Menu
:-------------------------:|:-------------------------:
![](https://user-images.githubusercontent.com/86213229/226467498-79851ec5-62b9-4614-a804-eccbb0c1c623.png)  |  ![](https://user-images.githubusercontent.com/86213229/226468050-38d159c3-f3b4-40cf-8c24-aa9a1836879c.png)

## Features
- standard sensitivity settings (Options for CS:GO and Valorant), video & audio settings, target color settings, crosshair settings
- multiple default game modes
- Full CS:GO AK-47 recoil pattern (not 100% accurate), which is togglable use in game wall menu
- Full auto/semi-auto fire modes

### BeatShot is designed to enable players to have full control over the game
- Create custom game modes using any default game mode as a template, or from another custom game mode
- Preview the game mode in the custom game mode editor
- 40+ settings to customize from which all default game modes are derived

### Customize the audio analysis using:
- User defined frequency band channels (e.g. 0-87 Hz for a bass channel)
- band limit threshold for each band channel
    - The threshold value changes how sensitive the analyzer is to beats within any given channel. Values greater than Threshold * Average trigger a target to attempt to spawn. 
    - higher threshold -> lower analyzer sensitivity -> less targets spawned

## Getting .mp3/.ogg files:
1. To convert Spotify playlists into Youtube playlists, I use [Soundiiz](https://soundiiz.com/) (also works with Apple Music, etc.)
2. You can use [this application](https://github.com/shaked6540/YoutubePlaylistDownloader) to download entire playlists from YouTube. Download and install. *You might need restart the application after you open it for the first time for it to work correctly.*
3. Click on the cog wheel next to the "Insert a youtube link here" text.
4. Select where you want the files to be downloaded to at the top.
5. Under the Audio heading, make sure the box with "Convert videos to" is checked, then select either ogg or mp3.

## More In-Game Screenshots:

Audio Analyzer Settings    |  SongSlection
:-------------------------:|:-------------------------:
![](https://user-images.githubusercontent.com/86213229/226468236-f9458581-675d-43b8-80ee-d2684edda763.png) | ![](https://user-images.githubusercontent.com/86213229/226467354-7a28e9aa-c9ed-428d-8a2a-7c7adf98eb7b.png)
Website Integration        |  NightTime
![](https://user-images.githubusercontent.com/86213229/226467406-3a05d922-05e7-4d52-a722-0b3ebb071c4d.png) | ![](https://user-images.githubusercontent.com/86213229/226467534-aa06b55a-5f9f-4620-a9da-70a2927b8352.png)

## Collaboration / Copyright
This project is on GitHub because Unreal Engine is real handful to learn, so perhaps there's some code that I've used in this project that could help someone else out. If you wish to collaborate or want to reach out for any reason, you can contact me on Discord @Markoleptic#6174.

Note that copying my work and releasing a game based on my work is a violation of the Unreal Engine UELA and United States Copyright Laws.
