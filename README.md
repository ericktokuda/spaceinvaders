# spaceinvaders
Game inspired in space invaders

## Requirements
 * GCC
 * libGL

 In Ubuntu/Debian, just run:
 ```
 sudo apt-get install freeglut3-dev

 ```

## Compilation
```
gcc src/main.c -lGL -lGLU  -lglut -lm -o si
```

## Playing
```
./si
```

![Gameplay](https://raw.githubusercontent.com/tokudaek/spaceinvaders/master/img/si_video.gif)

## Troubleshooting
If your get a `cannot find lGL` error, try to locate the libGL.so file and check if its path is included in the LD\_LIBRARY\_PATH.
