#include <fstream>
#include <map>
#include <vector>
#include <memory>
#include <algorithm>

using namespace std;
ifstream fin("input.txt");
ofstream fout("output.txt");
class Weapon;
class Archer;
class Wizard;
class Fighter;

//create container of characters, which is used in main in generic type
template<typename C>
class ContainerWithCharacters {
private:
    map<string, shared_ptr<C>> characters;
public:
    //get element by name
    shared_ptr<C> findByName(string name) {
        return characters.at(name);
    }

    void addCharacter(string name, shared_ptr<C> character) {
        characters.insert({name, character});
    }
    //get vector of characters, it's need for show characters, because I cannot get every character by name in that method
    vector<shared_ptr<C>> getCharacters() {
        vector<shared_ptr<C>> charactersVector;
        for (typename map<string, shared_ptr<C>>::iterator it = characters.begin(); it != characters.end(); ++it) {
            charactersVector.push_back(it->second);
        }
        return charactersVector;
    }
    //delete character by name
    void erase(string name) {
        characters.erase(name);
    }
};

class PhysicalItem;
//concept as a class template with specialization for objects derived from PhysicalItem
template<typename T>
concept DerivedFromPhysicalItem = std::is_base_of<PhysicalItem, T>::value;
//using concept
template <DerivedFromPhysicalItem T>
class Container {
public:class ContainerMap {
    private:
        map<string, T> elements;
    public:
        void addItem(string nameOfItem, T newItem) {
            elements.insert({nameOfItem, newItem});
        }
        void removeItem(T newItem) {
            elements.erase(newItem);
        }
        bool find(T item) {
            return true;
        }
        T find(string itemName) {
            return elements.at(itemName);
        }
    };
    //I used vector instead of map, but it can be done using map(using map it is easier, but I decided do it that)
    class ContainerVector{
    private:
        vector<shared_ptr<T>> elements;
    public:
        //get element by name in vector
        shared_ptr<T> find(string nameOfItem) {
            for (auto item : elements) {
                if (item->getName() == nameOfItem) {
                    return item;
                }
            }
            return nullptr;
        }
        //get index of element, which I will remove from vector(for that I need index)
        int findIndex(string nameOfItem) {
            int index = 0;
            for (auto item : elements) {
                if (item->getName() == nameOfItem) {
                    return index;
                }
                index++;
            }
            return -1;
        }
        void addItem(shared_ptr<T> newItem) {
            elements.push_back(newItem);
        }
        //get all element from vector for methods: showWeapons, showPotions, showSpells
        vector<shared_ptr<T>> getItems() {
            return elements;
        }
        //delete element from vector by index, it is used in arsenal for example
        void erase(int index) {
            elements.erase(elements.begin() + index);
        }
        int size() {
            return elements.size();
        }
    };
};

//abstract class ContainerWithMaxCapacity, which almost doesn't make sense in my code)
template<DerivedFromPhysicalItem T>
class ContainerWithMaxCapacity : public Container<T>::ContainerVector{
private:
    int maxCapacity;
public:
    virtual void addItem(shared_ptr<T> item) = 0;
    virtual Container<T>::ContainerVector getItems() = 0;
    virtual shared_ptr<T> find(string name) = 0;
    virtual void erase(int index) = 0;
    virtual int findIndex(string nameOfItem) = 0;
    virtual int size() = 0;
//    virtual void show() = 0;
};


class PhysicalItem;

//abstract class Character, in my code I don't store any information in Character, all information stored in
// derivative classes of Character, but this class used for generic storing in container
class Character {
public:
    virtual string getName() = 0;
    virtual int getHP() = 0;
    virtual void takeDamage(int damage) = 0;
private:
    int healthPoints;
    string name;
protected:
    virtual void obtainItemSideEffect(shared_ptr<PhysicalItem> item) = 0;
};

//abstract class PhysicalItem, in my code I don't store any information in PhysicalItem, all information stored in
// derivative classes of PhysicalItem
class PhysicalItem {
private:
    bool isUsableOnce;
    shared_ptr<Character> owner;
    string name;
public:
    virtual string getName() = 0;
};
class Fighter;


class Weapon : public PhysicalItem {
private:
    int damage;
    string name;
    shared_ptr<Character> owner;
public:
    //constructor of Weapon, which has three parameters
    Weapon(shared_ptr<Character> own, string weaponName, int damageValue) {
        owner = own;
        name = weaponName;
        damage = damageValue;
    }
    string getName() override {
        return name;
    }
    int getDamage() {
        return damage;
    }
};
class Potion : public PhysicalItem {
private:
    int healValue;
    string name;
    shared_ptr<Character> owner;
public:
    //constructor of Potion, which has three parameters
    Potion(shared_ptr<Character> own, string potionName, int heal) {
        owner = own;
        name = potionName;
        healValue = heal;
    }
    int getHealValue() {
        return healValue;
    }
    string getName() override {
        return name;
    }
};
class Spell : public PhysicalItem {
private:
    vector<shared_ptr<Character>> allowedTargets;
    string name;
    shared_ptr<Character> owner;
public:
    //constructor of Spell, which has three parameters
    //targets it is a vector of characters, which can be killed by spell
    Spell(shared_ptr<Character> own, string potionName, vector<shared_ptr<Character>> targets) {
        owner = own;
        name = potionName;
        allowedTargets = targets;
    }
    int numAllowedTargets() {
        return allowedTargets.size();
    }
    //check character can be killed using this spell or not
    bool consistTarget(shared_ptr<Character> target) {
        for (int i = 0; i < allowedTargets.size(); ++i) {
            if (allowedTargets[i] == target) {
                return true;
            }
        }
        return false;
    }
    string getName() override{
        return name;
    }
};


//class Arsenal which contains a vector (from Container) of Weapons, all another methods are clear
class Arsenal : public ContainerWithMaxCapacity<Weapon> {
private:
    Container<Weapon>::ContainerVector container;
public:
    shared_ptr<Weapon> find(string nameOfItem) {
        return container.find(nameOfItem);
    }
    void addItem(shared_ptr<Weapon> item) override {
        container.addItem(item);
    }
    Container<Weapon>::ContainerVector getItems() override {
        return container;
    }
    void erase(int index) override {
        container.erase(index);
    }
    int findIndex(string nameOfItem) override {
        return container.findIndex(nameOfItem);
    }
    int size() override {
        return container.size();
    }
};

