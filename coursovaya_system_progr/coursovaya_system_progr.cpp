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
            return true;
        }
    }

    void createTestTable() {
        const char* createTableSQL = R"(
            CREATE TABLE IF NOT EXISTS test_table (
                id INTEGER PRIMARY KEY AUTOINCREMENT,
                name TEXT NOT NULL,
                birthdate DATE NOT NULL
            );
        )";

        const char* insertDataSQL = R"(
            INSERT INTO test_table (name, birthdate)
            VALUES ('Alice', '1994-05-10'), ('Bob', '1999-02-15'), ('Charlie', '1988-11-20');
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

    sqlite3* getDB() const {
        return db;
    }

    static int callback(void* NotUsed, int argc, char** argv, char** azColName) {
        for (int i = 0; i < argc; i++) {
            std::cout << azColName[i] << ": " << (argv[i] ? argv[i] : "NULL") << std::endl;
        }
        std::cout << std::endl;
        return 0;
    }

private:
    sqlite3* db;
};

void executeSQL(sqlite3* db) {
    std::string sql;
    while (true) {
        std::cout << "Введите SQL-команду (или 'exit' для выхода): ";
        std::getline(std::cin, sql);

        if (sql == "exit") {
            break;
        }

        char* errMsg = nullptr;
        int result = sqlite3_exec(db, sql.c_str(), Database::callback, nullptr, &errMsg);
        if (result != SQLITE_OK) {
            std::cerr << "Ошибка SQL: " << errMsg << std::endl;
            sqlite3_free(errMsg);
        }
        else {
            std::cout << "SQL-команда успешно выполнена!" << std::endl;
        }
    }
}

int main() {
    Database db;
    setlocale(LC_ALL, "ru"); // установка правильной кодировки в выводе
    std::string dbPath;
    std::cout << "Введите путь к базе данных: ";
    std::getline(std::cin, dbPath);

    if (db.connect(dbPath)) {
        db.createTestTable();
        executeSQL(db.getDB());
    }

    return 0;
}
