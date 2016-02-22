#include "mpi.h"
#include <algorithm>
#include <cstdlib>
#include <cctype>
#include <fstream>
#include <iostream>
#include <vector>
#include <sstream>
#include <iterator>

// Don't CHANGE This Code (you can add more functions)-----------------------------------------------------------------------------

struct Result
{
    Result()
            : lineNumber(0), firstChar(0), length(0)
    {}

    Result(int lineNumber_, int firstChar_, int length_)
            : lineNumber(lineNumber_), firstChar(firstChar_), length(length_)
    {}

    // This allows you to compare results with the < (less then) operator, i.e. r1 < r2
    bool
    operator<(Result const& o)
    {
        // Line number can't be equal
        return length < o.length ||
               (length == o.length && lineNumber >  o.lineNumber) ||
               (length == o.length && lineNumber == o.lineNumber  && firstChar > o.firstChar);
    }

    int lineNumber, firstChar, length;
};

void
DoOutput(Result r)
{
    std::cout << "Result: " << r.lineNumber << " " << r.firstChar << " " << r.length << std::endl;
}

// CHANGE This Code (you can add more functions)-----------------------------------------------------------------------------
std::string SearchFromCentre(std::string &param){
    std::string str = "";
    std::stringstream ss(param);
    std::string line;
    int bestLen = 0;
    int bestStart = 0;
    int lineNumber = 0;
    std::string bestString = "";
    for (int lines = 0; std::getline(ss, line, '\n'); lines++)
    {
        for(int centre= 0 ; centre < line.length(); centre++)
        {//pick a centre to expand from
            int i = centre -1;
            int j = centre +1;
            bool stillPal = true;
            while ((i>=0&&j<line.length())&&stillPal)
            {
                if (line[i]==line[j])
                {
                    i--;
                    j++;
                }
                else if (j-i>bestLen)
                {
                    lineNumber = lines;
                    bestStart = i;
                    bestLen = j-i;
                    stillPal = false;
                    bestString = line.substr(i, (bestLen-i));
                }
                else stillPal = false;
            }
            if (stillPal)
            {
                int x = (j<line.length()) ? j-i: line.length()-i;
                int y = (i>0) ? i : 0;
                if (x-y>bestLen) {
                    lineNumber = lines;
                    bestStart = (i > 0) ? i : 0;
                    bestLen = (j < line.length()) ? j - i : line.length() - i;
                    bestString = line.substr(i, (bestLen-i));
                    stillPal = false;
                }
            }
            //Searchs for an even length palindrome
            if (centre< line.length()-1){
                if (line[centre]==line[centre+1]) {
                    i = centre - 1;
                    j = centre + 2;
                    stillPal = true;
                    while (i >= 0 && j < line.length() && stillPal) {
                        if (line[i] == line[j]) {
                            i--;
                            j++;
                        }
                        else if (j - i > bestLen) {
                            lineNumber = lines;
                            bestStart = i;
                            bestLen = j - i;
                            stillPal = false;
                            bestString = line.substr(i, (bestLen-i));
                        }
                        else stillPal = false;

                    }
                    if (stillPal) {
                        int x = (j < line.length()) ? j - i : line.length() - i;
                        int y = (i > 0) ? i : 0;
                        if (x - y > bestLen) {
                            lineNumber = lines;
                            bestStart = (i > 0) ? i : 0;
                            bestLen = x - y;
                            stillPal = false;
                            bestString = line.substr(i, (bestLen-i));
                        }
                    }
                }
            }
        }
    }
    std::cout<< bestString << std::endl;
    /*Result res = {0,0,0};
    res.lineNumber = lineNumber;
    res.firstChar = bestStart+1;//+1
    res.length = bestLen-1;//-1*/
    //std::cout << "\nres in func len, lineNum, start: "<< res.length<< " "<< res.lineNumber<< "  "<< res.firstChar<<std::endl;
    return bestString;
}


int
main(int argc, char* argv[])
{
    int processId;
    int numberOfProcesses;

    // Setup MPI
    MPI_Init( &argc, &argv );
    MPI_Comm_rank( MPI_COMM_WORLD, &processId);
    MPI_Comm_size( MPI_COMM_WORLD, &numberOfProcesses);

    // Two arguments, the program name and the input file. The second should be the input file
    if(argc != 2)
    {
        if(processId == 0)
        {
            std::cout << "ERROR: Incorrect number of arguments. Format is: <filename>" << std::endl;
        }
        MPI_Finalize();
        return 0;
    }

    // ....... Your SPMD program goes here ............
    // get all lines into an array if world rank = 0
    std::ifstream file(argv[1]);
    std::string line;
    Result results[numberOfProcesses];

    //create buffer to hold a subset of the lines, one for each process
    //std::vector<std::string> chunks;
    //this should split up the input into numProcs chunks of roughly equal length
    std::string information = "";
    int lineCount = 0;
    for (int i = 0; i< numberOfProcesses; i++)
    {
        lineCount++;
        std::getline(file, line);
        if (line.length()<1) lineCount--;
        else
         information.append(" "+line);
      //  chunks.push_back(line);
    }
    for (int i = 0; std::getline(file, line); i++)
    {
        lineCount++;
        if (line.length()<1) lineCount--;
        else
            information.append(" "+line);
    //    chunks[i%numberOfProcesses].append(" "+line);  //the space is a delim used later
    }
    std::string *send_data = &information;  //so here we basically have a 2d array of chars
    //Scatter the lines to each process in the world
    char recv_data[information.length()/numberOfProcesses];
    MPI_Scatter(send_data, information.length()/numberOfProcesses, MPI_CHAR, recv_data,
                information.length()/numberOfProcesses, MPI_CHAR, 0, MPI_COMM_WORLD);
    file.close();

    //Find the largest palindrome
    std::string x(&recv_data[0]);//, std::end(recv_data) - std::begin(recv_data)
    std::string res = SearchFromCentre(x);
    //if worldrank = 0 create an array to hold all Paindromes
    std::string *gatherResults = NULL;
    if (processId==0){
        gatherResults[numberOfProcesses];
    }

    MPI_Gather(res,res.length(), MPI_CHAR, gatherResults,res.length(), MPI_Char, 0, MPI_COMM_WORLD);
    //Gather the processes
    std::string bestString = "";
    for (int i = 0; i < numberOfProcesses; i++){
        if (gatherResults[i].length()>bestString.length()){
            bestString = gatherResults[i];
        }
    }

    //if world rank = 0 find largest Palindrome
    // ... Eventually..
    if(processId == 0)
    {
        std::ifstream searchfile(argv[1]);
        std::string searchStr;
        int lineCount = -1;
        int firstChar = 0;
        while (std::getline(searchfile, searchStr)){
            lineCount++;
            if (searchStr.find(bestString) != std::string::npos)
                firstChar = searchStr.find(bestString);
        }
        Result finalResult =  {0,0,0};
        finalResult.lineNumber = lineCount;
        finalResult.firstChar = firstChar;
        finalResult.length = bestString.length();
        DoOutput(finalResult);
    }

    MPI_Finalize();

    return 0;
}