//class MedicalBag which contains a vector (from Container) of Potions, all another methods are clear
class MedicalBag : public ContainerWithMaxCapacity<Potion> {
private:
    Container<Potion>::ContainerVector container;
public:
    shared_ptr<Potion> find(string nameOfItem) {
        return container.find(nameOfItem);
    }
    void addItem(shared_ptr<Potion> item) override {
        container.addItem(item);
    }
    Container<Potion>::ContainerVector getItems() override {
        return container;
    }
    void erase(int index) override {
        container.erase(index);
    }
    int findIndex(string nameOfItem) override {
        return container.findIndex(nameOfItem);
    }
    int size() override {
        return container.size();
    }
};

//class SpellBook which contains a vector (from Container) of Spells, all another methods are clear
class SpellBook : public ContainerWithMaxCapacity<Spell> {
private:
    Container<Spell>::ContainerVector container;
public:
    shared_ptr<Spell> find(string nameOfItem) override {
        return container.find(nameOfItem);
    }
    void addItem(shared_ptr<Spell> item) override {
        container.addItem(item);
    }
    Container<Spell>::ContainerVector getItems() override {
        return container;
    }
    void erase(int index) override {
        container.erase(index);
    }
    int findIndex(string nameOfItem) override {
        return container.findIndex(nameOfItem);
    }
    int size() override {
        return container.size();
    }
};

//Classes below derivative from Character and store containers of elements
// these classes used like based class of Fighter, Archer and Wizard
// as some from them can be different types and has several containers
class WeaponUser : public Character {
protected:
    Arsenal arsenal;
public:
    virtual vector<shared_ptr<Weapon>> showWeapons() = 0;
};
class PotionUser : public Character {
protected:
    MedicalBag medicalBag;
public:
    virtual vector<shared_ptr<Potion>> showPotions() = 0;
};
class SpellUser : public Character {
protected:
    SpellBook spellBook;
public:
    virtual vector<shared_ptr<Spell>> showSpells() = 0;
};


class Fighter : public WeaponUser, PotionUser {
public:
    //constructor of Fighter, which give containers arsenal and medicalBac, according to base classes
    Fighter(string n, int health) {
        name = n;
        healthPoints = health;
        arsenal = arsenal;
        medicalBag = medicalBag;
    }
    //method for get weapon by name, in this method I check weapon is exist or not
    shared_ptr<Weapon> getWeapon(string weaponName) {
        shared_ptr<Weapon> weapon = arsenal.find(weaponName);
        if (weapon != nullptr) {
            int index = arsenal.findIndex(weaponName);
            if (index == -1) {
                return nullptr;
            } else {
                return weapon;
            }
        } else {
            return nullptr;
        }
    }
    //method for get potion by name, in this method I check potion is exist or not
    shared_ptr<Potion> getPotion(string potionName) {
        shared_ptr<Potion> potion = medicalBag.find(potionName);
        if (potion != nullptr) {
            int index = medicalBag.findIndex(potionName);
            if (index == -1) {
                return nullptr;
            } else {
                return potion;
            }
        } else {
            return nullptr;
        }
    }
    //delete potion from container by name
    void removePotion(string potionName) {
        int index = medicalBag.findIndex(potionName);
        medicalBag.erase(index);
    }
    int getHP() override {
        return healthPoints;
    }
    string getName() override {
        return name;
    }
    //requirements
    const int MAX_ALLOWED_WEAPONS = 3;
    const int MAX_ALLOWED_POTIONS = 5;

