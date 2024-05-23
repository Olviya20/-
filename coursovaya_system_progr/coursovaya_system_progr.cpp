#include <iostream>
#include <sqlite3.h>
#include <string>

class Database {
public:
    Database() : db(nullptr) {}

    ~Database() {
        if (db) {
            sqlite3_close(db);
        }
    }

    bool connect(const std::string& dbPath) {
        int result = sqlite3_open(dbPath.c_str(), &db);
        if (result) {
            std::cerr << "Не удалось открыть базу данных. Ошибка: " << sqlite3_errmsg(db) << std::endl;
            return false;
        }
        else {
            std::cout << "База данных открыта" << std::endl;
            createTestTable();
            return true;
        }
    }

    void executeSQL() {
        std::string sql;
        while (true) {
            std::cout << "Введите SQL-команду (или 'exit' для выхода): ";
            std::getline(std::cin, sql);

            if (sql == "exit") {
                break;
            }

            if (sql.find("SELECT") == 0 || sql.find("select") == 0) {
                char* errMsg = nullptr;
                int result = sqlite3_exec(db, sql.c_str(), callback, nullptr, &errMsg);
                if (result != SQLITE_OK) {
                    std::cerr << "Ошибка SQL: " << errMsg << std::endl;
                    sqlite3_free(errMsg);
                }
            }
            else {
                char* errMsg = nullptr;
                int result = sqlite3_exec(db, sql.c_str(), nullptr, nullptr, &errMsg);
                if (result != SQLITE_OK) {
                    std::cerr << "Ошибка SQL: " << errMsg << std::endl;
                    sqlite3_free(errMsg);
                }
                else {
                    std::cout << "SQL-команда успешно выполнена!" << std::endl;
                }
            }
        }
    }

private:
    void createTestTable() {
        const char* createTableSQL = R"(
            CREATE TABLE IF NOT EXISTS test_table (
                id INTEGER PRIMARY KEY AUTOINCREMENT,
                name TEXT NOT NULL,
                age INTEGER NOT NULL
            );
        )";

        const char* insertDataSQL = R"(
            INSERT INTO test_table (name, age)
            VALUES ('Alice', 30), ('Bob', 25), ('Charlie', 35);
        )";

        char* errMsg = nullptr;
        int result = sqlite3_exec(db, createTableSQL, nullptr, nullptr, &errMsg);
        if (result != SQLITE_OK) {
            std::cerr << "Ошибка создания таблицы: " << errMsg << std::endl;
            sqlite3_free(errMsg);
        }
        else {
            std::cout << "Тестовая таблица 'test_table' успешно создана." << std::endl;

            result = sqlite3_exec(db, insertDataSQL, nullptr, nullptr, &errMsg);
            if (result != SQLITE_OK) {
                std::cerr << "Ошибка вставки данных: " << errMsg << std::endl;
                sqlite3_free(errMsg);
            }
            else {
                std::cout << "Тестовые данные успешно вставлены." << std::endl;
            }
        }
    }

    static int callback(void* NotUsed, int argc, char** argv, char** azColName) {
        for (int i = 0; i < argc; i++) {
            std::cout << azColName[i] << ": " << (argv[i] ? argv[i] : "NULL") << std::endl;
        }
        std::cout << std::endl;
        return 0;
    }

    sqlite3* db;
};

int main() {
    Database db;
    setlocale(LC_ALL, "ru"); // установка правильной кодировки в выводе
    std::string dbPath;
    std::cout << "Введите путь к базе данных: ";
    std::getline(std::cin, dbPath);

    if (db.connect(dbPath)) {
        db.executeSQL();
    }

    return 0;
}
