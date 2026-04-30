/*
* CODE BY CCA8798, LAST EDIT IN 2026/4/30
 * MIT LICENSE
 */

#include <iostream>
#include <fstream>
#include <filesystem>
#include <vector>
#include <random>
#include "jsonxx/jsonxx.h"
#include "curl/include/curl/curl.h"

template<typename Ts=std::string>
void pause(const Ts& text=std::string("")) {
	if constexpr (std::is_convertible_v<Ts, std::string>) {
		std::cout << text << std::endl;
	} else {
		throw std::invalid_argument("Invalid argument type for pause function. Expected a string or a type convertible to string!");
	}
	std::cin.get();
	return;
}

std::string generateRandomString(const size_t len) {
	static const char charset[] = "abcdefghijklmnopqrstuvwxyz0123456789";
	static const size_t charsetSize = sizeof(charset) - 1;

	static std::random_device randomDevice;
	static std::mt19937 mt19937Generateor(randomDevice());
	static std::uniform_int_distribution<size_t> dist(0, charsetSize - 1);

	std::string result;
	result.reserve(len);
	for (size_t i = 0; i < len; ++i) {
		result.push_back(charset[dist(mt19937Generateor)]);
	}
	return result;
}

static size_t WriteCallback(void* contents, size_t size, size_t nmemb, std::string* output) {
	size_t totalSize = size * nmemb;
	output->append((char*)contents, totalSize);
	return totalSize;
}

std::string getJsonFromWebApi(const std::string& url) {
	CURL* curl = curl_easy_init();
	if (!curl) {
		std::cerr << "curl_easy_init failed" << std::endl;
		return "-1";
	}

	std::string responseBody;
	curl_easy_setopt(curl, CURLOPT_URL, url.c_str());
	curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, WriteCallback);
	curl_easy_setopt(curl, CURLOPT_WRITEDATA, &responseBody);
	curl_easy_setopt(curl, CURLOPT_TIMEOUT, 10L);
	curl_easy_setopt(curl, CURLOPT_CONNECTTIMEOUT, 5L);
	curl_easy_setopt(curl, CURLOPT_FOLLOWLOCATION, 1L);
	curl_easy_setopt(curl, CURLOPT_USERAGENT, "SimpleIDLottery/1.0");

	CURLcode res = curl_easy_perform(curl);
	if (res != CURLE_OK) {
		std::cerr << "curl_easy_perform failed: " << curl_easy_strerror(res) << std::endl;
		curl_easy_cleanup(curl);
		return "-1";
	}

	long http_code = 0;
	curl_easy_getinfo(curl, CURLINFO_RESPONSE_CODE, &http_code);
	if (http_code != 200) {
		std::cerr << "HTTP status code: " << http_code << std::endl;
		curl_easy_cleanup(curl);
		return "-1";
	}

	curl_easy_cleanup(curl);
	return responseBody;
}

enum class inputMode {
	Local=1,
	WebApi
};

