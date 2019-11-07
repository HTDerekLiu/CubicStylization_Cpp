# Cubic Stylization
Cubic stylization is a 3D stylization tool. Unlike image stylization (2D to 2D) and non-photorealistic rendering (3D to 2D), cubic stylization is a 3D to 3D stylization algorithm which takes a manifold triangle mesh as the input and outputs a cubified triangle mesh. 

This is a C++ implementation based on "[Cubic Stylization](https://www.dgp.toronto.edu/projects/cubic-stylization/)" by [Hsueh-Ti Derek Liu](https://www.dgp.toronto.edu/~hsuehtil/) and [Alec Jacobson](https://www.cs.toronto.edu/~jacobson/). 

## Download
We can download the application by cloning the repository ***with the RECURSIVE option***
```
git clone --recursive https://github.com/HTDerekLiu/CubicStylization_Cpp.git
```

### 1. ImGui version 
Once we clone the cubic stylization code, we offer an [ImGui](https://github.com/ocornut/imgui) version in folder `cubicStylization_ImGui` for one to play with the stylization interactively in the GUI. To compile the application, please type these commands in the terminal
```
cd CubicStylization_Cpp/cubicStylization_ImGui
mkdir build
cd build
cmake ..
make
```
This will create the excutable of the cubic stylization. To start the application, please run
```
./cubicStylization_bin [meshName]
```
where the example meshes are provided in `/CubicStylization_Cpp/meshes`. Instructions of how to control the cubic stylization is listed on the side of the GUI.

### 2. Command line version 
We also offer a command line version in folder `cubicStylization_cmd` for one to cubify shapes directly. To compile the application, please type these commands in the terminal
```
cd CubicStylization_Cpp/cubicStylization_cmd
mkdir build
cd build
cmake ..
make
```
This will create the excutable of the cubic stylization. To start the application, please run
```
./cubicStylization_bin [meshName] [lambda]
```
where the example meshes are provided in `/CubicStylization_Cpp/meshes` and the _lambda_ parameter is the _cubeness_. Both demos are based on the Algorithm 1 in the [paper](https://arxiv.org/abs/1910.02926). 

Please visit [CubicStylization_MATLAB](https://github.com/HTDerekLiu/CubicStylization_MATLAB) for a prototype implementation in MATLAB. Our code is licensed under [MPL2](https://www.mozilla.org/en-US/MPL/2.0/). If any questions, please contact Hsueh-Ti Derek Liu (hsuehtil@cs.toronto.edu).

### Bibtex
```
@article{Liu:CubicStyle:2019,
  title = {Cubic Stylization},
  author = {Hsueh-Ti Derek Liu and Alec Jacobson},
  year = {2019},
  journal = {ACM Transactions on Graphics}, 
}
```
