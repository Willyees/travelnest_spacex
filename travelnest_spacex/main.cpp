#include <iostream>
#include <sstream>
#include <unordered_map>
#include "curl/curl.h"
#include "curl/easy.h"
#include "rapidjson/document.h"
#include "rapidjson/writer.h"
#include "rapidjson/stringbuffer.h"
#include "rapidjson/prettywriter.h"

using namespace std;
using namespace rapidjson;

size_t writefunc(void* ptr, size_t size, size_t nmemb, std::string* s) {
    s->append(static_cast<char*>(ptr), size * nmemb);
    return size * nmemb;
}

int main(void) {
    CURL* curl = curl_easy_init();
    string url_base = "https://api.spacexdata.com/v4/";
    //json headers
    struct curl_slist* headers = NULL;
    headers = curl_slist_append(headers, "Accept: application/json");
    headers = curl_slist_append(headers, "Content-Type: application/json");
    headers = curl_slist_append(headers, "charset: utf-8");

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
        string id = d["name"].GetString();
        string date = d["date_utc"].GetString();
        string details = d["details"].GetString();
        std::cout << "Latest spacex launch" << std::endl;
        cout << "id:\t\t" << id << endl;
        cout << "date:\t\t" << date << endl;
        cout << "details:\t" << details << endl;
        cout << "--------" << endl;

        Value& v_ships = d["ships"];//array 
        Document d_ships, d_options;
        auto& all = d_ships.GetAllocator();
        Value v_ships_copy(v_ships, all);//copying with the correct allocator
        d_ships.SetObject().AddMember("query", rapidjson::kObjectType, all);
        Value& query = d_ships["query"];
        query.AddMember("_id", v_ships_copy, all);
        //d_ships.AddMember("options", rapidjson::kObjectType, all); //uncomment if needed to pass options
        //stringify the ships array with the new json value name: id
        StringBuffer buff;
        PrettyWriter<StringBuffer> w(buff);
        d_ships.Accept(w);
        const char* json_query_ships = buff.GetString();

        string ships_info = "";
        curl_easy_setopt(curl, CURLOPT_URL, (url_base + "ships/query").c_str());
        curl_easy_setopt(curl, CURLOPT_POSTFIELDS, json_query_ships);//set post request body
        curl_easy_setopt(curl, CURLOPT_HTTPHEADER, headers);//needs headers into the request, otherwise is sent as raw format
        curl_easy_setopt(curl, CURLOPT_WRITEDATA, &ships_info);
        res = curl_easy_perform(curl);

        //parse the json returned data
        Document d_ship;
        d_ship.Parse(ships_info.c_str());
        Value& v_ships_info = d_ship["docs"];

        cout << "SHIPS in the launch" << endl;
        for (auto it = v_ships_info.Begin(); it != v_ships_info.End(); ++it) {
            cout << "name:\t\t" << (*it)["name"].GetString() << endl;
            cout << "type:\t\t" << (*it)["type"].GetString() << endl;
            cout << "home port:\t" << (*it)["home_port"].GetString() << endl;
            cout << endl;
        }
    
        /* always cleanup */
        curl_easy_cleanup(curl);
    }
    system("pause");
    return 0;
}