int main() {
	std::string currentFile("init");
	size_t inputMode=static_cast<size_t>(inputMode::Local);

	while (true) {
		std::cout<<"Welcome to Simple ID Lottery, please choose input mode:"<<std::endl
		<<"1. Local File Input"<<std::endl
		<<"2. Web API Input"<<std::endl;
		std::cin>>inputMode;
		switch (inputMode){
			case 1: {
				std::cout<<"Please input the required file name in the new line, directly press enter to exit the program"<<std::endl;
				std::cin>>currentFile;
				if (currentFile.empty()) exit(0);
				break;
			}
			case 2: {
				std::string url, fileName;
				std::cout<<"Please input the Web API URL: ";
				std::cin>>url;
				std::cout<<"Please input the required file name: ";
				std::cin>>fileName;
				std::ofstream outFile(fileName);
				outFile << getJsonFromWebApi(url);
				outFile.close();
				currentFile = fileName;
				break;
			}
			default: {
				std::cout<<"Invalid input mode! Please input again!"<<std::endl;
				continue;
			}
		}

		std::ifstream textData(currentFile);
		if (!textData) {
			std::cerr << "Error when opening the data file!";
			throw std::runtime_error("Error when opening the data file!");
		}

		std::cout<<"Successfully read the data file! Parsing json text from the data file"<<std::endl;

		jsonxx::Array jsonData;
		std::stringstream stringStreamTemp;
		stringStreamTemp<<textData.rdbuf();
		if (!jsonData.parse(stringStreamTemp.str())) {
			std::cerr << "Error parsing the data file!";
			throw std::runtime_error("Error parsing the data file!");
		}

		std::vector<std::string> rollList={};
		{
			std::cout<<"==============Reads of files=============="<<std::endl;
			std::cout<<"The users was read from the file:"<<std::endl;
			for (size_t i = 0; i < jsonData.size(); ++i) {
				std::string name = jsonData.get<std::string>(i);
				rollList.emplace_back(name);
				std::cout << name << std::endl;
			}
			std::cout<<std::endl;
		}

		/* CONFIG OF ROUNDS */
		/* TODO: REMOVE THE GOTO FLAG IN THE FUTURE */
		reinput:
		size_t roundsCount=0;
		size_t totalWinnersCount=0;
		std::vector<size_t> resultsCountPerRound(roundsCount,0);
		std::cout<<"==============Config of Rounds=============="<<std::endl;
		{
			std::cout<<"Please input the number of rounds: ";
			std::cin>>roundsCount;
			resultsCountPerRound.resize(roundsCount,0);
			std::cout<<"Please input the number of winners per round in the next"<<std::endl;
			for (size_t i = 0; i < roundsCount; ++i) {
				std::cout<<"Winner number of Round "<<i+1<<": ";
				std::cin>>resultsCountPerRound.at(i);
				totalWinnersCount+=resultsCountPerRound.at(i);
				if (totalWinnersCount > rollList.size()) {
					std::cout<<"Sorry! The number of winners per round is larger than the number of users! Please Input Again!"<<std::endl;
					goto reinput;
				}
			}
		}

		bool deleteUsersAfterRoll=false;
		{
			std::cout<<"Would you like to delete the winners from the list after the round? (Y/n): ";
			char inputChar;
			std::cin>>inputChar;
			if (inputChar=='Y') {
				deleteUsersAfterRoll = true;
			}
			std::cout<< std::endl;
		}

		bool rollsStepByStep=true;
		{
			std::cout<<"Would you like to roll step by step?(If no, the program will output all winners in once) (Y/n): ";
			char inputChar;
			std::cin>>inputChar;
			if (inputChar=='n') {
				rollsStepByStep = false;
			}
			std::cout<< std::endl;
		}

		pause("Press Any Key to start rolling from the showed list");

		std::string randomString(generateRandomString(15));
		{
			namespace fs = std::filesystem;
			if (!fs::exists("results")){
				fs::create_directory("results");
			}
			if (!fs::exists("results/"+randomString)) {
				fs::create_directory("results/"+randomString);
			}
		}

		{
			namespace fs=std::filesystem;
			for (size_t r = 0; r < roundsCount; r++) {
				for (size_t i = 0; i < resultsCountPerRound.at(r); i++) {
					fs::path currentStageFolder="results/"+randomString+"/stage_"+std::to_string(r+1);
					if (!fs::exists(currentStageFolder)) {
						fs::create_directory(currentStageFolder);
					}
					jsonxx::Array currentRollJson;
					for (auto& name: rollList) {
						currentRollJson.append(name);
					}
					std::ofstream currentRollFile(currentStageFolder.string()+"/roll_"+std::to_string(i+1)+".json");
					currentRollFile<<currentRollJson.json();
					std::random_device randomDevice;
					std::mt19937_64 mt19937Range(randomDevice());
					std::uniform_int_distribution<size_t> dist(0, rollList.size() - 1);
					size_t winner=dist(mt19937Range);
					std::cout << "Winner "<<i+1<<" of Round "<<r+1<<" is "<< rollList.at(winner) << std::endl;
					if (deleteUsersAfterRoll) {
						rollList.erase(rollList.begin()+winner,rollList.begin()+winner+1);
					}
					if (rollsStepByStep) pause();
				}
			}
		}

		pause("Rolls Over! The id of the rolling round is"+randomString+", Press Any Key to start another round");
	}
	curl_global_cleanup();
	return 0;
}