#include <iostream>
#include <fstream>
#include <sstream>
#include <bitset>
#include <string>
#include <vector>
#include <filesystem>
#include <TFile.h>
#include <TTree.h>

namespace fs = std::filesystem;


int main() {
    //función para convertir valores hex a binario 
    auto hexToBin = [](const std::string& hexValue, int line_number) {
        try {
            
            std::bitset<8> bin(std::stoi(hexValue, nullptr, 16));
            return bin.to_string();
        } catch (const std::invalid_argument& e) {
            std::cerr << "Error en la línea " << line_number << ": La cadena '" << hexValue 
                      << "' no es un valor hexadecimal válido." << std::endl;
            return std::string(8, '0');  //cadena de 8 ceros en caso de error
        } catch (const std::out_of_range& e) {
            std::cerr << "Error en la línea " << line_number << ": El valor hexadecimal '" << hexValue 
                      << "' está fuera del rango permitido." << std::endl;
            return std::string(8, '0');  //cadena de 8 ceros en caso de error
        }
    };

   
    auto positionsToString = [](const std::vector<int>& positions) {
        if (positions.empty()) return std::string("(0)");
        std::string result = "(" + std::to_string(positions[0]);
        for (size_t i = 1; i < positions.size(); ++i) {
            result += "," + std::to_string(positions[i]);
        }
        result += ")";
        return result;
    };

    //binario a lista de posiciones activadas
    auto binToPositions = [](const std::string& binValue) {
        std::vector<int> positions;
        for (int i = 0; i < binValue.size(); ++i) {
            if (binValue[binValue.size() - 1 - i] == '1') {
                positions.push_back(i + 1);
            }
        }
        return positions;
    };

    std::string directory = "./";  // Directorio
    std::vector<std::string> fechas_procesadas;

    //itero sobre los archivos en el directorio
    for (const auto& entry : fs::directory_iterator(directory)) {
        std::string path = entry.path().string();
       
        // busscar archivos mate-m101, mate-m102, mate-m103
        if (path.find("mate-m101.txt") != std::string::npos) {
            // Extraer la fecha del nombre del archivo
            std::string fecha = path.substr(0, path.find("mate-m101.txt"));
           
            //revisar si ya procesamos esta fecha
            if (std::find(fechas_procesadas.begin(), fechas_procesadas.end(), fecha) != fechas_procesadas.end()) {
                continue;  // Ya procesamos esta fecha
            }

            //archivos correspondientes a esa fecha
            std::ifstream infile_m101(fecha + "mate-m101.txt");
            std::ifstream infile_m102(fecha + "mate-m102.txt");
            std::ifstream infile_m103(fecha + "mate-m103.txt");
            std::ofstream outfile(fecha + "combined_output.txt");

            if (!infile_m101.is_open() || !infile_m102.is_open() || !infile_m103.is_open()) {
                std::cerr << "Error abriendo uno de los archivos para la fecha " << fecha << "." << std::endl;
                return 1;
            }

            //.root
            TFile *rootFile = new TFile((fecha + "output.root").c_str(), "RECREATE");
            TTree *tree = new TTree("tree", "Tree con datos procesados");

            //defino variables que se guardarán en el TTree
            int ts, nA1, nB1, nA2, nB2, nA3, nB3, ts2, evt;
            std::vector<int> A1, B1, A2, B2, A3, B3;

            //ramas del tree
            tree->Branch("ts", &ts, "ts/I");
            tree->Branch("A1", &A1);
            tree->Branch("B1", &B1);
            tree->Branch("A2", &A2);
            tree->Branch("B2", &B2);
            tree->Branch("A3", &A3);
            tree->Branch("B3", &B3);
            tree->Branch("nA1", &nA1, "nA1/I");
            tree->Branch("nB1", &nB1, "nB1/I");
            tree->Branch("nA2", &nA2, "nA2/I");
            tree->Branch("nB2", &nB2, "nB2/I");
            tree->Branch("nA3", &nA3, "nA3/I");
            tree->Branch("nB3", &nB3, "nB3/I");
            tree->Branch("ts2", &ts2, "ts2/I");
            tree->Branch("evt", &evt, "evt/I");

            //primer y último valor de event para cada archivo
            std::string line_m101, line_m102, line_m103;
            std::string temp;
            
            

            

            
            
            //para leer líneas
            while (std::getline(infile_m101, line_m101) && std::getline(infile_m102, line_m102) && std::getline(infile_m103, line_m103)) {
                std::stringstream ss_m101(line_m101), ss_m102(line_m102), ss_m103(line_m103);

                std::string timestamp_m101, BB_m101, BA_m101, AA_m101, timestamp2_m101, event_m101;
                std::string BB_m102, BA_m102, AA_m102;
                std::string BB_m103, BA_m103, AA_m103;

                //líneas m101
                std::getline(ss_m101, timestamp_m101, ',');
                std::getline(ss_m101, BB_m101, ',');
                std::getline(ss_m101, BA_m101, ',');
                std::getline(ss_m101, AA_m101, ',');
                std::getline(ss_m101, timestamp2_m101, ',');
                std::getline(ss_m101, event_m101, ',');

                //líneas de m102
                std::getline(ss_m102, temp, ',');  // timestamp2_m102, no se usa
                std::getline(ss_m102, BB_m102, ',');
                std::getline(ss_m102, BA_m102, ',');
                std::getline(ss_m102, AA_m102, ',');

                //leer lineas de m103
                std::getline(ss_m103, temp, ',');  // timestamp2_m103, no se usa
                std::getline(ss_m103, BB_m103, ',');
                std::getline(ss_m103, BA_m103, ',');
                std::getline(ss_m103, AA_m103, ',');

                //hexxadecimal a bin
                int line_number = __LINE__;
                std::string BB_m101_bin = hexToBin(BB_m101, line_number);
                std::string BA_m101_bin =hexToBin(BA_m101, line_number);
                std::string AA_m101_bin = hexToBin(AA_m101, line_number);

                std::string BB_m102_bin =hexToBin(BB_m102, line_number);
                std::string BA_m102_bin = hexToBin(BA_m102, line_number);
                std::string AA_m102_bin = hexToBin(AA_m102, line_number);

                std::string BB_m103_bin = hexToBin(BB_m103, line_number);
                std::string BA_m103_bin = hexToBin(BA_m103, line_number);
                std::string AA_m103_bin = hexToBin(AA_m103, line_number);

                //Concatenar B y A para m101
                std::string B_m101 = BB_m101_bin + BA_m101_bin.substr(0, 4);  //los primeros 4 bits de BA_m101
                std::string A_m101 = BA_m101_bin.substr(4, 4) + AA_m101_bin;  //últimos 4 bits de BA_m101

                //concatenar B y A para m102
                std::string B_m102 = BB_m102_bin + BA_m102_bin.substr(0, 4);  //primeros 4 bits de BA_m102
                std::string A_m102 = BA_m102_bin.substr(4, 4) + AA_m102_bin;  // últimos 4 bits de BA_m102

                // concatenar B y A para m103
                std::string B_m103 = BB_m103_bin + BA_m103_bin.substr(0, 4);  // Los primeros 4 bits de BA_m103
                std::string A_m103 = BA_m103_bin.substr(4, 4) + AA_m103_bin;  // Los últimos 4 bits de BA_m103

                //convertir B y A de binario a lista de posiciones activadas
                std::vector<int> positions_B_m101 = binToPositions(B_m101);
                std::vector<int> positions_A_m101 = binToPositions(A_m101);

                std::vector<int> positions_B_m102 = binToPositions(B_m102);
                std::vector<int> positions_A_m102 = binToPositions(A_m102);

                std::vector<int> positions_B_m103 = binToPositions(B_m103);
                std::vector<int> positions_A_m103 = binToPositions(A_m103);

                //posiciones activadas a cadenas
                std::string B_m101_str = positionsToString(positions_B_m101);
                std::string A_m101_str = positionsToString(positions_A_m101);
                std::string B_m102_str = positionsToString(positions_B_m102);
                std::string A_m102_str = positionsToString(positions_A_m102);
                std::string B_m103_str = positionsToString(positions_B_m103);
                std::string A_m103_str = positionsToString(positions_A_m103);

                //aquí calculo el número de bits activos nAi y nBi
                nA1 = (positions_A_m101.empty() || positions_A_m101[0] == 0) ? 0 : positions_A_m101.size();
                nB1 = (positions_B_m101.empty() || positions_B_m101[0] == 0) ? 0 : positions_B_m101.size();
                nA2 = (positions_A_m102.empty() || positions_A_m102[0] == 0) ? 0 : positions_A_m102.size();
                nB2 = (positions_B_m102.empty() || positions_B_m102[0] == 0) ? 0 : positions_B_m102.size();
                nA3 = (positions_A_m103.empty() || positions_A_m103[0] == 0) ? 0 : positions_A_m103.size();
                nB3 = (positions_B_m103.empty() || positions_B_m103[0] == 0) ? 0 : positions_B_m103.size();

                //salida
                outfile << timestamp_m101 << ","
                        << B_m101_str << "," << A_m101_str << "," << B_m102_str << "," << A_m102_str << ","
                        << B_m103_str << "," << A_m103_str << "," << event_m101 << "," << timestamp2_m101 << ","
                        << nB1 << "," << nA1 << "," << nB2 << "," << nA2 << "," << nB3 << "," << nA3 << "," << "\n";

                //variables del TTree
                ts = std::stoi(timestamp_m101);
                evt = std::stoi(event_m101);
                ts2 = std::stoll(timestamp2_m101);
                A1 = positions_A_m101;
                B1 = positions_B_m101;
                A2 = positions_A_m102;
                B2 = positions_B_m102;
                A3 = positions_A_m103;
                B3 = positions_B_m103;

                //llenar tree
                tree->Fill();
            }

            // Cerrar archivos
            infile_m101.close();
            infile_m102.close();
            infile_m103.close();
            outfile.close();

            //guardo tree como .root
            rootFile->Write();
            rootFile->Close();

            std::cout << "Procesamiento completo para la fecha " << fecha << ". Archivo guardado en " << fecha + "combined_output.txt" << " y " << fecha + "output.root" << std::endl;
           
            //añadir fecha a las procesadas
            fechas_procesadas.push_back(fecha);
        }
    }

    return 0;
}

