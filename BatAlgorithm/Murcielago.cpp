#include <iostream>
#include <fstream>
#include <vector>
#include <cmath>
#include <cstdlib>
#include <ctime>
#include <map>
#include <chrono>
#include <iomanip>

using namespace std;

// Estructura para representar una torre
struct Torre {
    double x, y; // Coordenadas de la torre
};

// Parámetros del Algoritmo de Murciélagos
const int NUM_TORRES = 20;
const double RANGO_COBERTURA = 10.0;

// Área geográfica
const double AREA_X = 100.0;
const double AREA_Y = 100.0;

// Parámetros
const double AREA_CIRCULO = 3.141592 * RANGO_COBERTURA * RANGO_COBERTURA;

// Calcula la distancia euclidiana entre dos puntos
double distancia(const Torre& a, const Torre& b) {
    return sqrt((a.x - b.x) * (a.x - b.x) + (a.y - b.y) * (a.y - b.y));
}

// Calcula el área de intersección entre dos círculos
double areaInterseccion(const Torre& a, const Torre& b) {
    double d = distancia(a, b);
    if (d >= 2 * RANGO_COBERTURA) {
        return 0.0; // No hay intersección
    } else if (d <= 0.0) {
        return AREA_CIRCULO; // Círculos completamente solapados
    }

    double r = RANGO_COBERTURA;
    double part1 = r * r * acos((d * d + r * r - r * r) / (2 * d * r));
    double part2 = r * r * acos((d * d + r * r - r * r) / (2 * d * r));
    double part3 = 0.5 * sqrt((-d + r + r) * (d + r - r) * (d - r + r) * (d + r + r));
    return part1 + part2 - part3;
}

// Calcula la cobertura total única
double calcularCobertura(const vector<Torre>& torres) {
    double coberturaTotal = 0.0;

    for (size_t i = 0; i < torres.size(); ++i) {
        coberturaTotal += AREA_CIRCULO; // Cobertura total de la torre i

        // Restar intersecciones con las torres ya contadas
        for (size_t j = 0; j < i; ++j) {
            coberturaTotal -= areaInterseccion(torres[i], torres[j]);
        }
    }

    return -coberturaTotal;
}


// Genera una torre en una posición aleatoria
Torre generarTorreAleatoria() {
    return {rand() % (int)AREA_X, rand() % (int)AREA_Y};
}

// Algoritmo de Murciélagos
int batAlgorithm(double ALPHA, double GAMMA, int ITERACIONES) {
    srand(time(0));

    vector<Torre> torres(NUM_TORRES);
    for (auto& torre : torres) {
        torre = generarTorreAleatoria();
    }
    vector<Torre> mejorSolucion = torres;
    double mejorFitness = calcularCobertura(mejorSolucion);

    for (int i = 0; i < ITERACIONES; i++) {
        for (auto& torre : torres) {
            // Movimiento aleatorio de la torre
            torre.x += (rand() % 2 == 0 ? 1 : -1) * (rand() % (int)RANGO_COBERTURA);
            torre.y += (rand() % 2 == 0 ? 1 : -1) * (rand() % (int)RANGO_COBERTURA);

            // Validamos que la torre no salga del área
            torre.x = min(max(torre.x, 0.0), AREA_X);
            torre.y = min(max(torre.y, 0.0), AREA_Y);

            // Refinamos la posición con probabilidad decreciente
            if ((rand() / double(RAND_MAX)) < ALPHA) {
                torre.x += ((rand() / double(RAND_MAX)) - 0.5) * 2 * GAMMA;
                torre.y += ((rand() / double(RAND_MAX)) - 0.5) * 2 * GAMMA;
            }
        }

        // Evaluamos la nueva solución
        double fitnessActual = calcularCobertura(torres);
        if (fitnessActual > mejorFitness) {
            mejorFitness = fitnessActual;
            mejorSolucion = torres;
        }
    }

    // Imprimimos la mejor solución
    cout << "Mejor cobertura obtenida: " << -mejorFitness << endl;
    cout << "Posiciones de las torres:" << endl;
    for (const auto& torre : mejorSolucion) {
        cout << "Torre en (" << torre.x << ", " << torre.y << ")" << endl;
    }
    return -mejorFitness;
}

void imprimirDatos(map<int, int> veces, chrono::duration<double> duration, ofstream &file, double ALPHA, double GAMMA, int ITERACIONES){
    file << "Tiempo de ejecución: " << duration.count() << " segundos" << endl;
    file << "iteraciones: " << ITERACIONES << endl;
    file << "gamma: " << GAMMA << endl;
    file << "alpha: " << ALPHA << endl;
    file << setfill('=') << setw(50) << "=" << setfill(' ') << endl;
    for(pair<int, int> prob: veces){
        if(prob.first > 0)
            file << prob.first << ' ' << prob.second << endl;
    }
    file << setfill('=') << setw(50) << "=" << setfill(' ') << endl;
}

int main() {
    ofstream file("Resultados.txt", ios::out);
    file << setprecision(2) << fixed;
    // Captura el tiempo de inicio
    double ALPHA[3] = {0.7,0.8,0.9};
    double GAMMA = 0.7;
    int ITERACIONES[3] = {10000, 50000, 100000};
    for(int i = 0; i < 3; i++){
        for (int j = 0; j < 3; j++){
                map<int, int> veces;
                auto start = chrono::high_resolution_clock::now();
                for(int l= 0; l < 100; l++){
                    int value = batAlgorithm(ALPHA[j], GAMMA, ITERACIONES[i]);
                    if(veces[value] == 0){
                        veces[value] = 1;
                    }else{
                        veces[value]++;
                    }
                }
                
                auto end = chrono::high_resolution_clock::now();
                chrono::duration<double> duration = end - start;
                imprimirDatos(veces, duration, file, ALPHA[j], GAMMA, ITERACIONES[i]);
            
        }
        
    }
    return 0;
}