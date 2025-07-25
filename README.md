
# Meta Toolkit (C++17)

Ce projet est une petite bibliothèque header-only réutilisable en **C++17**, conçue pour simplifier les tâches de métaprogrammation courantes comme :

- L’introspection de structures (`tie`)
- La comparaison récursive de types complexes
- Le dispatch compile-time (`TypeSwitch`)
- Les itérations compile-time (`StaticForEach`)
- L’indexation typée (`IndexOf`)
- La détection de membres (`hasTie`)

---

## 🔧 Fichiers

### `meta_toolkit.hpp`
Contient tous les outils suivants :
- `TypeList<Ts...>` : liste de types
- `TypeTag<T>` : wrapper de type pour dispatch
- `hasTie<T>` : détection automatique de la fonction `T::tie()`
- `getTie()` : retourne `tie()` si disponible, sinon `std::tie()`
- `StaticForEach<TypeList<...>>(lambda)` : exécute un lambda pour chaque type
- `TypeSwitch(index, lambda)` : dispatch sur type via index
- `compareValue(a, b)` : comparaison récursive de deux objets introspectables
- `structEqual(a, b)` : comparaison simple via `tie() == tie()`
- `IndexOf<T, TypeList<...>>` : renvoie l’indice d’un type dans une TypeList

### `main.cpp`
Exemple minimal d’utilisation avec deux structures (`Vec3`, `Transform`) utilisant `tie()` pour l’introspection, et comparées via `compareValue`.

---

## 🚀 Compilation

```bash
g++ -std=c++17 main.cpp -o test
./test
```

---

## 🧩 Exemple

```cpp
struct Vec3 {
    float x, y, z;
    static auto tie(auto& self) { return std::tie(self.x, self.y, self.z); }
};
```

```cpp
Transform a{{1,2,3}, {0,0,0}, {1,1,1}};
Transform b = a;
compareValue(a, b); // true
```

---

## 🧠 Détails techniques

### 🔍 Introspection avec `tie()`

Chaque struct introspectable doit exposer une méthode statique `tie()` :

```cpp
struct MyStruct {
    int x;
    float y;
    static auto tie(auto& self) {
        return std::tie(self.x, self.y);
    }
};
```

Cela permet de manipuler les champs via `std::apply`, sans avoir besoin de macros ou de réflexions externes.

---

### 🔁 StaticForEach : boucle sur TypeList à la compilation

```cpp
using MyTypes = TypeList<int, float, double>;

StaticForEach<MyTypes>([](auto tag) {
    using T = typename decltype(tag)::type;
    std::cout << "Type: " << typeid(T).name() << std::endl;
});
```

---

### 🎯 TypeSwitch : dispatch selon l’index d’un type

```cpp
using MyTypes = TypeList<int, float, double>;
int typeIndex = 1;

TypeSwitch<MyTypes>(typeIndex, [](auto tag) {
    using T = typename decltype(tag)::type;
    T value{};
    std::cout << "Switched on type: " << typeid(T).name() << std::endl;
});
```

---

### 🔗 Comparaison récursive

La fonction `compareValue(a, b)` utilise SFINAE pour détecter si les types ont un `tie()` :
- Si oui → appelle `recursiveCompare()` champ par champ
- Sinon → applique `operator==`

Elle supporte les structures imbriquées automatiquement.

```cpp
bool equal = compareValue(myA, myB); // Supporte les sous-objets récursifs
```

---
