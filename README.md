# HockeyPlayersDetector

##### It uses computer vision methods to detect players on the hockey field.

```
Requirements:
- C++11 compiler;
- OpenCV3 built with ffmpeg support;
- Bgslibrary for OpenCV 3 ("libbgs.dylib" is built library for Mac OS X,
bgslibrary - folder that contains headers for built library).
```

```
How to use:
- Video should have "hockey_game.avi" name, you can change it in code;
- At the first start, you have to choose region of interest. It will be saved as "region.png" and only
white area will be applied to the video. You can whenever change region by pressing on the 'r' key;
- To close video frame, click any key.
```