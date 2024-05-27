#include <iostream>
#include <vector>
#include <memory>
#include <string>
#include <conio.h>
#include <locale>
#include <cstdlib>
#include <ctime>
#include <windows.h>
#include <thread>
#include <future>
#include <atomic>

class GameElement {
public:
    virtual void updatePosition(int x, int y) = 0;
    virtual ~GameElement() = default;
};

class Character : public GameElement {
protected:
    int x, y;
    int health;
    int baseAttack;
    std::string name;
    char symbol;
public:
    Character(std::string name, char symbol, int health, int baseAttack)
        : name(name), x(0), y(0), health(health), baseAttack(baseAttack), symbol(symbol) {}

    virtual void attack(Character& target) {
        target.takeDamage(baseAttack);
    }

    virtual void defend() = 0;
    virtual void checkBackpack() = 0;
    virtual void pickItem() = 0;

    void updatePosition(int newX, int newY) override {
        x = newX;
        y = newY;
    }

    int getX() const { return x; }
    int getY() const { return y; }
    char getSymbol() const { return symbol; }
    std::string getName() const { return name; }
    int getHealth() const { return health; }
    int getBaseAttack() const { return baseAttack; }

    void takeDamage(int damage) {
        health -= damage;
        if (health < 0) health = 0;
    }

    void heal(int amount) {
        health += amount;
    }

    bool isAlive() const {
        return health > 0;
    }

    void setBaseAttack(int value) {
        baseAttack = value;
    }

    virtual ~Character() = default;
};

class Warrior : public Character {
    int rage;
public:
    Warrior(std::string name, char symbol) : Character(name, symbol, 150, 20), rage(0) {}

    void defend() override {
        std::cout << name << " защищается щитом.\n";
    }

    void checkBackpack() override {
        std::cout << name << " проверяет свой рюкзак.\n";
    }

    void pickItem() override {
        std::cout << name << " подбирает предмет.\n";
    }
};

class Archer : public Character {
    int dexterity;
public:
    Archer(std::string name, char symbol) : Character(name, symbol, 100, 15), dexterity(15) {}

    void defend() override {
        std::cout << name << " уворачивается от атаки.\n";
    }

    void checkBackpack() override {
        std::cout << name << " проверяет свой рюкзак.\n";
    }

    void pickItem() override {
        std::cout << name << " подбирает предмет.\n";
    }
};

class Mage : public Character {
    int mana;
public:
    Mage(std::string name, char symbol) : Character(name, symbol, 80, 25), mana(100) {}

    void defend() override {
        std::cout << name << " создает магический щит.\n";
    }

    void checkBackpack() override {
        std::cout << name << " проверяет свой рюкзак.\n";
    }

    void pickItem() override {
        std::cout << name << " подбирает предмет.\n";
    }
};

class Object : public GameElement {
protected:
    int x, y;
    std::string name;
public:
    Object(std::string name) : name(name), x(0), y(0) {}

    void updatePosition(int newX, int newY) override {
        x = newX;
        y = newY;
    }

    std::string getName() const { return name; }
    int getX() const { return x; }
    int getY() const { return y; }

    virtual void applyEffect(Character& player) = 0;

    virtual ~Object() = default;
};

class HealthPotion : public Object {
    int healingAmount;
public:
    HealthPotion(std::string name, int healingAmount) : Object(name), healingAmount(healingAmount) {}

    void applyEffect(Character& player) override {
        player.heal(healingAmount);
        std::cout << player.getName() << " использовал " << name << " и восстановил " << healingAmount << " здоровья.\n";
    }
};

class StrengthPotion : public Object {
    int attackIncrease;
public:
    StrengthPotion(std::string name, int attackIncrease) : Object(name), attackIncrease(attackIncrease) {}

    void applyEffect(Character& player) override {
        std::cout << player.getName() << " использовал " << name << " и теперь наносит на " << attackIncrease << " больше урона.\n";
        player.setBaseAttack(player.getBaseAttack() + attackIncrease);
    }
};

class GameField {
    int width, height;
    std::vector<std::vector<char>> field;
    std::shared_ptr<Character> player1, player2;
    std::vector<std::shared_ptr<Object>> objects;
    std::atomic<bool> gameOver;
public:
    GameField(int width, int height) : width(width), height(height), field(width, std::vector<char>(height, '.')), gameOver(false) {}

