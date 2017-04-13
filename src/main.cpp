#include "vox_fill.hpp"
#include <vector>
#include <iostream>
#include <fstream>
#include <string>
#include <sstream>
#include <exception>
#include <boost/program_options.hpp>
#include <iterator>
// Author: Jiri Kolar
// April 2017
using namespace std;
namespace po = boost::program_options;

int main (int argc, char* argv[])
{
	try {
		po::options_description desc ("Allowed options");
		desc.add_options()
		("help,h", "produce help message")
		("input-file,i", po::value<string>(), "input .vtk file where information about foam structure is stored.")
		("output-file,o", po::value<string>()->default_value ("foam_tmp.vtk"), "output file")
		("radius,r", po::value<int>()->default_value (5), "Radius for cell recognition")
		("step,s", po::value<int>()->default_value (10), "Step for foam walking")
		("format,f", po::value<string>()->default_value ("vtk"), "Output format, ascii vtk or .txt file with slices [vtk|txt]")
		;
		// -p  // plot gnu plot graph NOT implemented
		
		po::variables_map vm;
		po::store (po::parse_command_line (argc, argv, desc), vm);
		po::notify (vm);

		if (vm.count ("help")) {
			cout << desc << "\n";
			return 0;
		}
		if (!vm.count ("input-file")) {
			cout << ("You have to to specify at least input file by -i foam.vtk") << endl;
			cout << ("\t Use --help for more information about usage") << endl;
			return 0;
		}
	
		Vox_fill voxfill(vm["step"].as<int>(),vm["radius"].as<int>());

		if (vm.count ("input-file")) {
			vector<vector<vector<int>>> arr;
			auto data=voxfill.load_voxel_vtk(vm["input-file"].as<string>(),arr);
			auto farr=voxfill.fill_holes_vtk(arr);
			if (vm["format"].as<string>()=="txt")				
				voxfill.write_to_file(vm["output-file"].as<string>(),farr);
			else
				voxfill.write_to_vtk(vm["output-file"].as<string>(),data,farr);
		} 
	} catch (exception& e) {
		cout << "An exception occurred: " << endl << e.what() << endl;
	}
	return 0;
}
