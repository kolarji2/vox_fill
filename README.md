# vox_fill
Small application for filling small empty holes in surface 3D voxel model of foam. In surface voxel model, voxels are drawn only on boundaries of cells, so struts and edges must be filled up. Input vtk file can be created by app anfoam_rec.

## Installation
	cmake .
	make
	sudo cp vox_fill /usr/local/bin/

## Usage
 use --help option to inform about usage
