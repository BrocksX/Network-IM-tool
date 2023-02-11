#include "TcpServer.h"
#include <iostream>
#include "Connection.h"
#include "Socket.h"
#include "Timestamp.h"
#include <string>
#include "json.hpp"
#include "RedisConnectPool.h"

using json = nlohmann::json_abi_v3_11_2::json;
int main()
{
    TcpServer *server = new TcpServer("0.0.0.0", 8888);
    RedisConnectPool *redisConns = new RedisConnectPool("127.0.0.1", 6379, 20);
    Timestamp *clock = new Timestamp();
    redisConns->connect();

    std::unordered_map<int, Connection *> client;
    std::unordered_map<int, std::string> fd_name;
    std::unordered_map<std::string, Connection *> name_conn;

    server->setMessageCallback([&redisConns, &clock, &client, &fd_name, &name_conn](Connection *conn)
                              {
    conn->read();
    if (conn->getState() == Connection::State::Closed) //关闭连接
    {
        if(client.find(conn->getSocket()->getFd())!=client.end())
            client.erase(conn->getSocket()->getFd());
        if(fd_name.find(conn->getSocket()->getFd())!=fd_name.end())
        {
            if(name_conn.find(fd_name[conn->getSocket()->getFd()]) != name_conn.end())
                name_conn.erase(fd_name[conn->getSocket()->getFd()]);
            fd_name.erase(conn->getSocket()->getFd());
        }
        if(!fd_name.empty())
        {
            json msg;
            msg["label"] = "member";
            std::string people_list;
            for(const auto & i : fd_name)
            {
                people_list += i.second;
                people_list += ' ';
            }
            msg["content"] = people_list;
            for(const auto & i : client)
            {
                i.second->send(msg.dump());
            }
            // std::cout << "SEND: " << msg.dump() << std::endl;
        }
        
        conn->close();
        return;
    }

    json receiveMsg = json::parse(conn->readBuffer());
    // std::cout << "RECEIVE: " << receiveMsg.dump() << std::endl;
    if(receiveMsg.contains("label"))
    {
        if(receiveMsg["label"]=="message")//接受信息
        {
            std::string msg = receiveMsg["time"];
            msg += " - [";
            msg += receiveMsg["sender"];
            msg += "] to [";
            msg += receiveMsg["receiver"];
            msg += "]: ";
            msg += receiveMsg["content"];
            // std::cout << msg << std::endl;

            if(receiveMsg["receiver"] == "public")
            {
                for(auto & cli : client)
                {
                    if(cli.second != conn)
                        cli.second->send(receiveMsg.dump());
                }
            }
            else
            {
                if(name_conn.find(receiveMsg["receiver"]) != name_conn.end())
                    name_conn[receiveMsg["receiver"]]->send(receiveMsg.dump());
            }

            std::string score = std::to_string(clock->now().getMicroSecondsSinceEpoch());
            std::string member = receiveMsg.dump();
            Redis* red = redisConns->getConnect();
            red->zadd("message", score, member);
            redisConns->releaseConnect(red);
        }
        else if(receiveMsg["label"]=="sign in")//新用户登陆
        {
            json msg;
            msg["label"] = "sign in";
            msg["name"] = receiveMsg["id"];
            if(!receiveMsg.contains("id")||!receiveMsg.contains("password"))
                return;
            std::string check;
            bool status = true;
            Redis* red = redisConns->getConnect();
            status = red->hget("account", receiveMsg["id"], check);
            redisConns->releaseConnect(red);

            if(check == receiveMsg["password"])
            {
                if(name_conn.find(receiveMsg["id"])==name_conn.end())
                    msg["content"] = "success";
                else
                {
                    msg["content"] = "repeat sign in";
                    status = false;
                }
            }
                
            else
            {
                msg["content"] = "wrong id or password";
                status = false;
            }
            conn->send(msg.dump());
            // std::cout << "SEND: " << msg.dump() << std::endl;
            
            if(!status) return;

            client[conn->getSocket()->getFd()] = conn;
            fd_name[conn->getSocket()->getFd()] = receiveMsg["id"];
            name_conn[receiveMsg["id"]] = conn;

            
            std::vector<std::string> values;
            red = redisConns->getConnect();
            bool foundHistroy = red->zrange("message", 0, 1000000, values, false);
            redisConns->releaseConnect(red);
            if(foundHistroy)
            {
                for(int i = 0; i < values.size(); i++)
                {
                    json history = json::parse(values[i]);
                    if(history.contains("receiver"))
                    {
                        if(history["receiver"] == "public" || history["receiver"] == receiveMsg["id"] || history["sender"] == receiveMsg["id"])
                        {
                            // std::cout << "SEND: " << values[i] << std::endl;
                            conn->send(values[i]);
                        }
                    } 
                }
            }
            
            msg.clear();
            msg["label"] = "member";
            std::string people_list;
            for(const auto & i : fd_name)
            {
                people_list += i.second;
                people_list += ' ';
            }
            msg["content"] = people_list;
            for(const auto & i : client)
            {
                i.second->send(msg.dump());
            }
            // std::cout << "SEND: " << msg.dump() << std::endl;

        }
        else if(receiveMsg["label"]=="sign up")//新用户注册
        {
            json msg;
            msg["label"] = "sign up";
            msg["name"] = receiveMsg["id"];
            if(!receiveMsg.contains("id")||!receiveMsg.contains("password"))
                return;
            int64_t status = 0;
            std::unordered_map<std::string, std::string> id_pwd;
            id_pwd[receiveMsg["id"]]=receiveMsg["password"];
            Redis* red = redisConns->getConnect();
            status = red->hset("account", id_pwd);
            redisConns->releaseConnect(red);

            if(!status)
                msg["content"] = "repeat id";
            else
                msg["content"] = "success";

            conn->send(msg.dump());
            // std::cout << "SEND: " << msg.dump() << std::endl;
            msg.clear();
        }
        else if(receiveMsg["label"]=="file")//文件传输
        {
            if(name_conn.find(receiveMsg["receiver"]) != name_conn.end())
                name_conn[receiveMsg["receiver"]]->send(receiveMsg.dump());
        }
    } });

    server->start();
    delete server;
    return 0;
}