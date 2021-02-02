#include <iostream>
#include <sstream>
#include "curl/curl.h"
#include "curl/easy.h"

using namespace std;
size_t writefunc(void* ptr, size_t size, size_t nmemb, std::string* s) {
    s->append(static_cast<char*>(ptr), size * nmemb);
    return size * nmemb;
}

int main(void) {
    CURL* curl = curl_easy_init();
    if (curl)
    {
        std::string s;

        curl_easy_setopt(curl, CURLOPT_URL, "https://api.spacexdata.com/v4/capsules");
        curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, writefunc);
        curl_easy_setopt(curl, CURLOPT_WRITEDATA, &s);

        CURLcode res = curl_easy_perform(curl);

        std::cout << s << std::endl;

        /* always cleanup */
        curl_easy_cleanup(curl);
    }
    return 0;
}