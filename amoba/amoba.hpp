#include <internal_use_only/config.hpp>
#include <iostream>
#include <websocketpp/config/asio_no_tls.hpp>
#include <websocketpp/server.hpp>
#include <nlohmann/json.hpp>
#include <amoba/amoba.h>
#include <amoba/amobas/inf_am.h>
#include <set>
#include <queue>

#include <string>
#include <cstdint>
#include <vector>

constexpr uint8_t g_Dimension = 2;
constexpr uint64_t g_Win = 3, g_ChunkSize = 32;

typedef websocketpp::server<websocketpp::config::asio> server;

namespace ws {
    enum PacketType {
        Login, Place
    };
    class Packet {
    public:
        PacketType type;
        virtual void from_json(nlohmann::json j) = 0;
        virtual void to_json(nlohmann::json& j) = 0;
        Packet(nlohmann::json j) { from_json(j); }
        Packet() = default;
    };

    class LoginPacket : Packet {
    public:
        std::string name;
    public:
        void from_json(nlohmann::json j) {
            j.at("name").get_to(this->name);
        }
        void to_json(nlohmann::json& j) {
            j = nlohmann::json{ {"name" , name} };
        }

    };
    class PlacePacket : Packet {
    public:
        std::vector<int64_t> position;
        int64_t color;
    public:
        PlacePacket() {};
        PlacePacket(std::vector<int64_t> p, int64_t c) : position(p), color(c) {}
        void from_json(nlohmann::json j) {
            j.at("position").get_to(this->position);
            j.at("color").get_to(this->color);
        };
        void to_json(nlohmann::json& j) {
            j = nlohmann::json{ {"position" , position}, {"color" , color} };
        }

        friend std::ostream& operator<<(std::ostream& os, PlacePacket wp) {
            os << "Color: " << wp.color << " ; Position : [";
            for (auto p : wp.position) os << p << ", ";
            os << "]";
            return os;
        }
    };
}

enum action_type {
    SUBSCRIBE,
    UNSUBSCRIBE,
    MESSAGE
};

struct am_action {
    am_action(action_type t, websocketpp::connection_hdl h) : type(t), hdl(h) {}
    am_action(action_type t, websocketpp::connection_hdl h, server::message_ptr m)
      : type(t), hdl(h), msg(m) {}

    action_type type;
    websocketpp::connection_hdl hdl;
    server::message_ptr msg;
};

class Player {
public:
    std::string name;
    int id;
    Player(std::string n, int i) : name(n), id(i) {}
};

class am_server {
  public:
    am_server();
    ~am_server();
    void run(uint16_t port);
    static void on_message(am_server* server, websocketpp::connection_hdl hdl, server::message_ptr msg);
    static void on_open(am_server* server, websocketpp::connection_hdl hdl);
    static void on_close(am_server* server, websocketpp::connection_hdl hdl);
    void process_messages();
  public:
    server m_Endpoint;
    am_amoba amoba;
  public:
    typedef std::set<websocketpp::connection_hdl,std::owner_less<websocketpp::connection_hdl> > con_list;

    con_list m_Connections;

    std::mutex m_ActionLock;
    std::mutex m_ConnectionLock;
    websocketpp::lib::condition_variable m_ActionCondition;

    std::vector<std::shared_ptr<Player>> m_Players;
    std::unordered_map<websocketpp::connection_hdl, std::shared_ptr<Player>> m_PlayerMap;
    std::queue<am_action> m_Actions;
};
