#ifndef DB_CONNECTOR_H
#define DB_CONNECTOR_H

#include <string>
#include <pqxx/pqxx>

namespace SS {

/**
 * @brief Database connector for PostgreSQL
 * Reads configuration from environment variables
 */
class DBConnector {
public:
    // Constructor - loads config from environment variables
    DBConnector();
    
    // Get connection string
    std::string get_connection_string() const;
    
    // Create a connection
    pqxx::connection connect() const;
    
    // Getters for configuration
    std::string get_dbname() const { return dbname_; }
    std::string get_user() const { return db_user_; }
    std::string get_host() const { return db_host_; }
    std::string get_port() const { return db_port_; }
    
private:
    std::string dbname_;
    std::string db_user_;
    std::string db_host_;
    std::string db_port_;
    std::string db_password_;
    
    // Load .env file into environment
    void load_env_file();
    
    // Load configuration from environment variables
    void load_env_config();
};

}

#endif // DB_CONNECTOR_H
