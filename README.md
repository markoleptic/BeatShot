# BeatShot

BeatShot is rhythm-based aim-trainer that syncs targets to your music. This repository contains the `Source` directory of Unreal Engine project. To find out more about the game and its features, you can view it on **[Steam](https://store.steampowered.com/app/2126580/BeatShot/)**. You can also checkout the BeatShot [website](https://beatshot.gg) and [developer blog](https://beatshot.gg/devblog).

<table>
  <tr>
    <td width=50%>
        <img src="https://github.com/markoleptic/BeatShot/assets/86213229/1ccbd74c-42f2-43fc-a23a-a25570da2c9f" alt="Game">
    </td>
    <td width=50%>
        <img src="https://github.com/markoleptic/BeatShot/assets/86213229/98558e7c-761d-45b0-93af-3cfc87ba081f" alt="Menu">
    </td>
  </tr>
</table>

**Warning if cloning:** There will be several module depedencies missing including:  
- The audio analyzer, which is a [paid plugin](https://www.unrealengine.com/marketplace/en-US/product/audio-analyzer).
- DLSS, which is installed in the engine I'm using but available for [free](https://www.unrealengine.com/marketplace/en-US/product/nvidia-dlss).
- Common Loading Screen, which I found in the [Lyra Sample Game](https://docs.unrealengine.com/5.3/en-US/lyra-sample-game-in-unreal-engine/) and modified slightly. I plan to upload this in a separate repository at some point.

## Project Structure
The source directory is split into three project-specific modules mainly to create a cleaner looking project structure. Two third party modules, which are basically just libraries, are also included.

**BeatShot:** Contains the bulk of the code for running the game that could not be separated into other modules.  

**BeatShotGlobal:** Independent module mostly used for data structures, loading and saving, and HTTP requests.  

**UserInterface:** Contains all UMG/Slate user interface classes used in the game.  

**LocalBoost:** The Boost library included in the engine was too old to support NumCpp, so I added a more recent version.  

**NumCpp:** Basically [Numpy for C++](https://github.com/dpilger26/NumCpp), mainly needed because I didn't want to manually perform certain matrix operations with Unreal data structures.  

## Getting .mp3/.ogg files:

1. To convert Spotify playlists into Youtube playlists, I use [Soundiiz](https://soundiiz.com/) (also works with Apple
   Music, etc.)
2. You can use [this application](https://github.com/shaked6540/YoutubePlaylistDownloader) to download entire playlists
   from YouTube. Download and install. *You might need restart the application after you open it for the first time for
   it to work correctly.*
3. Click on the cog wheel next to the "Insert a youtube link here" text.
4. Select where you want the files to be downloaded to at the top.
5. Under the Audio heading, make sure the box with "Convert videos to" is checked, then select either ogg or mp3.

## Collaboration / Copyright

This project is on GitHub because Unreal Engine is real handful to learn, so perhaps there's some code that I've used in
this project that could help someone else out. If you wish to collaborate or want to reach out for any reason, you can
contact me on Discord @markoleptic.

Note that copying my work and releasing a game based on my work is a violation of the Unreal Engine UELA and United
States Copyright Laws.
