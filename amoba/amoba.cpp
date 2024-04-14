#include "amoba.hpp"
#include <iostream>
#include <functional>

am_server::am_server() {
  using websocketpp::lib::placeholders::_1;
  using websocketpp::lib::placeholders::_2;
  m_Endpoint.init_asio();
  m_Endpoint.set_message_handler(std::bind(&am_server::on_message, this, _1, _2));
  m_Endpoint.set_open_handler(std::bind(&am_server::on_open,this,_1));
  m_Endpoint.set_close_handler(std::bind(&am_server::on_close,this,_1));
  m_Endpoint.set_error_channels(websocketpp::log::elevel::all);
  m_Endpoint.set_access_channels(websocketpp::log::alevel::all ^ websocketpp::log::alevel::frame_payload);

  amoba = am_amoba_list[0];
  am_inf_am_parameters params = {g_Win, g_ChunkSize, g_Dimension, {0}};
  amoba.init_fun(&amoba, &params);
}
am_server::~am_server() {
  m_Endpoint.stop();
}
void am_server::run(uint16_t port) {
  m_Endpoint.listen(port);
  m_Endpoint.start_accept();
  m_Endpoint.run();
}
void am_server::on_message(am_server* server, websocketpp::connection_hdl hdl, server::message_ptr msg) {
  {
    std::lock_guard<std::mutex> guard(server->m_ActionLock);
    server->m_Actions.push(am_action(MESSAGE,hdl,msg));
  }
  server->m_ActionCondition.notify_one();
}
void am_server::on_open(am_server *server, websocketpp::connection_hdl hdl) {
  {
    std::lock_guard<std::mutex> guard(server->m_ActionLock);
    server->m_Actions.push(am_action(SUBSCRIBE,hdl));
  }
  server->m_ActionCondition.notify_one();
}
void am_server::on_close(am_server *server, websocketpp::connection_hdl hdl) {
  {
    std::lock_guard<std::mutex> guard(server->m_ActionLock);
    server->m_Actions.push(am_action(UNSUBSCRIBE,hdl));
  }
  server->m_ActionCondition.notify_one();
}

void am_server::process_messages() {
  bool won = false;
  for (;!won;) {
    std::unique_lock<std::mutex> lock(m_ActionLock);
    while (m_Actions.empty()) {
      m_ActionCondition.wait(lock);
    }
    am_action a = m_Actions.front();
    m_Actions.pop();
    lock.unlock();
    switch (a.type) {
      case SUBSCRIBE: {
        std::lock_guard<std::mutex> guard(m_ConnectionLock);
        m_Connections.insert(a.hdl);
                      }
        break;
      case UNSUBSCRIBE: {
        std::lock_guard<std::mutex> guard(m_ConnectionLock);
        m_Connections.erase(a.hdl);
                        }
        break;
      case MESSAGE: {
          std::lock_guard<std::mutex> guard(m_ConnectionLock);
          auto msg = a.msg;
          nlohmann::json j = nlohmann::json::parse(msg->get_payload());
          if (j["type"] == "place") {
              ws::PlacePacket data; data.from_json(j);
              std::cout << data << std::endl;

              std::vector<int64_t> position = data.position;
              color_t color = m_PlayerMap[a.hdl]->id;

              am_inf_am_position ampos;
              ampos.dimension = g_Dimension;
              for (uint8_t i = 0; i < g_Dimension; i++) {
                  ampos.component[i] = position[i];
              }

              amoba.set_fun(&amoba, color, &ampos);

              color_t win = amoba.check_fun(&amoba, &ampos);
              if (win) {
                  std::cout << m_PlayerMap[a.hdl]->name << " has won! Goodbye!" << std::endl;
                  won = true;
                  ws::PlacePacket p({}, win);
                  nlohmann::json j; p.to_json(j);
                  std::string s = j.dump();
                  std::cout << s << std::endl;
                  for (auto connection : m_Connections) {
                      m_Endpoint.send(connection, s, websocketpp::frame::opcode::text);
                  }
              }
              else {
                  ws::PlacePacket p(position, color);
                  nlohmann::json j; p.to_json(j);
                  std::string s = j.dump();
                  std::cout << s << std::endl;
                  for (auto connection : m_Connections) {
                      m_Endpoint.send(connection, s, websocketpp::frame::opcode::text);
                  }
              }
          }
          else if (j["type"] == "login") {
              ws::LoginPacket p; p.from_json(j);
              int id = m_Players.size() + 1;
              std::shared_ptr<Player> j = std::make_shared<Player>(p.name, id);
              m_Players.push_back(j);
              m_PlayerMap[a.hdl] = j;
          }
      }
      break;
    }
  }
  std::cout << "Goodbye!" << std::endl;
}
int main() {
  am_server serv;
  std::thread t(std::bind(&am_server::process_messages, &serv));
  serv.run(9002);
}
