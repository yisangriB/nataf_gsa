
#include "lib_json/json.hpp"
#include <fstream>
#include <sstream>
#include <string>
#include <iostream>
#include <algorithm>
#include <vector>
#include <string>
#include <regex>

using json = nlohmann::json;


void Getpnames(std::string distname, std::string optname, std::vector<std::string>& par_char);

void readjson(int &nmc, int& nrv, int& ng, int &rseed, std::string& UQ_method, std::vector<std::string>& get_distnames, std::vector<std::vector<double>>& get_vals,
				std::vector<std::string>& get_opts, std::vector<std::string>& get_rvnames, std::vector<double>& get_corr, std::vector<std::vector<double>>& get_add,
				std::vector<std::vector<double>>& get_groups, std::string& get_workdir)
{
	// === Working directory???
	// === Should this program clean-up the working directory??
	// === Error format

	get_workdir = "C:/Users/yisan/Documents/quoFEM/LocalWorkDir";
		
	ng = 1; // Number of outputs
	//get_corr = { 1.0, 0.2, 0.2, 0.2,   0.2, 1.0, 0.2, 0.2 ,  0.2, 0.2, 1.0, 0.2,   0.2, 0.2, 0.2, 1.0}; 
	//get_corr = { 1.0, 0.0, 0.0, 0.0,   0.0, 1.0, 0.0, 0.0 ,  0.0, 0.0, 1.0, 0.0,   0.0, 0.0, 0.0, 1.0 };
	//get_opts = { "MOM","MOM","MOM","MOM" };

	// === read json
	std::ifstream myfile(get_workdir+"/tmp.SimCenter/templatedir/dakota.json");
	if (!myfile.is_open()) {
		//ERROR
		std::ofstream errfile(get_workdir + "/errorLog.txt");
		errfile << "Unable to open dakota.json" << std::endl;
		errfile.close();
		exit(1);
	}


	json UQjson = json::parse(myfile);

	// === get variables
	nmc = UQjson["UQ_Method"]["samplingMethodData"]["samples"];
	rseed = UQjson["UQ_Method"]["samplingMethodData"]["seed"];
	UQ_method  = UQjson["UQ_Method"]["samplingMethodData"]["method"];
	//get_workdir = UQjson["workingDir"];


	// === Specify parameters in each distributions.
	nrv = 0;
	for (auto& elem : UQjson["randomVariables"])
	{
		// name of distribution
		get_distnames.push_back(elem["distribution"]);		
		// name of random variable
		get_rvnames.push_back(elem["name"]);

		// type of inputs (PAR,MOM,DAT)
		std::string curType = elem["inputType"];
		get_opts.push_back(curType.substr(0,3));
		for (int i = 0; i < 3; i++) {
			get_opts[nrv][i] = toupper(get_opts[nrv][i]);
		}

		std::vector<std::string> pnames;
		Getpnames(get_distnames[nrv], get_opts[nrv], pnames); // get parameter names from dist name
		std::vector<double> addDefault{ 0.0, 0.0 };
		// IF Data
		if (get_opts[nrv].compare("DAT") == 0) {

			std::string directory = elem["datafile"];
			std::ifstream data_table(directory);
			if (!data_table.is_open()) {
				//ERROR
				std::ofstream errfile(get_workdir + "/errorLog.txt");
				errfile << "There was a problem opening the input file at " << directory << std::endl;
				errfile.close();
				exit(1);
			}

			std::vector<double> vals_temp;
			double samps = 0.0;
			while (data_table >> samps) {
				vals_temp.push_back(samps);
			}
			get_vals.push_back(vals_temp);
			data_table.close();

			if (get_distnames[nrv].compare("binomial") == 0) {
				get_add.push_back({ elem["n"],0.0 });
			}
			else if (get_distnames[nrv].compare("beta") == 0) {
				get_add.push_back({ elem["lower"],elem["upper"] });

			} 
			else
			{
				get_add.push_back(addDefault);
			}
		}
		else
		{ 

			
			if (get_distnames[nrv].compare("discrete") == 0) {
				std::vector<double> vals_temp;
				for (int i=0 ; i< elem[pnames[0]].size();i++)
				{
					vals_temp.push_back(elem[pnames[0]][i]);
					vals_temp.push_back(elem[pnames[1]][i]);
				}
				get_vals.push_back(vals_temp);
				//get_vals.push_back(elem[pnames[0]]);
				//get_vals.push_back(elem[pnames[1]]);
				//get_vals = elem[pnames[0]];
				//get_vals.insert(get_vals.end(), elem[pnames[0]].begin(), elem[pnames[0]].end());
				//get_vals.insert(get_vals.end(), elem[pnames[1]].begin(), elem[pnames[1]].end());
			}
			else
			{
				std::vector<double> vals_temp;
				for (auto& pn : pnames)
				{
					vals_temp.push_back(elem[pn]); // get parameter value from dist & parameter name
				}
				get_vals.push_back(vals_temp);	
			}
				
			get_add.push_back(addDefault);
		}
		nrv++;
	}

	// === get correlation matrix

	//get_corr = { 1.0, 0.2, 0.2, 1.0 }; // {{row1}, {row2},...}
	if (UQjson.find("correlationMatrix") != UQjson.end()) {
		for (int i=0; i<nrv*nrv; i++) {
			get_corr.push_back(UQjson["correlationMatrix"][i]);
		}
	} 
	else
	{
		for (int i = 0; i < nrv * nrv; i++) {
			get_corr.push_back(0);
		}
		for (int i = 0; i < nrv; i++) {
			get_corr[i * nrv + i] = 1;
		}
	}

	// === group index (if exists)
	if (UQjson["UQ_Method"].find("sensitivityGroups") != UQjson["UQ_Method"].end()) {
		std::string groupTxt = UQjson["UQ_Method"]["sensitivityGroups"];
		std::regex re(R"(\{([^}]+)\})");
		std::sregex_token_iterator it(groupTxt.begin(), groupTxt.end(), re, 1);
		std::sregex_token_iterator end;

		while (it != end) {
			std::stringstream ss(*it++);
			std::vector<double> aGroup; // use double for GSA from matlab
			while (ss.good()) {
				std::string substr;
				getline(ss, substr, ',');

				std::vector<std::string>::iterator itr = std::find(get_rvnames.begin(), get_rvnames.end(), substr);
				if (itr != get_rvnames.cend()) {
					aGroup.push_back(std::distance(get_rvnames.begin(), itr) + 1);
				}
				else {
					std::cout << "Element not found";
				}
			}
			get_groups.push_back(aGroup);
		}
	}
	else {
		for (int i = 0; i < nrv; i++) {
			//double num = i + 1;
			get_groups.push_back({ i + 1.0 });
		}
	}

}


