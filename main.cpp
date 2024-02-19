#include <stdexcept>
#include <iostream>
#include <vector>
#include <string>
#include <fstream>
#include <algorithm>
#include <unordered_map>

#include "table.hpp"

//std::vector<std::unique_ptr<AC>> cells;
using R = std::vector<std::string>;
using S = std::vector<std::string>;
/*
class AC {
public:
	virtual ~AC() {}
	virtual bool compare(const std::string& a, const std::string& b) const = 0;
};

class C_EQ : public AC {
	virtual bool compare(const std::string& a, const std::string& b) const override {
		return a == b;
	}
};
*/

void join(table& result, const table& left, std::size_t left_idx, const table& right, std::size_t right_idx) {
	R left_header = left.get_header();
	S right_header = right.get_header();
	int i = 0;
	for (const auto& l : left_header) {
		std::string column = "R." + l;
		result.add_column(column, i);
		i++;
	}
	for (const auto& r : right_header) {
		std::string column = "S." + r;
		result.add_column(column, i);
		i++;
	}
	std::unordered_multimap<std::string, std::size_t> m;
	for (size_t i = 0; i < left.n_row(); ++i) {
		m.insert({ left[i][left_idx], i });
	}
	for (size_t i = 0; i < right.n_row(); ++i) {
		auto range = m.equal_range(right[i][right_idx]);
		for (auto& it = range.first; it != range.second; ++it) {
			row_type row;
			for (auto& r : left[it->second]) {
				row.emplace_back(r);
			}
			for (auto& s : right[i]) {
				row.emplace_back(s);
			}
			result.add_row(move(row));
		}
	}
}
int main(int argc, char** argv) {
	try {
		if (argc != 7)
		{
			std::cout << "Invalid number of arguments." << std::endl;
			return 1;
		}
		std::ifstream isR(argv[argc-3]);
		if (isR.fail()) {
			std::cout << "File " << argv[argc - 3] << "does not exist." << std::endl;
			return 1;
		}
		std::ifstream isS(argv[argc - 2]);
		if (isS.fail()) {
			std::cout << "File " << argv[argc - 2] << "does not exist." << std::endl;
			return 1;
		}
		std::ofstream os(argv[argc - 1]);
		table dataR;
		dataR.read_csv(isR);
		if (dataR.n_col() == 0) {
			std::cout << "Can't read from fie" << argv[argc - 3] << std::endl;
			return 1;
		}
		table dataS;
		dataS.read_csv(isS);
		if (dataS.n_col() == 0) {
			std::cout << "Can't read from fie" << argv[argc - 2] << std::endl;
			return 1;
		}
		std::string columnR = argv[2];
		std::size_t columnR_idx = dataR.get_column_index(columnR);
		std::string columnS = argv[3];
		std::size_t columnS_idx = dataS.get_column_index(columnS);
		table joined_data;
		join(joined_data, dataR, columnR_idx, dataS, columnS_idx);

		joined_data.print_csv(os);
	}
	catch (const std::exception& e) {
		std::cout << e.what() << std::endl;
	}
	catch (...) {
		std::cout << "Unknown exception." << std::endl;
	}
	return 0;
}
