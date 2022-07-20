#ifndef SCENE_REDIS_DB_H
#define SCENE_REDIS_DB_H

#include "hiredis/hiredis.h"
#include "hiredis/hiredis_ssl.h"

#include <iostream>
#include <filesystem>
#include <thread>
#include <string>

class RedisDB
{
public:
    RedisDB();
    ~RedisDB();
    bool check();
    void push(const std::string &msg, bool check_idle = true);
    std::string &get_mutable_python_path();
    std::string &get_mutable_script_path();
    std::string &get_mutable_models_dir_path();
    bool is_updated();
private:
    static void Exec(std::string cmd);
    std::string python_path_{"C:\\Users\\No\\anaconda3\\envs\\pytorch\\python.exe                            "};
    std::string script_path_{"D:\\Study\\Pixel3D\\message_queue.py                                            "};
    std::string models_dir_path_{"D:\\Study\\Pixel3D\\procedural_3d_terrain\\output                          "};

    redisContext *conn_ = NULL;
    redisReply *resp_ = NULL;
    std::thread worker_;
};

#endif