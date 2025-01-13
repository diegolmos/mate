#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>
#include <map>
#include <string>
#include <filesystem>
#include <cmath>
#include <algorithm>

namespace fs = std::filesystem;

std::vector<std::string> get_files(const std::string& folder_path, const std::string& extension, const std::string& keyword) {
    std::vector<std::string> files;
    for (const auto& entry : fs::directory_iterator(folder_path)) {
        if (entry.is_regular_file() && entry.path().extension() == extension && entry.path().filename().string().find(keyword) != std::string::npos) {
            files.push_back(entry.path().filename().string());
        }
    }
    return files;
}

std::vector<std::string> read_file(const std::string& file_path) {
    std::vector<std::string> data;
    std::ifstream file(file_path);
    std::string line;
    
    while (std::getline(file, line)) {
        data.push_back(line);
    }
    return data;
}

void process_files(const std::vector<std::string>& files, const std::string& folder_path) {
    std::map<std::string, std::vector<std::string>> file_groups;
    
    //ARCHIVOS POR FECHA
    for (const auto& file : files) {
        std::string date = file.substr(0, file.find("_", file.find("_") + 1));
        file_groups[date].push_back(file);
    }
    
    for (const auto& [date, group] : file_groups) {
        std::cout << "\nVerificando archivos para la fecha: " << date << std::endl;
        
        std::map<std::string, int> num_rows;
        std::map<std::string, int> events;
        
        for (const auto& file : group) {
            std::string file_path = folder_path + "/" + file;
            std::vector<std::string> data = read_file(file_path);
            
            std::vector<int> col6;
            for (const auto& line : data) {
                std::stringstream ss(line);
                std::string value;
                int col_count = 0;
                while (std::getline(ss, value, ',')) {
                    if (col_count == 5) {
                        col6.push_back(std::stoi(value));
                        break;
                    }
                    col_count++;
                }
            }
            
            //VERIFICACIÓN SI LOS ARCHIVOS SON CONSECUTIVOSS
            bool non_consecutive_found = false;
            for (size_t i = 1; i < col6.size(); ++i) {
                if (col6[i] != col6[i - 1] + 1) {
                    non_consecutive_found = true;
                    std::cout << "  -> Error: Eventos no consecutivos en " << file << std::endl;
                    std::cout << "     * Evento no consecutivo en la linea: " << i + 1 << ", Valor: " << col6[i] << std::endl;
                }
            }
            if (!non_consecutive_found) {
                std::cout << "  -> OK: Eventos consecutivos en " << file << std::endl;
            }
            
            num_rows[file] = data.size();
            events[file] = col6.back();
        }
        
        std::cout << "  -> Verificando número de lineas y último evento entre archivos..." << std::endl;
        for (const auto& [file, count] : num_rows) {
            std::cout << "     * Archivo: " << file << ", Número de lineas: " << count << ", Último evento: " << events[file] << std::endl;
        }
        
        if (num_rows.size() > 1 && std::all_of(num_rows.begin(), num_rows.end(), [&](const auto& pair) {
                return pair.second == num_rows.begin()->second;
            })) {
            std::cout << "  -> OK: Todos los archivos tienen el mismo número de lineas" << std::endl;
        } else {
            std::cout << "  -> Error: Diferente número de lineas entre los archivos" << std::endl;
        }
    }
}

int main() {
    std::string folder_path = "./"; //RUTA
    std::string extension = ".txt";
    std::string keyword = "mate-m10";
    
    std::vector<std::string> files = get_files(folder_path, extension, keyword);
    
    process_files(files, folder_path);
    
    return 0;
}