    void addPlayer(std::shared_ptr<Character> player) {
        int x, y;
        do {
            x = rand() % width;
            y = rand() % height;
        } while (field[x][y] != '.');

        player->updatePosition(x, y);
        if (player1 == nullptr) {
            player1 = player;
        }
        else {
            player2 = player;
        }
        field[x][y] = player->getSymbol();
    }

    void addObject(std::shared_ptr<Object> object) {
        int x, y;
        do {
            x = rand() % width;
            y = rand() % height;
        } while (field[x][y] != '.');

        object->updatePosition(x, y);
        objects.push_back(object);
        field[x][y] = (object->getName() == "Маленькая аптечка") ? 'h' :
            (object->getName() == "Большая аптечка") ? 'H' :
            (object->getName() == "Маленькое зелье силы") ? 's' : 'S';
    }

    void updateField() {
        for (auto& row : field) {
            std::fill(row.begin(), row.end(), '.');
        }
        field[player1->getX()][player1->getY()] = player1->getSymbol();
        field[player2->getX()][player2->getY()] = player2->getSymbol();
        for (const auto& object : objects) {
            field[object->getX()][object->getY()] = (object->getName() == "Маленькая аптечка") ? 'h' :
                (object->getName() == "Большая аптечка") ? 'H' :
                (object->getName() == "Маленькое зелье силы") ? 's' : 'S';
        }
    }

    void showField() {
        COORD coord = { 0, 0 };
        SetConsoleCursorPosition(GetStdHandle(STD_OUTPUT_HANDLE), coord);

        std::cout << "\nИгровое поле:\n";
        for (const auto& row : field) {
            for (const auto& cell : row) {
                std::cout << cell << " ";
            }
            std::cout << "\n";
        }
        std::cout << "\nГайд управления 1: WASD\nГайд управления 2: IJKL\n";
        std::cout << "Описание персонажа Игрока 1: " << player1->getName() << " (Здоровье: " << player1->getHealth() << ", Урон: " << player1->getBaseAttack() << ")\n";
        std::cout << "Описание персонажа Игрока 2: " << player2->getName() << " (Здоровье: " << player2->getHealth() << ", Урон: " << player2->getBaseAttack() << ")\n";
    }

    void movePlayer(std::shared_ptr<Character> player, int dx, int dy) {
        int newX = player->getX() + dx;
        int newY = player->getY() + dy;
        if (newX >= 0 && newX < width && newY >= 0 && newY < height) {
            char newCell = field[newX][newY];
            if (newCell == '.') {
                field[player->getX()][player->getY()] = '.';
                player->updatePosition(newX, newY);
                field[newX][newY] = player->getSymbol();
                updateField();
            }
            else if (newCell == 'h' || newCell == 'H' || newCell == 's' || newCell == 'S') {
                for (auto it = objects.begin(); it != objects.end(); ++it) {
                    if ((*it)->getX() == newX && (*it)->getY() == newY) {
                        (*it)->applyEffect(*player);
                        objects.erase(it);
                        break;
                    }
                }
                field[player->getX()][player->getY()] = '.';
                player->updatePosition(newX, newY);
                field[newX][newY] = player->getSymbol();
                updateField();
            }
            else if (newCell == player1->getSymbol() || newCell == player2->getSymbol()) {
                auto target = (player == player1) ? player2 : player1;
                player->attack(*target);
                if (checkCollision()) {
                    if (isGameOver()) {
                        return;
                    }
                }
            }
        }
    }

    bool checkCollision() {
        return !player1->isAlive() || !player2->isAlive();
    }

    bool isGameOver() {
        if (!gameOver) {
            if (!player1->isAlive()) {
                std::cout << player1->getName() << " проиграл!\n";
                gameOver = true;
                return true;
            }
            else if (!player2->isAlive()) {
                std::cout << player2->getName() << " проиграл!\n";
                gameOver = true;
                return true;
            }
        }
        return false;
    }

