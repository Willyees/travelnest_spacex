#include <iostream>
#include <sstream>
#include <unordered_map>
#include "curl/curl.h"
#include "curl/easy.h"
#include "rapidjson/document.h"
#include "rapidjson/writer.h"
#include "rapidjson/stringbuffer.h"
using namespace std;
using namespace rapidjson;

size_t writefunc(void* ptr, size_t size, size_t nmemb, std::string* s) {
    s->append(static_cast<char*>(ptr), size * nmemb);
    return size * nmemb;
}

/*unordered_map<const char*, const char*> launch_tags
{ {"id", "id"}, {"name", "name"} };*/
int main(void) {
    CURL* curl = curl_easy_init();
    string url_base = "https://api.spacexdata.com/v4/";

    
    if (curl)
    {
        std::string latest_launch;
        curl_easy_setopt(curl, CURLOPT_URL, (url_base + "launches/latest").c_str());//expecting 0-terminated C string type
        curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, writefunc);
        curl_easy_setopt(curl, CURLOPT_WRITEDATA, &latest_launch);
        //get the latest launch info
        CURLcode res = curl_easy_perform(curl);
        
        Document d;
        d.Parse(latest_launch.c_str());
        string id = d["name"].GetString();//not checking if it is array because of schema
        string date = d["date_utc"].GetString();
        string details = d["details"].GetString();
        std::cout << "Latest spacex launch" << std::endl;
        cout << "id:\t\t" << id << endl;
        cout << "date:\t\t" << date << endl;
        cout << "details:\t" << details << endl;
        cout << "--------" << endl;

        Value& v_ships = d["ships"];//array 

        if (v_ships.IsArray()) {
            cout << "SHIPS in the launch" << endl;
            for (auto it = v_ships.Begin(); it != v_ships.End(); ++it) {
                string ship_info = "";
                //cout << it->GetString() << endl;
                curl_easy_setopt(curl, CURLOPT_URL, (url_base + "ships/" + it->GetString()).c_str());
                curl_easy_setopt(curl, CURLOPT_WRITEDATA, &ship_info);

                res = curl_easy_perform(curl);
                Document d_ship;
                d_ship.Parse(ship_info.c_str());
                cout << "name:\t\t" << d_ship["name"].GetString() << endl;
                cout << "type:\t\t" << d_ship["type"].GetString() << endl;
                cout << "home port:\t" << d_ship["home_port"].GetString() << endl;
                cout << endl;
            }
        }

        /* always cleanup */
        curl_easy_cleanup(curl);
    }
    system("pause");
    return 0;
}