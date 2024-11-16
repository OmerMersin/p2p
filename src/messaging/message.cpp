//
// Created by Omer Mersin on 11/16/24.
//
#include "messaging/message.h"
#include <sqlite3.h>
#include <iostream>

Message::Message(const std::string &sender, const std::string &recipient, const std::string &content)
        : sender(sender), recipient(recipient), content(content) {}

void Message::saveToDatabase() {
    sqlite3 *db;
    sqlite3_open("messages.db", &db);
    std::string sql = "INSERT INTO messages (sender, recipient, content) VALUES ('" + sender + "', '" + recipient + "', '" + content + "');";
    char *errMsg;
    if (sqlite3_exec(db, sql.c_str(), 0, 0, &errMsg) != SQLITE_OK) {
        std::cerr << "Error saving message: " << errMsg << std::endl;
        sqlite3_free(errMsg);
    }
    sqlite3_close(db);
}