    void controlPlayers() {
        while (!gameOver) {
            showField();
            if (_kbhit()) {
                char ch = _getch();
                switch (ch) {
                case 'w':
                    movePlayer(player1, -1, 0);
                    break;
                case 's':
                    movePlayer(player1, 1, 0);
                    break;
                case 'a':
                    movePlayer(player1, 0, -1);
                    break;
                case 'd':
                    movePlayer(player1, 0, 1);
                    break;
                case 'i':
                    movePlayer(player2, -1, 0);
                    break;
                case 'k':
                    movePlayer(player2, 1, 0);
                    break;
                case 'j':
                    movePlayer(player2, 0, -1);
                    break;
                case 'l':
                    movePlayer(player2, 0, 1);
                    break;
                default:
                    break;
                }

                if (isGameOver()) {
                    break;
                }
            }
            Sleep(100); // Уменьшено для более быстрого перемещения
        }
    }

    void spawnRandomObjects() {
        while (!gameOver) {
            int spawnChance = rand() % 100;
            if (spawnChance < 40) {
                addObject(std::make_shared<HealthPotion>("Маленькая аптечка", 30));
            }
            else if (spawnChance < 70) {
                addObject(std::make_shared<HealthPotion>("Большая аптечка", 100));
            }
            else if (spawnChance < 90) {
                addObject(std::make_shared<StrengthPotion>("Маленькое зелье силы", 5));
            }
            else {
                addObject(std::make_shared<StrengthPotion>("Большое зелье силы", 15));
            }
            updateField();
            std::this_thread::sleep_for(std::chrono::seconds(10));
        }
    }

    void reset() {
        gameOver = false;
        player1.reset();
        player2.reset();
        objects.clear();
        field.assign(width, std::vector<char>(height, '.'));
    }

    bool getGameOver() const {
        return gameOver;
    }

    void setGameOver(bool state) {
        gameOver = state;
    }
};

class GameProcess {
    GameField field;
public:
    GameProcess(int width, int height) : field(width, height) {}

    void showMainMenu() {
        std::cout << "Главное меню:\n1. Играть\n2. Выйти\n";
    }

    void showCharacterCreationMenu(int playerNum) {
        std::cout << "Игрок " << playerNum << ", выберите класс:\n";
        std::cout << "1. Войн\n2. Лучник\n3. Маг\n";
    }

    std::shared_ptr<Character> createCharacter(int playerNum, char symbol) {
        showCharacterCreationMenu(playerNum);
        int choice;
        std::cin >> choice;

        switch (choice) {
        case 1:
            return std::make_shared<Warrior>("Игрок " + std::to_string(playerNum), symbol);
        case 2:
            return std::make_shared<Archer>("Игрок " + std::to_string(playerNum), symbol);
        case 3:
            return std::make_shared<Mage>("Игрок " + std::to_string(playerNum), symbol);
        default:
            std::cout << "Неверный выбор. Попробуйте снова.\n";
            return createCharacter(playerNum, symbol);
        }
    }

    void start() {
        std::cout << "Начало игры...\n";
        auto player1 = createCharacter(1, 'X');
        auto player2 = createCharacter(2, 'O');

        field.addPlayer(player1);
        field.addPlayer(player2);

        std::thread objectSpawner(&GameField::spawnRandomObjects, &field);

        field.controlPlayers();
        objectSpawner.join();

        showEndMenu();
    }

    void showEndMenu() {
        std::cout << "Конец игры. Выберите действие:\n1. Новая игра\n2. Выйти\n";
        int choice;
        while (true) {
            std::cin >> choice;
            if (choice == 1) {
                system("cls"); // Очистка экрана
                launch();
                break;
            }
            else if (choice == 2) {
                std::cout << "Выход из игры...\n";
                exit(0);
            }
            else {
                std::cout << "Неверный выбор. Попробуйте снова.\n";
            }
        }
    }

    void launch() {
        srand(static_cast<unsigned int>(time(nullptr))); // Инициализация генератора случайных чисел
        while (true) {
            showMainMenu();
            int choice;
            std::cin >> choice;

            if (choice == 1) {
                field.reset();
                start();
            }
            else if (choice == 2) {
                std::cout << "Выход из игры...\n";
                field.setGameOver(true); // завершение игры
                break;
            }
            else {
                std::cout << "Неверный выбор. Попробуйте снова.\n";
            }
        }
    }
};

int main() {
    setlocale(LC_ALL, "Russian");
    GameProcess game(15, 15); // Увеличенное поле
    game.launch();

    return 0;
}
