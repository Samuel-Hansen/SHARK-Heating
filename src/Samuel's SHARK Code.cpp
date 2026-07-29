//============================================================================
// Name        : Samuel's.cpp
// Author      : Samuel and ChatGPT
// Version     :
// Copyright   : Your copyright notice
// Description : Hello World in C++, Ansi-style
//============================================================================

#include <iostream>
#include <cmath>
#include <fstream>
#include <iterator>
#include <map>
#include <numeric>
#include <tuple>
#include <sstream>
#include <unordered_map>
#include <vector>
#include <algorithm>
#include <stdexcept>
#include <ostream>
#include <random>

#include <gsl/gsl_sf_lambert.h>


#include "utils.h"
#include "interpolator.h"
#include "heating.h"
#include "nfw_distribution.h"

using namespace std;

std::map<char, std::string> find_heating_tables(
	const std::string& environment_tables_dir)
{

	string prefix = "raise_";
	string tables = environment_tables_dir + "/" + prefix + "tables.txt";
	std::string line;
	std::map<char, std::string> environment_tables;

	std::ifstream f = open_file(tables);
	while (std::getline(f, line)) {
			trim(line);
			if (empty_or_comment(line)) {
				continue;
			}
			char environment = 'A';
			std::string table_fname;
			std::istringstream iss(line);

			iss >> line >> table_fname;
			trim(table_fname);

			environment_tables[environment] = table_fname;
	}
	f.close();
	return environment_tables;
}


HeatingData open_heating_file(const std::string& fname)
{
    std::ifstream file(fname);
    HeatingData data;

    if (!file.is_open()) {
        std::cout << "Could not open " << fname << '\n';
        return {};
    }

    std::string line;
    size_t row = 0;

    while (std::getline(file, line)) {
        if (line.empty() || line[0] == '#')
            continue;

        std::stringstream ss(line);
        double value;
        size_t col = 0;

        while (ss >> value) {

            if (row == 0) {
                // Header row: skip first value, store ages
                if (col > 0)
                    data.active_age_values.push_back(value);
            }
            else if (col == 0) {
                // First column: jet powers
                data.jet_power_values.push_back(value);
            }
            else {
                // Actual heating values
                data.heating_points.push_back(value);
            }

            ++col;
        }

        ++row;
    }

    return data;
}


double powerlaw(double u, double alpha, double xmin, double xmax){
	double x;
	if (alpha == 1){
		x = xmin * pow(xmax/xmin, u);
	}
	else{
		x = pow((pow(xmin,(1-alpha)) + u * (pow(xmax,(1-alpha)) - pow(xmin,(1-alpha)))) , (1/(1-alpha)));
	}


	return x;
}
int main() {
	string environment_tables_dir = "environment";
	string tables_idx;
	std::map<char, std::string> environment_tables = find_heating_tables(environment_tables_dir);
	string heating_tables_dir = "heating";
	//get the heating table and corresponding active ages and jet powers

	auto it = environment_tables.find('A');


//std::default_random_engine generator(exec_params.get_seed(galaxy));
	std::default_random_engine generator(54321);

	std::uniform_real_distribution<double> dist(0.0, 1.0);

	double u = dist(generator);
	double alpha = -0.97;
	double xmin = 5.5;
	double xmax = 8.5;

	double active_age = powerlaw(u, alpha, xmin, xmax);

	cout << log10(5 * 1e33 * 5)<< endl;
	if (it != environment_tables.end()) {
	    std::string fname = heating_tables_dir + "/" + it->second;
	    HeatingData heating_data = open_heating_file(fname);


	    double jet_power = 35;
		auto heating_interpolator = shark::Interpolator(
				heating_data.active_age_values,
				heating_data.jet_power_values,
				heating_data.heating_points);
		double q = heating_interpolator.get(active_age, jet_power);
		cout << q;
	}

	return 0;
}