    //method for get vector of weapons, it's need for output
    vector<shared_ptr<Weapon>> showWeapons() override {
        auto weapons = arsenal.getItems().getItems();
        return weapons;
    }
    //method for get vector of potions, it's need for output
    vector<shared_ptr<Potion>> showPotions() override {
        auto potions = medicalBag.getItems().getItems();
        return potions;
    }
    //using this method I add PhysicalItem in container,
    // check type of PhysicalItem using dynamic_pointer_cast and add this item in container
    void obtainItemSideEffect(shared_ptr<PhysicalItem> item) override {
        auto weapon = dynamic_pointer_cast<Weapon>(item);
        if (weapon != nullptr) {
            if (arsenal.size() < MAX_ALLOWED_WEAPONS) {
                arsenal.addItem(weapon);
                fout << name << " just obtained a new weapon called " << weapon->getName() << "." << endl;
            } else {
                fout << "Error caught" << endl;
            }
            return;
        } else {
            auto potion = dynamic_pointer_cast<Potion>(item);
            if (potion != nullptr) {
                if (medicalBag.size() < MAX_ALLOWED_POTIONS) {
                    medicalBag.addItem(potion);
                    fout << name << " just obtained a new potion called " << potion->getName() << "." << endl;
                } else {
                    fout << "Error caught" << endl;
                }
                return;
            } else {
                return;
            }
        }
    }
    void takeDamage(int damage) override{
        healthPoints = healthPoints-damage;
    }
private:
    string name;
    int healthPoints;
};
class Archer : public WeaponUser, PotionUser, SpellUser {
public:
    //constructor of Archer, which give containers arsenal, medicalBac and spellBook, according to base classes
    Archer(string n, int health) {
        name = n;
        healthPoints = health;
        arsenal = arsenal;
        medicalBag = medicalBag;
        spellBook = spellBook;
    }
    //method for get weapon by name, in this method I check weapon is exist or not
    shared_ptr<Weapon> getWeapon(string weaponName) {
        shared_ptr<Weapon> weapon = arsenal.find(weaponName);
        if (weapon != nullptr) {
            int index = arsenal.findIndex(weaponName);
            if (index == -1) {
                return nullptr;
            } else {
                return weapon;
            }
        } else {
            return nullptr;
        }
    }
    //method for get spell by name, in this method I check spell is exist or not
    shared_ptr<Spell> getSpell(string spellName) {
        shared_ptr<Spell> spell = spellBook.find(spellName);
        if (spell != nullptr) {
            if (spell->numAllowedTargets() == 0) {
                return nullptr;
            }
            int index = spellBook.findIndex(spellName);
            if (index == -1) {
                return nullptr;
            } else {
                return spell;
            }
        } return nullptr;
    }
    //delete spell from container by name
    void removeSpell(string spellName) {
        int index = spellBook.findIndex(spellName);
        spellBook.erase(index);
    }
    //method for get potion by name, in this method I check potion is exist or not
    shared_ptr<Potion> getPotion(string potionName) {
        shared_ptr<Potion> potion = medicalBag.find(potionName);
        if (potion != nullptr) {
            int index = medicalBag.findIndex(potionName);
            if (index == -1) {
                return nullptr;
            } else {
                return potion;
            }
        } else {
            return nullptr;
        }
    }
    //delete potion from container by name
    void removePotion(string potionName) {
        int index = medicalBag.findIndex(potionName);
        medicalBag.erase(index);
    }
    int getHP() override {
        return healthPoints;
    }
    string getName() override {
        return name;
    }
    const int MAX_ALLOWED_WEAPONS = 2;
    const int MAX_ALLOWED_POTIONS = 3;
    const int MAX_ALLOWED_SPELLS = 2;
    //method for get vector of weapons, it's need for output
    vector<shared_ptr<Weapon>> showWeapons() override {
        auto weapons = arsenal.getItems().getItems(); // Предполагается, что getItems() возвращает вектор элементов
        return weapons;
    }
    //method for get vector of potions, it's need for output
    vector<shared_ptr<Potion>> showPotions() override {
        auto potions = medicalBag.getItems().getItems(); // Предполагается, что getItems() возвращает вектор элементов
        return potions;
    }
    //method for get vector of spells, it's need for output
    vector<shared_ptr<Spell>> showSpells() override {
        auto spells = spellBook.getItems().getItems(); // Предполагается, что getItems() возвращает вектор элементов
        return spells;
    }
    //using this method I add PhysicalItem in container,
    // check type of PhysicalItem using dynamic_pointer_cast and add this item in container
    void obtainItemSideEffect(shared_ptr<PhysicalItem> item) override {
        auto weapon = dynamic_pointer_cast<Weapon>(item);
        if (weapon != nullptr) {
            if (arsenal.size() < MAX_ALLOWED_WEAPONS) {
                arsenal.addItem(weapon);
                fout << name << " just obtained a new weapon called " << weapon->getName() << "." << endl;
            } else {
                fout << "Error caught" << endl;
            }
            return;
        } else {
            auto potion = dynamic_pointer_cast<Potion>(item);
            if (potion != nullptr) {
                if (medicalBag.size() < MAX_ALLOWED_POTIONS) {
                    medicalBag.addItem(potion);
                    fout << name << " just obtained a new potion called " << potion->getName() << "." << endl;
                } else {
                    fout << "Error caught" << endl;
                }
                return;
            } else {
                auto spell = dynamic_pointer_cast<Spell>(item);
                if (spell != nullptr) {
                    if (spellBook.size() < MAX_ALLOWED_SPELLS) {
                        spellBook.addItem(spell);
                        fout << name << " just obtained a new spell called " << spell->getName() << "." << endl;
                    } else {
                        fout << "Error caught" << endl;
                    }
                    return;
                } else {
                    return;
                }
            }
        }
    }
    void takeDamage(int damage) override {
        healthPoints = healthPoints - damage;
    }
private:
    string name;
    int healthPoints;
};

class Wizard : public PotionUser, SpellUser {
public:
    //constructor of Wizard, which give containers medicalBac and spellBook, according to base classes
    Wizard(string n, int health) {
        name = n;
        healthPoints = health;
        medicalBag = medicalBag;
        spellBook = spellBook;
    }
    //method for get spell by name, in this method I check spell is exist or not
    shared_ptr<Spell> getSpell(string spellName) {
        shared_ptr<Spell> spell = spellBook.find(spellName);
        if (spell != nullptr) {
            if (spell->numAllowedTargets() == 0) {
                return nullptr;
            }
            int index = spellBook.findIndex(spellName);
            if (index == -1) {
                return nullptr;
            } else {
                return spell;
            }
        } return nullptr;
    }
    //delete spell from container by name
    void removeSpell(string spellName) {
        int index = spellBook.findIndex(spellName);
        spellBook.erase(index);
    }
    //method for get potion by name, in this method I check potion is exist or not
    shared_ptr<Potion> getPotion(string potionName) {
        shared_ptr<Potion> potion = medicalBag.find(potionName);
        if (potion != nullptr) {
            int index = medicalBag.findIndex(potionName);
            if (index == -1) {
                return nullptr;
            } else {
                return potion;
            }
        } else {
            return nullptr;
        }
    }
    //delete potion from container by name
    void removePotion(string potionName) {
        int index = medicalBag.findIndex(potionName);
        medicalBag.erase(index);
    }
    int getHP() override {
        return healthPoints;
    }
    string getName() override {
        return name;
    }
    //method for get vector of potions, it's need for output
    vector<shared_ptr<Potion>> showPotions() override {
        auto potions = medicalBag.getItems().getItems(); // Предполагается, что getItems() возвращает вектор элементов
        return potions;
    }
    //method for get vector of spells, it's need for output
    vector<shared_ptr<Spell>> showSpells() override {
        auto spells = spellBook.getItems().getItems(); // Предполагается, что getItems() возвращает вектор элементов
        return spells;
    }
    //using this method I add PhysicalItem in container,
    // check type of PhysicalItem using dynamic_pointer_cast and add this item in container
    void obtainItemSideEffect(shared_ptr<PhysicalItem> item) override {
        auto spell = dynamic_pointer_cast<Spell>(item);
        if (spell != nullptr) {
            if (spellBook.size() < MAX_ALLOWED_SPELLS) {
                spellBook.addItem(spell);
                fout << name << " just obtained a new spell called " << spell->getName() << "." << endl;
            } else {
                fout << "Error caught" << endl;
            }
            return;
        } else {
            auto potion = dynamic_pointer_cast<Potion>(item);
            if (potion != nullptr) {
                if (medicalBag.size() < MAX_ALLOWED_POTIONS) {
                    medicalBag.addItem(potion);
                    fout << name << " just obtained a new potion called " << potion->getName() << "." << endl;
                } else {
                    fout << "Error caught" << endl;
                }
                return;
            } else {
                return;
            }
        }
    }
    void takeDamage(int damage) override {
        healthPoints = healthPoints - damage;
    }
    const int MAX_ALLOWED_POTIONS = 10;
    const int MAX_ALLOWED_SPELLS = 10;
private:
    string name;
    int healthPoints;
};

