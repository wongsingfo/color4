
## How to build

```
git clone git@github.com:wongsingfo/color4.git
cd color4
git submodule init
git submodule update
mkdir build
cmake -S . -B build
cmake --build build
cmake --build build --target test
```

