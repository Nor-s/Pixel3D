#include "redis_db.h"

RedisDB::RedisDB()
{
    worker_ = std::thread(Exec, python_path_ + " " + script_path_ + " --models_output_path " + models_dir_path_);
    conn_ = redisConnect("127.0.0.1", 6379);
    if ((NULL != conn_) && conn_->err)
    {
        std::cout << "Can't connect to Redis server" << std::endl;
        redisFree(conn_);
    }
    else if (NULL == conn_)
    {
        std::cout << "Can't connect to Redis server" << std::endl;
    }
    /* PING server */
    resp_ = (redisReply *)redisCommand(conn_, "PING");
    printf("PING: %s\n", resp_->str);
    freeReplyObject(resp_);
    resp_ = (redisReply *)redisCommand(conn_, "SET Pixel3D %s", "start");
    freeReplyObject(resp_);

    std::cout << "redis connected" << std::endl;
}
RedisDB::~RedisDB()
{
    std::cout << "~HeightmapLayer" << std::endl;
    resp_ = (redisReply *)redisCommand(conn_, "LPUSH mq {\"end\":\"1\"}");
    freeReplyObject(resp_);
    redisFree(conn_);
    if (worker_.joinable())
    {
#ifndef NDEBUG
        std::cout << "~RedisDB : Detacth worker thread" << std::endl;
#endif
        worker_.detach();
    }
}
bool RedisDB::is_updated()
{
    resp_ = (redisReply *)redisCommand(conn_, "GET UPDATE");
    if (!check() || !resp_->str)
    {
        return false;
    }
    if (resp_->str[0] != '1')
    {
        freeReplyObject(resp_);
        return false;
    }
    resp_ = (redisReply *)redisCommand(conn_, "SET UPDATE 0");
    freeReplyObject(resp_);

    return true;
}

bool RedisDB::check()
{
    if (!resp_)
    {
        std::cout << "COMMAND ERROR: " << conn_->errstr << std::endl;
        return false;
    }
    else if (resp_->type == REDIS_REPLY_ERROR)
    {
        std::cout << "COMMAND ERROR: " << resp_->str << std::endl;
        freeReplyObject(resp_);
        return false;
    }
    return true;
}

void RedisDB::push(const std::string &msg, bool check_idle)
{
    std::cout << "push\n";
    if (check_idle)
    {
        resp_ = (redisReply *)redisCommand(conn_, "GET IDLE");
        if (!check())
        {
            std::cout << "! IDLE" << std::endl;
            return;
        }
        if (resp_->str[0] != '1')
        {
            std::cout << resp_->str[0] << "<------resp\n";
            freeReplyObject(resp_);
            return;
        }
    }
    std::cout << "msg: " << msg << "\n";
    resp_ = (redisReply *)redisCommand(conn_, "LPUSH %s %s", "mq", msg.c_str());
    if (check())
    {
        freeReplyObject(resp_);
    }
}

std::string &RedisDB::get_mutable_python_path()
{
    return python_path_;
}
std::string &RedisDB::get_mutable_script_path()
{
    return script_path_;
}
std::string &RedisDB::get_mutable_models_dir_path()
{
    return models_dir_path_;
}

void RedisDB::Exec(std::string cmd)
{
    std::cout << "======================================\n";
    std::cout << cmd << std::endl;
    system(cmd.c_str());
    std::cout << "===================================end\n";
}