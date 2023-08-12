#include <iostream>
#include <fstream>
#include <string>
#include <Poco/DigestEngine.h>
#include <Poco/MD5Engine.h>
#include <chrono>
using namespace std;
bool hashMatch(const string &input, const string &hashToMatch)
{
    Poco::MD5Engine md5;
    md5.update(input);
    const ::string hash = Poco::DigestEngine::digestToHex(md5.digest());
    return hash == hashToMatch;
}

string findPasswordSequential(const string &hashToMatch, const string &filename)
{
    ifstream file(filename);
    if (!file.is_open())
    {
        cerr << "Error abriendo el archivo: " << filename << endl;
        return "";
    }

    string line;
    while (getline(file, line))
    {
        if (hashMatch(line, hashToMatch))
        {
            file.close();
            return line;
        }
    }

    file.close();
    return "";
}

int main()
{
    const string hashToMatch = "c5e5da67655b41ef4c34862a251367fe";
    const string filename = "rockyou.txt";

    cout << "Busuqeda secuencial..." << endl;
    auto start = std::chrono::high_resolution_clock::now();
    string password = findPasswordSequential(hashToMatch, filename);
    auto end = std::chrono::high_resolution_clock::now();

    if (!password.empty())
    {
        cout << "Contraseña encontrada: " << password << endl;
    }
    else
    {
        cout << "Contraseña no encontrada." << endl;
    }

    auto duration = end - start;
    std::cout << "Tiempo: " << std::chrono::duration<double, std::milli>(duration).count() << " milisegundos" << std::endl;

    return 0;
}