void Getpnames(std::string distname, std::string optname, std::vector<std::string>& par_char)
{
	std::transform(distname.begin(), distname.end(), distname.begin(), ::tolower);

	if (optname.compare("PAR") == 0) { // Get parameters

		if (distname.compare("binomial") == 0) {  // Not used
			par_char.push_back("n");
			par_char.push_back("p");
		}
		else if (distname.compare("geometric") == 0) {  // Not used
			par_char.push_back("p");
		}
		else if (distname.compare("negativebinomial") == 0) {  // Not used
			par_char.push_back("k");
			par_char.push_back("p");
		}
		else if (distname.compare("poisson") == 0) {
			par_char.push_back("lambda");
			par_char.push_back("t");
		}
		else if (distname.compare("uniform") == 0) {
			par_char.push_back("lowerbound");
			par_char.push_back("upperbound");
		}
		else if (distname.compare("normal") == 0) {
			par_char.push_back("mean");
			par_char.push_back("stdDev");
		}
		else if (distname.compare("lognormal") == 0) {
			par_char.push_back("lambda");
			par_char.push_back("zeta");
		}
		else if (distname.compare("exponential") == 0) {
			par_char.push_back("lambda");
		}
		else if (distname.compare("gamma") == 0) {
			par_char.push_back("lambda");
			par_char.push_back("k");
		}
		else if (distname.compare("beta") == 0) {
			par_char.push_back("alphas");
			par_char.push_back("betas");
			par_char.push_back("upperBound");
			par_char.push_back("upperBound");
		}
		else if (distname.compare("gumbelMin") == 0) {  // Not used
			par_char.push_back("an");
			par_char.push_back("bn");
		}
		else if (distname.compare("gumbel") == 0) {
			par_char.push_back("alphaparam");
			par_char.push_back("betaparam");
		}
		else if (distname.compare("frechet") == 0) {  // Not used
			par_char.push_back("an");
			par_char.push_back("k");
		}
		else if (distname.compare("weibull") == 0) {
			par_char.push_back("an");
			par_char.push_back("k");
		}
		else if (distname.compare("GEV") == 0) {  
			par_char.push_back("beta");
			par_char.push_back("alpha");
			par_char.push_back("epsilon");
		}
		else if (distname.compare("GEVMin") == 0) {  // Not used
			par_char.push_back("beta");
			par_char.push_back("alpha");
			par_char.push_back("epsilon");
		}
		else if (distname.compare("pareto") == 0) {  // Not used
			par_char.push_back("x_m");
			par_char.push_back("alpha");
		}
		else if (distname.compare("rayleigh") == 0) {  // Not used
			par_char.push_back("alpha");
		}
		else if (distname.compare("Chisquared") == 0) {
			par_char.push_back("k");
		}
		else if (distname.compare("discrete") == 0) {
			par_char.push_back("Values");
			par_char.push_back("Weights");
		}
		else if (distname.compare("truncatedexponential") == 0) {
			par_char.push_back("lambda");
			par_char.push_back("a");
			par_char.push_back("b");
		}
		else {
			// NA
		}
	} 
	else if (optname.compare("MOM") == 0) { // Get Moments
		if (distname.compare("geometric") == 0) {  // Not used
			par_char.push_back("mean");
		}
		else if (distname.compare("poisson") == 0) {
			par_char.push_back("mean");
		}
		else if (distname.compare("exponential") == 0) {
			par_char.push_back("mean");
		}
		else if (distname.compare("beta") == 0) {
			par_char.push_back("mean");
			par_char.push_back("standardDev");
			par_char.push_back("lowerBound");
			par_char.push_back("upperBound");
		}
		else if (distname.compare("GEV") == 0) {
			par_char.push_back("mean");
			par_char.push_back("standardDev");
			par_char.push_back("epsilon");
		}
		else if (distname.compare("GEVMin") == 0) {  // Not used
			par_char.push_back("mean");
			par_char.push_back("standardDev");
			par_char.push_back("epsilon");
		}
		else if (distname.compare("rayleigh") == 0) {  // Not used
			par_char.push_back("mean");
		}
		else if (distname.compare("chisquared") == 0) {
			par_char.push_back("mean");
		}
		else if (distname.compare("truncatedexponential") == 0) {
			par_char.push_back("mean");
			par_char.push_back("a");
			par_char.push_back("b");
		}
		else
		{
			par_char.push_back("mean");
			par_char.push_back("standardDev");
		}
	}
	else if (optname.compare("DAT") == 0) { // Get DATA	
		if (distname.compare("binomial") == 0) {
			par_char.push_back("n");
		}
		else if (distname.compare("beta") == 0) {
			par_char.push_back("lowerBound");
			par_char.push_back("upperBound");
		}
		else if (distname.compare("truncatedexponential") == 0) {
			par_char.push_back("a");
			par_char.push_back("b");
		}
	}
	else {
		//NA
	}
}