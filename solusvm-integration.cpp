#include <string>
#include <sstream>
#include <curl/curl.h>
#include <nlohmann/json.hpp>

using json = nlohmann::json;

class SolusVM {
public:
    SolusVM(std::string api_url, std::string api_key) :
        api_url(api_url), api_key(api_key), api_hash(md5(api_key + std::to_string(time(nullptr))))
    {}

    std::string createServer(std::string plan, std::string hostname, std::string password, std::string username, std::string os, std::string template) {
        std::string api_action = "vserver-create";
        std::stringstream api_params;
        api_params << "plan=" << plan << "&hostname=" << hostname << "&password=" << password << "&username=" << username << "&os=" << os << "&template=" << template;

        json response = makeApiRequest(api_action, api_params.str());

        if (response["status"] == "success") {
            return response["vserverid"].get<std::string>();
        } else {
            throw std::runtime_error(response["statusmsg"].get<std::string>());
        }
    }

    json getServerInfo(std::string vserverid) {
        std::string api_action = "vserver-info";
        std::stringstream api_params;
        api_params << "vserverid=" << vserverid;

        json response = makeApiRequest(api_action, api_params.str());

        if (response["status"] == "success") {
            return response;
        } else {
            throw std::runtime_error(response["statusmsg"].get<std::string>());
        }
    }

    bool rebootServer(std::string vserverid) {
        std::string api_action = "vserver-reboot";
        std::stringstream api_params;
        api_params << "vserverid=" << vserverid;

        json response = makeApiRequest(api_action, api_params.str());

        if (response["status"] == "success") {
            return true;
        } else {
            throw std::runtime_error(response["statusmsg"].get<std::string>());
        }
    }

    bool suspendServer(std::string vserverid) {
        std::string api_action = "vserver-suspend";
        std::stringstream api_params;
        api_params << "vserverid=" << vserverid;

        json response = makeApiRequest(api_action, api_params.str());

        if (response["status"] == "success") {
            return true;
        } else {
            throw std::runtime_error(response["statusmsg"].get<std::string>());
        }
    }

    bool unsuspendServer(std::string vserverid) {
        std::string api_action = "vserver-unsuspend";
        std::stringstream api_params;
        api_params << "vserverid=" << vserverid;

        json response = makeApiRequest(api_action, api_params.str());

        if (response["status"] == "success") {
            return true;
        } else {
            throw std::runtime_error(response["statusmsg"].get<std::string>());
        }
    }

private:
    std::string api_url;
    std::string api_key;
    std::string api_hash;

    json makeApiRequest(std::string api_action, std::string api_params) {
        CURL *curl;
        CURLcode res;
        std::string request_url = api_url + "?key=" + api_key + "&hash=" + api_hash + "&action=" + api_action + "&" + api_params;
        std::string response;

        curl = curl_easy_init();

        if (curl) {
            curl_easy_setopt(curl, CURLOPT_URL, request_url.c_str());
            curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, writeCallback);
            curl_easy_setopt(curl, CURLOPT_WRITEDATA, &response);
            res = curl_easy_perform(curl);

            if (res != CURLE_OK) {
                throw std::runtime_error(curl_easy_strerror(res));
            }

            curl_easy_cleanup(curl);

            json result = json::parse(response);

            return result;
        } else {
            throw std::runtime_error("Failed to initialize CURL");
        }
    }
};

static size_t writeCallback(char* ptr, size_t size, size_t nmemb, std::string* data) {
    data->append(ptr, size * nmemb);
    return size * nmemb;
}

static std::string md5(std::string input) {
    // Implementation of MD5 hash function
}
