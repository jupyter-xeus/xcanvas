# xcanvas: The HTML 2D Canvas API brought to xeus-cling

## Install

### From sources (using conda)

First install dependencies (here using conda):

```bash
conda install cmake xwidgets xeus xeus-cling ipycanvas -c conda-forge
```

Then compile:

```bash
mkdir build
cd build
cmake -D CMAKE_INSTALL_PREFIX=$CONDA_PREFIX
make install
```
