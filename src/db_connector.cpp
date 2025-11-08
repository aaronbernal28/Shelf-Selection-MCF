#include "db_connector.h"
#include <cstdlib>
#include <stdexcept>

namespace SS {

DBConnector::DBConnector() {
    load_env_config();
}

void DBConnector::load_env_config() {
    const char* env_dbname = std::getenv("DB_NAME");
    const char* env_user = std::getenv("DB_USER");
    const char* env_host = std::getenv("DB_HOST");
    const char* env_port = std::getenv("DB_PORT");
    const char* env_password = std::getenv("DB_PASSWORD");
    
    if (!env_dbname || !env_user || !env_host || !env_port) {
        throw std::runtime_error(
            "Database environment variables not set. "
            "Please set DB_NAME, DB_USER, DB_HOST, and DB_PORT"
        );
    }
    
    dbname_ = env_dbname;
    db_user_ = env_user;
    db_host_ = env_host;
    db_port_ = env_port;
    db_password_ = env_password ? env_password : "";
}

std::string DBConnector::get_connection_string() const {
    std::string conn_str = "dbname=" + dbname_ + 
                          " user=" + db_user_ + 
                          " host=" + db_host_ + 
                          " port=" + db_port_;
    
    if (!db_password_.empty()) {
        conn_str += " password=" + db_password_;
    }
    
    return conn_str;
}

pqxx::connection DBConnector::connect() const {
    return pqxx::connection(get_connection_string());
}

}
