#pragma once
#include <optional>
#include <sqlite3.h>
#include <string>
#include <unordered_map>
#include <unordered_set>
#include <vector>

namespace db {
    struct User {

        User(std::string login, std::string name, std::string password_hash, std::string role, bool is_deleted, int64_t unixtime) :
            login(login), name(name), password_hash(password_hash), role(role), is_deleted(is_deleted), unixtime(unixtime) {}

        std::string login;
        std::string name;
        std::string password_hash;
        std::string role;
        bool is_deleted;
        int64_t unixtime; //ns
    };

    struct Message {
        
        Message(std::string message, int64_t unixtime, 
                std::string user_login, std::string room, int64_t id_message_in_room):
            message(message), 
            unixtime(unixtime), 
            user_login(user_login), 
            room(room), 
            id_message_in_room(id_message_in_room){}
        
        std::string message;
        int64_t unixtime; //ns
        std::string user_login;
        std::string room;
        int64_t id_message_in_room;
    };

    class DB {
    public:
        DB();
        explicit DB(const std::string& db_file);
        ~DB();

        // --- System ---
        bool OpenDB();
        void CloseDB();
        std::string GetVersionDB();

        // --- Users ---
        bool CreateUser(const User& user);
        // если числится хоть в одной комнате, удаления не будет, только пометка is_deleted = 1, т.н. мягкое  удаление:
        bool DeleteUser(const std::string& user_login);
        bool IsUser(const std::string& user_login);
        bool IsAliveUser(const std::string& user_login);
        bool ChangeUserName(const std::string& user_login, const std::string& new_name);
        std::optional<User> GetUserData(const std::string& user_login);
        std::vector<User> GetAllUsers();
        std::vector<User> GetActiveUsers();
        std::vector<User> GetDeletedUsers();
        std::vector<std::string> GetUserRooms(const std::string& user_login);
        std::unordered_map<std::string, std::unordered_set<std::string>> GetAllRoomWithRegisteredUsers();

        // --- Rooms ---
        bool CreateRoom(const std::string& room, int64_t unixtime);
        bool ChangeRoomName(const std::string& current_room_name, const std::string& new_room_name);
        // при удалении комнаты в БД происходит автоматическое удаление из TABLE messages сообщений удаляемой комнаты:
        bool DeleteRoom(const std::string& room);
        bool IsRoom(const std::string& room);
        bool AddUserToRoom(const std::string& user_login, const std::string& room);
        std::vector<User> GetRoomActiveUsers(const std::string& room);
        // без удаления сообщений пользователя в комнате, только из TABLE user_rooms:
        bool DeleteUserFromRoom(const std::string& user_login, const std::string& room);
        std::vector<std::string> GetRooms();

        // --- Messages ---
        bool InsertMessageToDB(const Message& message); 
        std::vector<Message> GetRangeMessagesRoom(const std::string& room, int64_t id_message_begin, int64_t id_message_end);
        int GetCountRoomMessages(const std::string& room);

    private:
        sqlite3* db_ = nullptr;
        std::string db_filename_ = "chat.db";

        bool InitSchema();
        bool SetUserForDelete(const std::string& user_login);
        bool PerformSQLReturnBool(const char* sql_query, std::vector<std::string> param);
        bool DelDeletedUsersWithoutRoom();
        std::vector<User> GetUsers(const char* sql);
    };
} // db

