# PKU Compiler Course

- Course Documentation: https://pku-minic.github.io/online-doc/#/
- Environment Repository (Upstream): https://github.com/pku-minic/compiler-dev

## Quick start

```bash
# pull image
docker pull maxxing/compiler-dev
```

```bash
# mount
docker run -it --rm -v "/home/uednd/Code/PKU-Compiler-Course:/workspace" maxxing/compiler-dev bash
cd /workspace
```

```bash
# config and build
cmake -DCMAKE_BUILD_TYPE=Debug -B build && cmake --build build
```

```bash
# test
./build/compiler test/hello.c
```

> **Note:**  
> If you are using Linux and the repository is on a mounted external hard drive, Docker may not have the necessary permissions when running the mount command above, and however, it won't tell you about it. This shit wasted 2hs of my life.