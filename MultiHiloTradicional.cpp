#include <iostream>
#include <fstream>
#include <string>
#include <thread>
#include <vector>
#include <Poco/DigestEngine.h>
#include <Poco/MD5Engine.h>
#include <chrono>
using namespace std;

const int NUM_THREADS = 2; 

bool hashMatch(const string &input, const string &hashToMatch)
{
    Poco::MD5Engine md5;
    md5.update(input);
    const string hash = Poco::DigestEngine::digestToHex(md5.digest());
    return hash == hashToMatch;
}

void findPasswordInRange(const string &hashToMatch, const string &filename, int startLine, int endLine, string &foundPassword)
{
    ifstream file(filename);
    if (!file.is_open())
    {
        cerr << "Error abriendo el archivo: " << filename << endl;
        return;
    }

    string line;
    int lineCounter = 0;
    while (getline(file, line))
    {
        if (lineCounter >= startLine && lineCounter <= endLine && hashMatch(line, hashToMatch))
        {
            foundPassword = line;
            break;
        }
        lineCounter++;
    }

    file.close();
}

int main()
{
    const string hashToMatch = "c5e5da67655b41ef4c34862a251367fe";
    const string filename = "rockyou.txt";

    cout << "Búsqueda en hilos..." << endl;
    auto start = std::chrono::high_resolution_clock::now();

    ifstream file(filename);
    if (!file.is_open())
    {
        cerr << "Error abriendo el archivo: " << filename << endl;
        return 1;
    }

    int totalLines = 0;
    string line;
    while (getline(file, line))
    {
        totalLines++;
    }

    vector<thread> threads;
    vector<string> passwords(NUM_THREADS);
    int linesPerThread = totalLines / NUM_THREADS;
    int remainingLines = totalLines % NUM_THREADS;
    int startLine = 0;

    for (int i = 0; i < NUM_THREADS; ++i)
    {
        int endLine = startLine + linesPerThread - 1;
        if (i == NUM_THREADS - 1)
        {
            endLine += remainingLines;
        }

        threads.emplace_back(findPasswordInRange, hashToMatch, filename, startLine, endLine, ref(passwords[i]));
        startLine = endLine + 1;
    }

    for (auto &thread : threads)
    {
        thread.join();
    }

    string foundPassword;
    for (const string &password : passwords)
    {
        if (!password.empty())
        {
            foundPassword = password;
            break;
        }
    }

    auto end = std::chrono::high_resolution_clock::now();
    if (!foundPassword.empty())
    {
        cout << "Contraseña encontrada: " << foundPassword << endl;
    }
    else
    {
        cout << "Contraseña no encontrada." << endl;
    }

    auto duration = end - start;
    cout << "Tiempo: " << std::chrono::duration<double, std::milli>(duration).count() << " milisegundos" << endl;

    return 0;
}
