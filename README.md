# PKU Compiler Course

- Course Documentation: https://pku-minic.github.io/online-doc/#/
- Environment Repository (Upstream): https://github.com/pku-minic/compiler-dev

## Quick start

```bash
docker pull maxxing/compiler-dev  # pulling image
```

```bash
docker run -it --rm -v "Z:\Github Repos\PKU-Compiler-Course:/workspace" maxxing/compiler-dev bash  # mount this host directory (e.g.) into the container

cd workspace
```

```bash
# config and build
cmake -DCMAKE_BUILD_TYPE=Debug -B build
cmake --build build
```

> **Note:**  
> If you are using Linux and the repository is on a mounted external hard drive, Docker may not have the necessary permissions when running the mount command above, and however, it won't tell you about it. fuck this problem wasted 2hs of my life.