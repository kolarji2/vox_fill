#include "vox_fill.hpp"

using namespace std;
Vox_fill::Vox_fill (int stepval,int radiusval)
{
	//init const
	step=stepval;
	radius=radiusval;
	cout<< "init" << endl;
	
}


vtkSmartPointer<vtkStructuredPoints> Vox_fill::load_voxel_vtk (string file_name,vector<vector<vector<int>>> &arr) {
	 // Create a structuredpoints
	cout << "Loading file: " << file_name << " ..." << endl;
	const char* fname=file_name.c_str();
	vtkSmartPointer<vtkStructuredPointsReader> r=
	vtkSmartPointer<vtkStructuredPointsReader>::New();
	r->SetFileName(fname);
	r->Update();
	vtkSmartPointer<vtkStructuredPoints> data = 
    vtkSmartPointer<vtkStructuredPoints>::New();
	
	data->SetDimensions(r->GetOutput()->GetDimensions());
    data->SetExtent(r->GetOutput()->GetExtent());
    data->SetScalarType(r->GetOutput()->GetScalarType(), r->GetOutputInformation(0));
    data->SetNumberOfScalarComponents(r->GetOutput()->GetNumberOfScalarComponents(),r->GetOutputInformation(0));
    data->GetPointData()->SetScalars(r->GetOutput()->GetPointData()->GetScalars());
  // Specify the size of the image data 
	int* dims = data->GetDimensions();
	int i,j,k,dim;
	dim=dims[0];
	arr.reserve(dim);
	arr.resize(dim);
	
	for (i=0;i<dim;i++) {
		arr[i].reserve(dim);
		arr[i].resize(dim);
	for (j=0;j<dim;j++) {
		arr[i][j].reserve(dim);
		arr[i][j].resize(dim);
	for (k=0;k<dim;k++) {
		if (data->GetScalarComponentAsDouble(i,j,k,0)>0)
			arr[i][j][k]=1;
		else
			arr[i][j][k]=0;		
	}		
	}		
	}
	return data;
	}
vector<vector<vector<int>>> Vox_fill::fill_holes_vtk_find_cells (vector<vector<vector<int>>> &arr) {
	cout << "Filling holes ..." << endl;
	int i,j,k,dim,nfill;
	bool isfoamcell;
	dim=arr.size();
	nfill=0;
	vector<vector<vector<int>>> farr=alloc_arr3d(dim,1);
	for (i=0;i<dim;i+=step) {
	for (j=0;j<dim;j+=step) {
	for (k=0;k<dim;k+=step) {		
		if (arr[i][j][k]==0) {
			vector<int> pos {i, j,k};
			isfoamcell=is_foam_cell(arr,pos);
			if (isfoamcell) {
				nfill+=1;
				fill_area(farr,arr,pos);
			}
		}
	}
	}
	}
	cout << "Found " << nfill << " cells" << endl;
	return farr;
	}

void Vox_fill::fill_area (vector<vector<vector<int>>> &fill0,vector<vector<vector<int>>> &fill1,vector<int> c){
	int i,j,dim;
	dim=fill0.size();
	fill1[c[0]][c[1]][c[2]] = 1;
    fill0[c[0]][c[1]][c[2]] = 0;
    queue<vector<int>>q;
    q.push(c);
    while (!q.empty()) {
        vector<int> p = q.front();
        q.pop();
        for (i=0;i<=2;i++){
			for (j=-1;j<=1;j+=2){
				vector<int> p1 {p[0], p[1],p[2]};
				p1[i]=mod(p1[i]+j,dim);
				if (fill1[p1[0]][p1[1]][p1[2]] == 0 && fill0[p1[0]][p1[1]][p1[2]] == 1) {
                    fill0[p1[0]][p1[1]][p1[2]] = 0;
                    fill1[p1[0]][p1[1]][p1[2]] = 1;
                    q.push(p1);
				}
		}
		}
	}
	}
bool Vox_fill::is_foam_cell(vector<vector<vector<int>>> &arr,vector<int> c){
	int i0,i1,j0,j1,k0,k1,i,j,k,dim;
	i0 = c[0];
    j0 = c[1];
    k0 = c[2];
    dim=arr.size();
    for (i=-radius;i<=radius;i++) {
	for (j=-radius;j<=radius;j++) {
	for (k=-radius;k<=radius;k++) {
		i1=mod(i0+i,dim);
        j1=mod(j0+j,dim);
        k1=mod(k0+k,dim);
        if (arr[i1][j1][k1]==1)
			return false;
	}
	}
	}          
    return true;
	}

vector<vector<vector<int>>> Vox_fill::alloc_arr3d(int dim,int val) {
	int i,j,k;
	vector<vector<vector<int>>> arr;
	arr.reserve(dim);
	arr.resize(dim);	
	for (i=0;i<dim;i++) {
		arr[i].reserve(dim);
		arr[i].resize(dim);
		for (j=0;j<dim;j++) {
			arr[i][j].reserve(dim);
			arr[i][j].resize(dim);
			for (k=0;k<dim;k++) {
					arr[i][j][k]=val;			
			}		
		}		
	}
	return arr;
}
void Vox_fill::write_to_file(string file_name, vector<vector<vector<int>>> arr) {
	cout << "Saving output to file: " << file_name << " ..." << endl;
	int i,j,k,dim;
	double setvox=0;
    double totvox=0;
    dim=arr.size();
    ofstream fout (file_name);
    fout << dim << endl << '#' << endl;
    for (i=0;i<dim;i++) {
	for (j=0;j<dim;j++) {
	for (k=0;k<dim;k++) {       
		if (arr[i][j][k]==1)
			setvox += 1;
		totvox+=1;
		fout<<arr[i][j][k];
    }
    fout << endl;
    }
    fout << '#' << endl;
    }
    cout << "Foam porosity: " << 1-setvox/totvox << endl;	
	}

void Vox_fill::write_to_vtk(string file_name,vtkSmartPointer<vtkStructuredPoints> data, vector<vector<vector<int>>> arr){
	cout << "Saving vtk output to file: " << file_name << " ..." << endl;
	const char* fname=file_name.c_str();
	vtkSmartPointer<vtkStructuredPointsWriter> w=
	vtkSmartPointer<vtkStructuredPointsWriter>::New();
	w->SetFileName(fname);
	int i,j,k,dim;
	double setvox=0;
    double totvox=0;
    dim=arr.size();
    for (i=0;i<dim;i++) {
	for (j=0;j<dim;j++) {
	for (k=0;k<dim;k++) {       
		if (arr[i][j][k]==1) {
			data->SetScalarComponentFromDouble(i,j,k,0,128);
			setvox += 1;
		} else
			data->SetScalarComponentFromDouble(i, j, k, 0, 0);
		totvox+=1;
    }}}
    w->SetInputData(data);
    w->Write();
    cout << "Foam porosity: " << 1-setvox/totvox << endl;
}
