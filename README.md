# NoiseBench

NoiseBench is a 2-versions noise algorithm benchmark. It allows PCs to test their sampling performances, It is based off ashima/webgl-noise's benchmark and demo but a bit more intense on their hosts. This project is available for Debian-based Linux PCs only.

# Terms used here

**Noise :** Noise is an algorithm used to generate pseudo-random numbers when the language that you are using does not have truly random numbers.

**Pseudo-random :** Pseudo-random is 50% random, It gives a random feeling but is not truly randomical generation.

# Explanation (Graphics Pros only)

So, how this project works ?
And how can I update some stuff in it ?

Well, The project uses multiple noise algorithms to make procedural textures that are used instead of real sampler2D or sampler3D textures, then they are mixed up and processed to give the final color of each pixel.

The animation and factors for the noise algo is used by texture coordinates and an uniform variable that takes the time from the main.c's ```glutSetOption(GLUT_ELAPSED_TIME);``` function.

For the v0.2, a sphere is drawed using Quads, which are more computationally intensive than Triangles.

Then a fire noise (kinda like the Sun surface) is generated using 4 different noise types : cellular, simplex (normal and gradient), and classic Perlin noise then processed and added to fire color and then pushed to ```gl_FragColor```.

# Installation

*For version 0.1*:

```sh
sudo apt install git make --no-install-recommends -y
git clone https://github.com/AyoubCoding21/NoiseBench
cd NoiseBench/NoiseBench-0.1/
make
```

*For version 0.2*:

```sh
sudo apt install git make --no-install-recommends -y
git clone https://github.com/AyoubCoding21/NoiseBench
cd NoiseBench/NoiseBench-0.2/
make
```

# ***ENJOY !!!!***
