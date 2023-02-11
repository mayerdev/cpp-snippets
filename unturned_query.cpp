#include <iostream>
#include <string>
#include <cstring>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <unistd.h>

using namespace std;

struct UnturnedServerInfo {
    bool online;
    string name;
    int players;
    int max_players;
};

UnturnedServerInfo get_untuned_server_info(const char* ip_address, int port) {
    UnturnedServerInfo server_info;

    // create a UDP socket
    int sock = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
    if (sock == -1) {
        cerr << "Error creating socket." << endl;
        server_info.online = false;
        return server_info;
    }

    // set socket timeout
    struct timeval timeout;
    timeout.tv_sec = 2;
    timeout.tv_usec = 0;
    if (setsockopt(sock, SOL_SOCKET, SO_RCVTIMEO, &timeout, sizeof(timeout)) == -1) {
        cerr << "Error setting socket timeout." << endl;
        close(sock);
        server_info.online = false;
        return server_info;
    }

    // build the query packet
    uint8_t packet[] = {
        0xFF, 0xFF, 0xFF, 0xFF, 'U', 'S', 'Q', 'e', 'r', 'y', 0x00
    };

    // send the query packet to the server
    struct sockaddr_in server_addr;
    memset(&server_addr, 0, sizeof(server_addr));
    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = inet_addr(ip_address);
    server_addr.sin_port = htons(port);

    if (sendto(sock, packet, sizeof(packet), 0, (struct sockaddr*)&server_addr, sizeof(server_addr)) == -1) {
        cerr << "Error sending query packet." << endl;
        close(sock);
        server_info.online = false;
        return server_info;
    }

    // receive the response packet from the server
    uint8_t response_packet[4096];
    memset(response_packet, 0, sizeof(response_packet));

    struct sockaddr_in response_addr;
    socklen_t response_addr_len = sizeof(response_addr);

    ssize_t num_bytes_received = recvfrom(sock, response_packet, sizeof(response_packet), 0, (struct sockaddr*)&response_addr, &response_addr_len);

    if (num_bytes_received == -1) {
        cerr << "Error receiving response packet." << endl;
        close(sock);
        server_info.online = false;
        return server_info;
    }

    close(sock);

    // parse the response packet
    server_info.online = true;

    // skip the first 10 bytes (magic bytes and packet type)
    const uint8_t* response_data = response_packet + 10;

    // read the server name (null-terminated string)
    server_info.name = (const char*)response_data;
    response_data += server_info.name.length() + 1;

    // read the map name (null-terminated string)
    string map_name = (const char*)response_data;
    response_data += map_name.length() + 1;

    // read the game mode (null-terminated string)
    string game_mode = (const char*)response_data;
    response_data += game_mode.length() + 1;

    // read the current number of players
    server_info.players = (int)*response_data++;
    server_info.max_players = (int)*response_data++;

    return server_info;
}
