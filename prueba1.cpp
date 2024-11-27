#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <vector>
#include <iomanip> // Para std::fixed y std::setprecision
#include <cmath>   // Para std::round

struct Column {
    std::string name;
    std::string type;
    bool isCalculated = false; // Identifica si es una columna calculada (como 'total')
};

// Función para redondear a 2 decimales
double roundToTwoDecimals(double value) {
    return std::round(value * 100.0) / 100.0;
}

// Función para obtener el siguiente índice automáticamente
int getNextIndex(const std::string& filename) {
    std::ifstream file(filename);
    if (!file.is_open()) {
        return 1; // Archivo no existe o está vacío, comenzar desde 1
    }

    std::string line, lastLine;
    while (std::getline(file, line)) {
        lastLine = line;
    }
    file.close();

    if (lastLine.empty()) {
        return 1; // Archivo vacío, comenzar desde 1
    }

    // Extraer el índice desde la última línea
    std::stringstream ss(lastLine);
    std::string indexStr;
    std::getline(ss, indexStr, ','); // Leer el índice (primer campo)
    return std::stoi(indexStr) + 1;
}

// Función para anadir una nueva fila
void addRow(const std::vector<Column>& columns) {
    std::ofstream file("taxables.csv", std::ios::app);
    if (!file.is_open()) {
        std::cerr << "Error: No se pudo abrir el archivo 'taxables.csv'." << std::endl;
        return;
    }

    int nextIndex = getNextIndex("taxables.csv");
    std::string inputData;
    double cost = 0, tax = 0, total = 0;

    file << nextIndex << ","; // Escribir índice automáticamente

    for (size_t i = 1; i < columns.size(); ++i) { // Saltar el índice
        const auto& column = columns[i];

        if (column.isCalculated) {
            // Calcular 'total' como la suma de 'cost' y 'tax'
            if (column.name == "total") {
                total = roundToTwoDecimals(cost + tax);
                file << std::fixed << std::setprecision(2) << total;
            }
        } else {
            // Solicitar entrada al usuario para las demás columnas
            std::cout << "Ingrese el valor para '" << column.name << "' (" << column.type << "): ";
            std::cin >> inputData;

            if (column.name == "cost" || column.name == "tax") {
                // Convertir a double y redondear a 2 decimales
                double value = roundToTwoDecimals(std::stod(inputData));
                if (column.name == "cost") {
                    cost = value;
                } else if (column.name == "tax") {
                    tax = value;
                }
                file << std::fixed << std::setprecision(2) << value;
            } else {
                // Otros campos como 'item' se escriben directamente
                file << inputData;
            }
        }

        if (i != columns.size() - 1) {
            file << ","; // Anadir coma entre valores
        }
    }

    file << std::endl;
    file.close();
    std::cout << "Fila anadida correctamente al archivo 'taxables.csv'." << std::endl;
}

int main() {
    // Leer la estructura de la tabla desde struct_table.txt
    std::ifstream file("struct_table.txt");
    if (!file.is_open()) {
        std::cerr << "Error: No se pudo abrir el archivo 'struct_table.txt'." << std::endl;
        return 1;
    }

    std::string line;
    std::vector<Column> columns;

    while (std::getline(file, line)) {
        if (line.find("CREATE TABLE") != std::string::npos) {
            continue; // Saltar declaración de la tabla
        }

        if (line.find(");") != std::string::npos) {
            break; // Detenerse al final de la tabla
        }

        std::stringstream ss(line);
        std::string name, type;
        ss >> name >> type;

        Column column = {name, type};

        // Identificar columnas calculadas
        if (name == "total") {
            column.isCalculated = true;
        }

        columns.push_back(column);
    }

    file.close();

    // Mostrar estructura de la tabla
    std::cout << "Estructura de la tabla:" << std::endl;
    for (const auto& column : columns) {
        std::cout << "Columna: " << column.name << ", Tipo: " << column.type
                  << (column.isCalculated ? " (Calculada automaticamente)" : "") << std::endl;
    }

    // Menú para anadir filas
    int option;
    do {
        std::cout << "\nMenu:\n1. Anadir fila\n0. Salir\nSeleccione una opcion: ";
        std::cin >> option;

        if (option == 1) {
            addRow(columns);
        } else if (option != 0) {
            std::cout << "Opcion no válida." << std::endl;
        }
    } while (option != 0);

    return 0;
}
