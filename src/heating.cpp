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


#include "interpolator.h"
#include "heating.h"
#include "nfw_distribution.h"
#include "logging.h"
#include "naming_convention.h"
#include "options.h"
#include "utils.h"

namespace shark{

std::map<char, std::string> find_heating_tables(
	const std::string& environment_tables_dir)
{

	std::string prefix = "raise_";
	std::string tables = environment_tables_dir + "/" + prefix + "tables.txt";
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
}
