#include <iostream>
#include <json.hpp>
#include <cstdlib>
#include <fstream>

using json = nlohmann::json;

int main()
{
	std::system("rm -r data.json");
	std::system("curl --request GET \
	--url 'https://api.spotify.com/v1/search?q=Wish%2520You%2520Were%2520Here%26artist%3DPink%2520Floyd&type=album' \
	--header 'Authorization: Bearer BQAPFK6lMmXOlpi2TU1shdbNwJGKEGNC3fjm6-LnVpIasyJUFK4cVPHJLKqJSDhkL1rZ23771EJSJWwmM5i2WURn24aymQgLcTcVWrBlEUYNVfcAWmY' > temp.json");
	std::system("fixjson temp.json > data.json");
	std::system("rm -r temp.json");

	json data = json::parse(std::ifstream("data.json"));

	int i = 0;
	auto& names =data["albums"]["items"];
	for(auto it = names.begin(); it != names.end(); ++it){
		std::cout << i << ": " << it->at("name") << "\n";
		i++;
	}
	

	return 0;
}
