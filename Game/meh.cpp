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
        std::cout << name << " ���������� �����.\n";
    }

    void checkBackpack() override {
        std::cout << name << " ��������� ���� ������.\n";
    }

    void pickItem() override {
        std::cout << name << " ��������� �������.\n";
    }
};

class Archer : public Character {
    int dexterity;
public:
    Archer(std::string name, char symbol) : Character(name, symbol, 100, 15), dexterity(15) {}

    void defend() override {
        std::cout << name << " ������������� �� �����.\n";
    }

    void checkBackpack() override {
        std::cout << name << " ��������� ���� ������.\n";
    }

    void pickItem() override {
        std::cout << name << " ��������� �������.\n";
    }
};

class Mage : public Character {
    int mana;
public:
    Mage(std::string name, char symbol) : Character(name, symbol, 80, 25), mana(100) {}

    void defend() override {
        std::cout << name << " ������� ���������� ���.\n";
    }

    void checkBackpack() override {
        std::cout << name << " ��������� ���� ������.\n";
    }

    void pickItem() override {
        std::cout << name << " ��������� �������.\n";
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
        std::cout << player.getName() << " ����������� " << name << " � ����������� " << healingAmount << " ��������.\n";
    }
};

class StrengthPotion : public Object {
    int attackIncrease;
public:
    StrengthPotion(std::string name, int attackIncrease) : Object(name), attackIncrease(attackIncrease) {}

    void applyEffect(Character& player) override {
        std::cout << player.getName() << " ����������� " << name << " � ������ ������� �� " << attackIncrease << " ������ �����.\n";
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
        field[x][y] = (object->getName() == "��������� �������") ? 'h' :
            (object->getName() == "������� �������") ? 'H' :
            (object->getName() == "��������� ����� ����") ? 's' : 'S';
    }

    void updateField() {
        for (auto& row : field) {
            std::fill(row.begin(), row.end(), '.');
        }
        field[player1->getX()][player1->getY()] = player1->getSymbol();
        field[player2->getX()][player2->getY()] = player2->getSymbol();
        for (const auto& object : objects) {
            field[object->getX()][object->getY()] = (object->getName() == "��������� �������") ? 'h' :
                (object->getName() == "������� �������") ? 'H' :
                (object->getName() == "��������� ����� ����") ? 's' : 'S';
        }
    }

    void showField() {
        COORD coord = { 0, 0 };
        SetConsoleCursorPosition(GetStdHandle(STD_OUTPUT_HANDLE), coord);

        std::cout << "\n������� ����:\n";
        for (const auto& row : field) {
            for (const auto& cell : row) {
                std::cout << cell << " ";
            }
            std::cout << "\n";
        }
        std::cout << "\n���� ���������� 1: WASD\n���� ���������� 2: IJKL\n";
        std::cout << "�������� ��������� ������ 1: " << player1->getName() << " (��������: " << player1->getHealth() << ", ����: " << player1->getBaseAttack() << ")\n";
        std::cout << "�������� ��������� ������ 2: " << player2->getName() << " (��������: " << player2->getHealth() << ", ����: " << player2->getBaseAttack() << ")\n";
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
                std::cout << player1->getName() << " ��������!\n";
                gameOver = true;
                return true;
            }
            else if (!player2->isAlive()) {
                std::cout << player2->getName() << " ��������!\n";
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
            Sleep(100); // ��������� ��� ����� �������� �����������
        }
    }

    void spawnRandomObjects() {
        while (!gameOver) {
            int spawnChance = rand() % 100;
            if (spawnChance < 40) {
                addObject(std::make_shared<HealthPotion>("��������� �������", 30));
            }
            else if (spawnChance < 70) {
                addObject(std::make_shared<HealthPotion>("������� �������", 100));
            }
            else if (spawnChance < 90) {
                addObject(std::make_shared<StrengthPotion>("��������� ����� ����", 5));
            }
            else {
                addObject(std::make_shared<StrengthPotion>("������� ����� ����", 15));
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
        std::cout << "������� ����:\n1. ������\n2. �����\n";
    }

    void showCharacterCreationMenu(int playerNum) {
        std::cout << "����� " << playerNum << ", �������� �����:\n";
        std::cout << "1. ����\n2. ������\n3. ���\n";
    }

    std::shared_ptr<Character> createCharacter(int playerNum, char symbol) {
        showCharacterCreationMenu(playerNum);
        int choice;
        std::cin >> choice;

        switch (choice) {
        case 1:
            return std::make_shared<Warrior>("����� " + std::to_string(playerNum), symbol);
        case 2:
            return std::make_shared<Archer>("����� " + std::to_string(playerNum), symbol);
        case 3:
            return std::make_shared<Mage>("����� " + std::to_string(playerNum), symbol);
        default:
            std::cout << "�������� �����. ���������� �����.\n";
            return createCharacter(playerNum, symbol);
        }
    }

    void start() {
        std::cout << "������ ����...\n";
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
        std::cout << "����� ����. �������� ��������:\n1. ����� ����\n2. �����\n";
        int choice;
        while (true) {
            std::cin >> choice;
            if (choice == 1) {
                system("cls"); // ������� ������
                launch();
                break;
            }
            else if (choice == 2) {
                std::cout << "����� �� ����...\n";
                exit(0);
            }
            else {
                std::cout << "�������� �����. ���������� �����.\n";
            }
        }
    }

    void launch() {
        srand(static_cast<unsigned int>(time(nullptr))); // ������������� ���������� ��������� �����
        while (true) {
            showMainMenu();
            int choice;
            std::cin >> choice;

            if (choice == 1) {
                field.reset();
                start();
            }
            else if (choice == 2) {
                std::cout << "����� �� ����...\n";
                field.setGameOver(true); // ���������� ����
                break;
            }
            else {
                std::cout << "�������� �����. ���������� �����.\n";
            }
        }
    }
};

int main() {
    setlocale(LC_ALL, "Russian");
    GameProcess game(15, 15); // ����������� ����
    game.launch();

    return 0;
}