//my realization of split function by " ", like in Python
vector<string> customSplit(string line, char el) {
    vector<string> lines;
    int ind = 0;
    for (int i = 0; i <= line.size(); ++i) {
        if (line[i] == el || i == line.size()) {
            lines.push_back(line.substr(ind, i-ind));
            ind = i + 1;
        }
    }
    return lines;
}

int main() {
    int n;
    fin >> n;
    string line;
    ContainerWithCharacters<Character> characters;
    while (getline(fin,line)) {
        vector<string> words = customSplit(line, ' ');
        try {
            //situation when create character
            if (words[0] == "Create" && words[1] == "character") {
                string type = words[2];
                string name = words[3];
                int initHP = stoi(words[4]);
                if (initHP < 1 || initHP > 200) {
                    fout
                            << "Character doesn't exist (Attack, Cast, Drink, Create item, spell target is not a character, Dialogue)"
                            << endl;
                    continue;
                }
                //cin code below I added characters in container of character, using static_pointer_Cast
                if (type == "fighter") {
                    auto fighter = make_shared<Fighter>(name, initHP);
                    characters.addCharacter(name, static_pointer_cast<WeaponUser>(fighter));
                    fout << "A new fighter came to town, " << fighter->getName() << "." << endl;
                } else if (type == "archer") {
                    auto archer = make_shared<Archer>(name, initHP);
                    characters.addCharacter(name, static_pointer_cast<WeaponUser>(archer));
                    fout << "A new archer came to town, " << archer->getName() << "." << endl;
                } else if (type == "wizard") {
                    auto wizard = make_shared<Wizard>(name, initHP);
                    characters.addCharacter(name, static_pointer_cast<PotionUser>(wizard));
                    fout << "A new wizard came to town, " << wizard->getName() << "." << endl;
                } else {
                    fout << "Error caught" << endl;
                    continue;
                }
            }
        } catch (exception e){
            fout << "Error caught" << endl;
        }
        try {
            //situation of creating weapon
            if (words[0] == "Create" && words[1] == "item" && words[2] == "weapon") {
                string ownerName = words[3];
                string weaponName = words[4];
                int damageValue = stoi(words[5]);
                if (damageValue < 1 || damageValue > 50) {
                    fout << "Error caught" << endl;
                    continue;
                }
                shared_ptr<Character> character = characters.findByName(ownerName);
                Weapon weapon(character, weaponName, damageValue);

                //using dynamic_pointer_cast I get or this character in this type or nullptr
                // use this I added weapon to containers of fighter and archer
                shared_ptr<Fighter> fighter = dynamic_pointer_cast<Fighter>(character);
                if (fighter) {
                    fighter->obtainItemSideEffect(make_shared<Weapon>(weapon));
                } else {
                    shared_ptr<Archer> archer = dynamic_pointer_cast<Archer>(character);
                    if (archer) {
                        archer->obtainItemSideEffect(make_shared<Weapon>(weapon));
                    } else {
                        fout << "Error caught" << endl;
                    }
                }
            }
        } catch (exception e){
            fout << "Error caught" << endl;
        }
        try {
            //situation of creating potion
            if (words[0] == "Create" && words[1] == "item" && words[2] == "potion") {
                string ownerName = words[3];
                string potionName = words[4];
                int healValue = stoi(words[5]);
                if (healValue >= 1) {
                    shared_ptr<Character> character = characters.findByName(ownerName);
                    Potion potion(character, potionName, healValue);

                    //using dynamic_pointer_cast I get or this character in this type or nullptr
                    // use this I added potion to containers of fighter, archer and wizard
                    shared_ptr<Fighter> fighter = dynamic_pointer_cast<Fighter>(character);
                    if (fighter) {
                        fighter->obtainItemSideEffect(make_shared<Potion>(potion));
                    } else {
                        shared_ptr<Archer> archer = dynamic_pointer_cast<Archer>(character);
                        if (archer) {
                            archer->obtainItemSideEffect(make_shared<Potion>(potion));
                        } else {
                            shared_ptr<Wizard> wizard = dynamic_pointer_cast<Wizard>(character);
                            if (wizard) {
                                wizard->obtainItemSideEffect(make_shared<Potion>(potion));
                            } else {
                                fout << "Error caught" << endl;
                            }
                        }
                    }
                } else {
                    fout << "Error caught" << endl;
                }
            }
        } catch (exception e){
            fout << "Error caught" << endl;
        }
        try {
            // situation of creating spell
            if (words[0] == "Create" && words[1] == "item" && words[2] == "spell") {
                string ownerName = words[3];
                string spellName = words[4];
                int amountOfSpells = stoi(words[5]);
                vector<string> characterNames;
                vector<shared_ptr<Character>> charactersTargets;
                for (int i = 6; i < amountOfSpells + 6; ++i) {
                    characterNames.push_back(words[i]);
                    charactersTargets.push_back(characters.findByName(words[i]));
                }
                shared_ptr<Character> character = characters.findByName(ownerName);
                Spell spell(character, spellName, charactersTargets);

                //using dynamic_pointer_cast I get or this character in this type or nullptr
                // use this I added spell to containers of wizard and archer
                shared_ptr<Wizard> wizard = dynamic_pointer_cast<Wizard>(character);
                if (wizard) {
                    wizard->obtainItemSideEffect(make_shared<Spell>(spell));
                } else {
                    shared_ptr<Archer> archer = dynamic_pointer_cast<Archer>(character);
                    if (archer) {
                        archer->obtainItemSideEffect(make_shared<Spell>(spell));
                    } else {
                        fout << "Error caught" << endl;
                    }
                }
            }
        } catch (exception e){
            fout << "Error caught" << endl;
        }
        try {
            //situation of Attack
            if (words[0] == "Attack") {
                string attackerName = words[1];
                string targetName = words[2];
                string weaponName = words[3];
                shared_ptr<Character> attacker = characters.findByName(attackerName);
                shared_ptr<Character> target = characters.findByName(targetName);

                if (attacker && target) {
                    //using dynamic_pointer_cast I get or this character in this type or nullptr
                    // use this I get type of character and in this type I change the HP of character
                    // and get Weapon, as all information in my code store on that level
                    shared_ptr<Fighter> fighter = dynamic_pointer_cast<Fighter>(attacker);
                    if (fighter) {
                        shared_ptr<Weapon> weaponToFighter = fighter->getWeapon(weaponName);
                        if (weaponToFighter != nullptr) {
                            shared_ptr<Fighter> targetFighter = dynamic_pointer_cast<Fighter>(target);
                            if (targetFighter) {
                                targetFighter->takeDamage(weaponToFighter->getDamage());
                                if (targetFighter->getHP() <= 0) {
                                    fout << attackerName << " attacks " << targetName << " with their " << weaponName
                                         << "!"
                                         << endl;
                                    fout << targetFighter->getName() << " has died..." << endl;
                                    characters.erase(targetName);
                                } else {
                                    fout << attackerName << " attacks " << targetName << " with their " << weaponName
                                         << "!"
                                         << endl;
                                }
                            } else {
                                shared_ptr<Archer> targetArcher = dynamic_pointer_cast<Archer>(target);
                                if (targetArcher) {
                                    targetArcher->takeDamage(weaponToFighter->getDamage());
                                    if (targetArcher->getHP() <= 0) {
                                        fout << attackerName << " attacks " << targetName << " with their "
                                             << weaponName
                                             << "!"
                                             << endl;
                                        fout << targetArcher->getName() << " has died..." << endl;
                                        characters.erase(targetName);
                                    } else {
                                        fout << attackerName << " attacks " << targetName << " with their "
                                             << weaponName
                                             << "!"
                                             << endl;
                                    }
                                } else {
                                    shared_ptr<Wizard> targetWizard = dynamic_pointer_cast<Wizard>(target);
                                    if (targetWizard) {
                                        targetWizard->takeDamage(weaponToFighter->getDamage());
                                        if (targetWizard->getHP() <= 0) {
                                            fout << attackerName << " attacks " << targetName << " with their "
                                                 << weaponName << "!"
                                                 << endl;
                                            fout << targetWizard->getName() << " has died..." << endl;
                                            characters.erase(targetName);
                                        } else {
                                            fout << attackerName << " attacks " << targetName << " with their "
                                                 << weaponName
                                                 << "!" << endl;
                                        }
                                    } else {
                                        fout << "Error caught" << endl;
                                    }
                                }
                            }
                        } else {
                            fout << "Error caught" << endl;
                        }
                    } else {
                        shared_ptr<Archer> archer = dynamic_pointer_cast<Archer>(attacker);
                        if (archer) {
                            shared_ptr<Weapon> weaponToArcher = archer->getWeapon(weaponName);
                            if (weaponToArcher != nullptr) {
                                shared_ptr<Fighter> targetFighter = dynamic_pointer_cast<Fighter>(target);
                                if (targetFighter) {
                                    targetFighter->takeDamage(weaponToArcher->getDamage());
                                    if (targetFighter->getHP() <= 0) {
                                        fout << attackerName << " attacks " << targetName << " with their "
                                             << weaponName
                                             << "!"
                                             << endl;
                                        fout << targetFighter->getName() << " has died..." << endl;
                                        characters.erase(targetName);
                                    } else {
                                        fout << attackerName << " attacks " << targetName << " with their "
                                             << weaponName
                                             << "!"
                                             << endl;
                                    }
                                } else {
                                    shared_ptr<Archer> targetArcher = dynamic_pointer_cast<Archer>(target);
                                    if (targetArcher) {
                                        targetArcher->takeDamage(weaponToArcher->getDamage());
                                        if (targetArcher->getHP() <= 0) {
                                            fout << attackerName << " attacks " << targetName << " with their "
                                                 << weaponName
                                                 << "!"
                                                 << endl;
                                            fout << targetArcher->getName() << " has died..." << endl;
                                            characters.erase(targetName);
                                        } else {
                                            fout << attackerName << " attacks " << targetName << " with their "
                                                 << weaponName
                                                 << "!"
                                                 << endl;
                                        }
                                    } else {
                                        shared_ptr<Wizard> targetWizard = dynamic_pointer_cast<Wizard>(target);
                                        if (targetWizard) {
                                            targetWizard->takeDamage(weaponToArcher->getDamage());
                                            if (targetWizard->getHP() <= 0) {
                                                fout << attackerName << " attacks " << targetName << " with their "
                                                     << weaponName << "!"
                                                     << endl;
                                                fout << targetWizard->getName() << " has died..." << endl;
                                                characters.erase(targetName);
                                            } else {
                                                fout << attackerName << " attacks " << targetName << " with their "
                                                     << weaponName
                                                     << "!" << endl;
                                            }
                                        } else {
                                            fout << "Error caught" << endl;
                                        }
                                    }
                                }
                            } else {
                                fout << "Error caught" << endl;
                            }
                        } else {
                            fout << "Error caught" << endl;
                        }

                    }
                } else {
                    fout << "Error catch" << endl;
                }
            }
        } catch (exception e){
            fout << "Error caught" << endl;
        }
        try {
            // situation of cast
            if (words[0] == "Cast") {
                string casterName = words[1];
                string targetName = words[2];
                string spellName = words[3];
                shared_ptr<Character> caster = characters.findByName(casterName);
                shared_ptr<Character> target = characters.findByName(targetName);
                if (caster && target) {
                    //using dynamic_pointer_cast I get or this character in this type or nullptr
                    // use this I get type of character and in this type I change the HP of character(delete character)
                    // and get Spell, as all information in my code store on that level
                    shared_ptr<Wizard> wizard = dynamic_pointer_cast<Wizard>(caster);
                    if (wizard) {
                        shared_ptr<Spell> spellToWizard = wizard->getSpell(spellName);
                        if (spellToWizard != nullptr) {
                            if (spellToWizard->consistTarget(target)) {
                                shared_ptr<Fighter> targetFighter = dynamic_pointer_cast<Fighter>(target);
                                if (targetFighter) {
                                    fout << casterName << " casts " << spellName << " on " << targetName << "!" << endl;
                                    fout << targetName << " has died..." << endl;
                                    wizard->removeSpell(spellName);
                                    characters.erase(targetName); //delete character
                                } else {
                                    shared_ptr<Archer> targetArcher = dynamic_pointer_cast<Archer>(target);
                                    if (targetArcher) {
                                        fout << casterName << " casts " << spellName << " on " << targetName << "!"
                                             << endl;
                                        fout << targetName << " has died..." << endl;
                                        wizard->removeSpell(spellName);
                                        characters.erase(targetName);
                                    } else {
                                        shared_ptr<Wizard> targetWizard = dynamic_pointer_cast<Wizard>(target);
                                        if (targetWizard) {
                                            fout << casterName << " casts " << spellName << " on " << targetName << "!"
                                                 << endl;
                                            fout << targetName << " has died..." << endl;
                                            wizard->removeSpell(spellName);
                                            characters.erase(targetName);
                                        }
                                    }
                                }
                            } else {
                                fout << "Error caught" << endl;
                            }
                        } else {
                            fout << "Error caught" << endl;
                        }
                    } else {
                        shared_ptr<Archer> archer = dynamic_pointer_cast<Archer>(caster);
                        if (archer) {
                            shared_ptr<Spell> spellToArcher = archer->getSpell(spellName);
                            if (spellToArcher != nullptr) {
                                if (spellToArcher->consistTarget(target)) {
                                    shared_ptr<Fighter> targetFighter = dynamic_pointer_cast<Fighter>(target);
                                    if (targetFighter) {
                                        fout << casterName << " casts " << spellName << " on " << targetName << "!"
                                             << endl;
                                        fout << targetName << " has died..." << endl;
                                        archer->removeSpell(spellName);
                                        characters.erase(targetName);
                                    } else {
                                        shared_ptr<Archer> targetArcher = dynamic_pointer_cast<Archer>(target);
                                        if (targetArcher) {
                                            fout << casterName << " casts " << spellName << " on " << targetName << "!"
                                                 << endl;
                                            fout << targetName << " has died..." << endl;
                                            archer->removeSpell(spellName);
                                            characters.erase(targetName);
                                        } else {
                                            shared_ptr<Wizard> targetWizard = dynamic_pointer_cast<Wizard>(target);
                                            if (targetWizard) {
                                                fout << casterName << " casts " << spellName << " on " << targetName
                                                     << "!"
                                                     << endl;
                                                fout << targetName << " has died..." << endl;
                                                archer->removeSpell(spellName);
                                                characters.erase(targetName);
                                            } else {
                                                fout << "Error caught" << endl;
                                            }
                                        }
                                    }
                                } else {
                                    fout << "Error caught" << endl;
                                }
                            } else {
                                fout << "Error caught" << endl;
                            }
                        } else {
                            fout << "Error caught" << endl;
                        }
                    }
                } else {
                    fout << "Error caught" << endl;
                }
            }
        } catch (exception e){
            fout << "Error caught" << endl;
        }
        try {
            //situation of Drink
            if (words[0] == "Drink") {
                string supplierName = words[1];
                string drinkerName = words[2];
                string potionName = words[3];
                shared_ptr<Character> supplier = characters.findByName(supplierName);
                shared_ptr<Character> drinker = characters.findByName(drinkerName);
                if (supplier && drinker) {
                    //using dynamic_pointer_cast I get or this character in this type or nullptr
                    // use this I get type of character and in this type I change the HP of character
                    // and get Potion, as all information in my code store on that level
                    shared_ptr<Fighter> fighter = dynamic_pointer_cast<Fighter>(drinker);
                    if (fighter) {
                        shared_ptr<Fighter> supplierFighter = dynamic_pointer_cast<Fighter>(supplier);
                        if (supplierFighter) {
                            shared_ptr<Potion> potionToFighter = supplierFighter->getPotion(potionName);
                            if (potionToFighter != nullptr) {
                                fighter->takeDamage(-potionToFighter->getHealValue());
                                fout << drinkerName << " drinks " << potionName << " from " << supplierName << "."
                                     << endl;
                                supplierFighter->removePotion(potionName);
                            } else {
                                fout << "Error caught" << endl;
                                continue;
                            }
                        } else {
                            shared_ptr<Archer> supplierArcher = dynamic_pointer_cast<Archer>(supplier);
                            if (supplierArcher) {
                                shared_ptr<Potion> potionToArcher = supplierArcher->getPotion(potionName);
                                if (potionToArcher != nullptr) {
                                    fighter->takeDamage(-potionToArcher->getHealValue());
                                    fout << drinkerName << " drinks " << potionName << " from " << supplierName << "."
                                         << endl;
                                    supplierArcher->removePotion(potionName);
                                } else {
                                    fout << "Error caught" << endl;
                                    continue;
                                }
                            } else {
                                shared_ptr<Wizard> supplierWizard = dynamic_pointer_cast<Wizard>(supplier);
                                if (supplierWizard) {
                                    shared_ptr<Potion> potionToWizard = supplierWizard->getPotion(potionName);
                                    if (potionToWizard != nullptr) {
                                        fighter->takeDamage(-potionToWizard->getHealValue());
                                        fout << drinkerName << " drinks " << potionName << " from " << supplierName
                                             << "."
                                             << endl;
                                        supplierWizard->removePotion(potionName);
                                    } else {
                                        fout << "Error caught" << endl;
                                        continue;
                                    }
                                } else {
                                    fout << "Error caught" << endl;
                                }
                            }
                        }
                    } else {
                        shared_ptr<Archer> archer = dynamic_pointer_cast<Archer>(drinker);
                        if (archer) {
                            shared_ptr<Fighter> supplierFighter = dynamic_pointer_cast<Fighter>(supplier);
                            if (supplierFighter) {
                                shared_ptr<Potion> potionToFighter = supplierFighter->getPotion(potionName);
                                if (potionToFighter != nullptr) {
                                    archer->takeDamage(-potionToFighter->getHealValue());
                                    fout << drinkerName << " drinks " << potionName << " from " << supplierName << "."
                                         << endl;
                                    supplierFighter->removePotion(potionName);
                                } else {
                                    fout << "Error caught" << endl;
                                    continue;
                                }
                            } else {
                                shared_ptr<Archer> supplierArcher = dynamic_pointer_cast<Archer>(supplier);
                                if (supplierArcher) {
                                    shared_ptr<Potion> potionToArcher = supplierArcher->getPotion(potionName);
                                    if (potionToArcher != nullptr) {
                                        archer->takeDamage(-potionToArcher->getHealValue());
                                        fout << drinkerName << " drinks " << potionName << " from " << supplierName
                                             << "."
                                             << endl;
                                        supplierArcher->removePotion(potionName);
                                    } else {
                                        fout << "Error caught" << endl;
                                        continue;
                                    }
                                } else {
                                    shared_ptr<Wizard> supplierWizard = dynamic_pointer_cast<Wizard>(supplier);
                                    if (supplierWizard) {
                                        shared_ptr<Potion> potionToWizard = supplierWizard->getPotion(potionName);
                                        if (potionToWizard != nullptr) {
                                            archer->takeDamage(-potionToWizard->getHealValue());
                                            fout << drinkerName << " drinks " << potionName << " from " << supplierName
                                                 << "."
                                                 << endl;
                                            supplierWizard->removePotion(potionName);
                                        } else {
                                            fout << "Error caught" << endl;
                                            continue;
                                        }
                                    } else {
                                        fout << "Error caught" << endl;
                                    }
                                }
                            }
                        } else {
                            shared_ptr<Wizard> wizard = dynamic_pointer_cast<Wizard>(drinker);
                            if (wizard) {
                                shared_ptr<Fighter> supplierFighter = dynamic_pointer_cast<Fighter>(supplier);
                                if (supplierFighter) {
                                    shared_ptr<Potion> potionToFighter = supplierFighter->getPotion(potionName);
                                    if (potionToFighter != nullptr) {
                                        wizard->takeDamage(-potionToFighter->getHealValue());
                                        fout << drinkerName << " drinks " << potionName << " from " << supplierName
                                             << "."
                                             << endl;
                                        supplierFighter->removePotion(potionName);
                                    } else {
                                        fout << "Error caught" << endl;
                                        continue;
                                    }
                                } else {
                                    shared_ptr<Archer> supplierArcher = dynamic_pointer_cast<Archer>(supplier);
                                    if (supplierArcher) {
                                        shared_ptr<Potion> potionToArcher = supplierArcher->getPotion(potionName);
                                        if (potionToArcher != nullptr) {
                                            wizard->takeDamage(-potionToArcher->getHealValue());
                                            fout << drinkerName << " drinks " << potionName << " from " << supplierName
                                                 << "."
                                                 << endl;
                                            supplierArcher->removePotion(potionName);
                                        } else {
                                            fout << "Error caught" << endl;
                                            continue;
                                        }
                                    } else {
                                        shared_ptr<Wizard> supplierWizard = dynamic_pointer_cast<Wizard>(supplier);
                                        if (supplierWizard) {
                                            shared_ptr<Potion> potionToWizard = supplierWizard->getPotion(potionName);
                                            if (potionToWizard != nullptr) {
                                                wizard->takeDamage(-potionToWizard->getHealValue());
                                                fout << drinkerName << " drinks " << potionName << " from "
                                                     << supplierName
                                                     << "."
                                                     << endl;
                                                supplierWizard->removePotion(potionName);
                                            } else {
                                                fout << "Error caught" << endl;
                                                continue;
                                            }
                                        } else {
                                            fout << "Error caught" << endl;
                                        }
                                    }
                                }
                            }
                        }
                    }
                } else {
                    fout << "Error caught" << endl;
                }
            }
        } catch (exception e){
            fout << "Error caught" << endl;
        }
        try {
            //situation of Dialogue
            if (words[0] == "Dialogue") {
                string speaker = words[1];
                if (speaker != "Narrator") {
                    //as I don't create Narrator as a character
                    shared_ptr<Character> character = characters.findByName(speaker);
                }
                int sp_len = stoi(words[2]);
                vector<string> speech;
                for (int i = 3; i < 3 + sp_len; ++i) {
                    speech.push_back(words[i]);
                }
                fout << speaker << ": ";
                //output speech
                for (int i = 0; i < sp_len; ++i) {
                    if (i == (sp_len - 1)) {
                        fout << speech[i];
                    } else {
                        fout << speech[i] + " ";
                    }
                }
                fout << endl;
            }
        } catch (exception e) {
            fout << "Error caught" << endl;
        }
        try {
            //situation show characters
            if (words[0] == "Show" && words[1] == "characters") {
                vector<shared_ptr<Character>> persons = characters.getCharacters(); //getting the vector of characters from map
                //sorting through the vector by name of character
                sort(persons.begin(), persons.end(), [](auto& a, auto& b) {
                    return a->getName() < b->getName();
                });
                //using dynamic_pointer_cast get the type of character or nullptr and output that characters
                for (int i = 0; i < persons.size(); ++i) {
                    shared_ptr<Fighter> fighter = dynamic_pointer_cast<Fighter>(persons[i]);
                    if (fighter) {
                        fout << persons[i]->getName() << ":" << "fighter:" << persons[i]->getHP() << " ";
                    } else {
                        shared_ptr<Archer> archer = dynamic_pointer_cast<Archer>(persons[i]);
                        if (archer) {
                            fout << persons[i]->getName() << ":" << "archer:" << persons[i]->getHP() << " ";
                        } else {
                            shared_ptr<Wizard> wizard = dynamic_pointer_cast<Wizard>(persons[i]);
                            if (wizard) {
                                fout << persons[i]->getName() << ":" << "wizard:" << persons[i]->getHP() << " ";
                            } else {
                                fout << "Error caught" << endl;
                            }
                        }
                    }
                }
                fout << endl;
            }
        } catch (exception e) {
            fout << "Error caught" << endl;
        }
        try {
            //situation of show weapons
            if (words[0] == "Show" && words[1] == "weapons") {
                string characterName = words[2];
                shared_ptr<Character> character = characters.findByName(characterName);

                //using dynamic_pointer_cast get the type of character or nullptr and output weapons of this character
                //only Fighter and Archer can have weapons
                shared_ptr<Fighter> fighter = dynamic_pointer_cast<Fighter>(character);
                if (fighter) {
                    vector<shared_ptr<Weapon>> weapons = fighter->showWeapons(); //get vector of weapons
                    //sorting through the vector by name of weapons
                    sort(weapons.begin(), weapons.end(), [](auto& a, auto& b) {
                        return a->getName() < b->getName();
                    });
                    for (int i = 0; i < weapons.size(); ++i) {
                        fout << weapons[i]->getName() << ":" << weapons[i]->getDamage() << " ";
                    }
                    fout << endl;
                } else {
                    shared_ptr<Archer> archer = dynamic_pointer_cast<Archer>(character);
                    if (archer) {
                        vector<shared_ptr<Weapon>> weapons = archer->showWeapons(); //get vector of weapons
                        //sorting through the vector by name of weapons
                        sort(weapons.begin(), weapons.end(), [](auto& a, auto& b) {
                            return a->getName() < b->getName();
                        });
                        for (int i = 0; i < weapons.size(); ++i) {
                            fout << weapons[i]->getName() << ":" << weapons[i]->getDamage() << " ";
                        }
                        fout << endl;
                    } else {
                        fout << "Error caught" << endl;
                    }
                }
            }
        } catch (exception e) {
            fout << "Error caught" << endl;
        }
        try {
            //situation of show potions
            if (words[0] == "Show" && words[1] == "potions") {
                string characterName = words[2];
                shared_ptr<Character> character = characters.findByName(characterName);

                //using dynamic_pointer_cast get the type of character or nullptr and output potions of this character
                //Fighter, Archer and Wizard can have potions
                shared_ptr<Fighter> fighter = dynamic_pointer_cast<Fighter>(character);
                if (fighter) {
                    vector<shared_ptr<Potion>> potions = fighter->showPotions(); //get vector of potions
                    //sorting through the vector by name of potions
                    sort(potions.begin(), potions.end(), [](auto& a, auto& b) {
                        return a->getName() < b->getName();
                    });
                    for (int i = 0; i < potions.size(); ++i) {
                        fout << potions[i]->getName() << ":" << potions[i]->getHealValue() << " ";
                    }
                    fout << endl;
                } else {
                    shared_ptr<Archer> archer = dynamic_pointer_cast<Archer>(character);
                    if (archer) {
                        vector<shared_ptr<Potion>> potions = archer->showPotions(); //get vector of potions
                        //sorting through the vector by name of potions
                        sort(potions.begin(), potions.end(), [](auto& a, auto& b) {
                            return a->getName() < b->getName();
                        });
                        for (int i = 0; i < potions.size(); ++i) {
                            fout << potions[i]->getName() << ":" << potions[i]->getHealValue() << " ";
                        }
                        fout << endl;
                    } else {
                        shared_ptr<Wizard> wizard = dynamic_pointer_cast<Wizard>(character);
                        if (wizard) {
                            vector<shared_ptr<Potion>> potions = wizard->showPotions(); //get vector of potions
                            //sorting through the vector by name of potions
                            sort(potions.begin(), potions.end(), [](auto& a, auto& b) {
                                return a->getName() < b->getName();
                            });
                            for (int i = 0; i < potions.size(); ++i) {
                                fout << potions[i]->getName() << ":" << potions[i]->getHealValue() << " ";
                            }
                            fout << endl;
                        } else {
                            fout << "Error caught" << endl;
                        }
                    }
                }
            }
        } catch (exception e) {
            fout << "Error caught" << endl;
        }
        try {
            //situation of show spells
            if (words[0] == "Show" && words[1] == "spells") {
                string characterName = words[2];
                shared_ptr<Character> character = characters.findByName(characterName);

                //using dynamic_pointer_cast get the type of character or nullptr and output spells of this character
                //only Wizard and Archer can have spells
                shared_ptr<Wizard> wizard = dynamic_pointer_cast<Wizard>(character);
                if (wizard) {
                    vector<shared_ptr<Spell>> spells = wizard->showSpells(); //get vector of spells
                    //sorting through the vector by name of spells
                    sort(spells.begin(), spells.end(), [](auto& a, auto& b) {
                        return a->getName() < b->getName();
                    });
                    for (int i = 0; i < spells.size(); ++i) {
                        fout << spells[i]->getName() << ":" << spells[i]->numAllowedTargets() << " ";
                    }
                    fout << endl;
                } else {
                    shared_ptr<Archer> archer = dynamic_pointer_cast<Archer>(character);
                    if (archer) {
                        vector<shared_ptr<Spell>> spells = archer->showSpells(); //get vector of spells
                        //sorting through the vector by name of spells
                        sort(spells.begin(), spells.end(), [](auto& a, auto& b) {
                            return a->getName() < b->getName();
                        });
                        for (int i = 0; i < spells.size(); ++i) {
                            fout << spells[i]->getName() << ":" << spells[i]->numAllowedTargets() << " ";
                        }
                        fout << endl;
                    } else {
                        fout << "Error caught" << endl;
                    }
                }
            }
        } catch (exception e) {
            fout << "Error caught" << endl;
        }
    }
    fin.close();
    fout.close();
}
