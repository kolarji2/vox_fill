#ifndef vox_fill_hpp
#define vox_fill_hpp
#include <vector>
#include <queue>
#include <algorithm>
#include <iostream>
#include <fstream>
#include <string>
#include <sstream>
#include <memory>
//VTK library
#include <vtkSmartPointer.h>
#include "vtkPointData.h"
#include "vtkStructuredPoints.h"
#include "vtkStructuredPointsReader.h"
#include "vtkStructuredPointsWriter.h"

using namespace std;

class Vox_fill
{
private:
	int step;
	int radius;
public:

	Vox_fill (int stepval,int radiusval);
	vtkSmartPointer<vtkStructuredPoints> load_voxel_vtk (string file_name,vector<vector<vector<int>>> &arr);
	vector<vector<vector<int>>> fill_holes_vtk (vector<vector<vector<int>>> &arr);
	void write_to_vtk(string file_name,vtkSmartPointer<vtkStructuredPoints> data, vector<vector<vector<int>>> arr);
	void write_to_file(string file_name, vector<vector<vector<int>>> arr);

private:
	vector<vector<vector<int>>> alloc_arr3d(int dim,int val);
	bool is_foam_cell(vector<vector<vector<int>>> &arr,vector<int> c);
	void fill_area (vector<vector<vector<int>>> &fill0,vector<vector<vector<int>>> &fill1,vector<int> c);
	inline int mod(int x,int d) {
		if (x>=0)
			return x%d;
		else
			return x+d;		
		}
};


#endif
