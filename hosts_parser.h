#pragma once

#include <string>
#include <vector>

struct hosts_item_t {
    uint64_t ip_flag:1;
    uint64_t domain_flag:1;
    uint64_t comments_flag:1;
    std::string ip;
    std::string domain;
    std::string comments;
};

class HostsParser {
private:
    std::vector<hosts_item_t> items_;
public:
    HostsParser()
        : items_{} {
    }

    bool parse(char ch);
};