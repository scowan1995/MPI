#include "mpi.h"
#include <algorithm>
#include <cstdlib>
#include <cctype>
#include <fstream>
#include <iostream>
#include <vector>
#include <sstream>
#include <iterator>
#include <assert.h>

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
std::string longestPalindromeDP(std::string s) {
    int n = s.length();
    int longestBegin = 0;
    int maxLen = 1;
    bool table[1000][1000] = {false};
    for (int i = 0; i < n; i++) {
        table[i][i] = true;
    }
    for (int i = 0; i < n-1; i++) {
        if (s[i] == s[i+1]) {
            table[i][i+1] = true;
            longestBegin = i;
            maxLen = 2;
        }
    }
    for (int len = 3; len <= n; len++) {
        for (int i = 0; i < n-len+1; i++) {
            int j = i+len-1;
            if (s[i] == s[j] && table[i+1][j-1]) {
                table[i][j] = true;
                longestBegin = i;
                maxLen = len;
            }
        }
    }
    return s.substr(longestBegin, maxLen);
}

std::string SearchFromCentre(std::string param){
    std::cout<<"starting search"<<std::endl;
    std::string str = "";
    std::stringstream ss(param);
    std::string line;
    int bestLen = 0;
    int bestStart = 0;
    int lineNumber = 0;
    std::string bestString = "";
    std::cout<<"for start"<<std::endl;
    for (int lines = 0; std::getline(ss, line, '\n'); lines++)
    {
        std::cout<<"loop line "<<line<<std::endl;
        for(int centre= 0 ; centre < line.length(); centre++)
        {//pick a centre to expand from
            std::cout<<"looping through "<<line<<" i = "<<centre<<"centre = "<<line[centre]<<std::endl;
            int i = centre -1;
            int j = centre +1;
            std::cout<<"sides left, right: "<< i<<" "<<j<<std::endl;
            bool stillPal = true;
            while (((i>=0)&&(j<line.length()))&&stillPal)
            {
                std::cout<<"huh";
                std::cout<<"Odd increment: "<<i<<" "<<j<< "with line length "<<line.length()<<std::endl;
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
            if ((centre < line.length()-1)&&(centre>=0)){
                std::cout<<"just 1"<<std::endl;
                if (line[centre]==line[centre+1]) {
                    i = centre - 1;
                    j = centre + 2;
                    stillPal = true;
                    while (i >= 0 && j < line.length() && stillPal) {
                        std::cout<<"Even increment: "<<i<<" "<<j<< "with line length "<<line.length()<<std::endl;
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
    std::cout<<"finishing search"<<std::endl;
    std::cout<<"pal"<< bestString << std::endl;

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
    std::cout<<"made it 1"<<std::endl;
    // Setup MPI
    MPI_Init( &argc, &argv );
    MPI_Comm_rank( MPI_COMM_WORLD, &processId);
    MPI_Comm_size( MPI_COMM_WORLD, &numberOfProcesses);
    std::cout<<"made it 2"<<std::endl;
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
    std::cout<<"made it 3"<<std::endl;
    // ....... Your SPMD program goes here ............
    // get all lines into an array if world rank = 0
    std::ifstream file(argv[1]);
    std::string line;
    Result results[numberOfProcesses];

    //create buffer to hold a subset of the lines, one for each process
    //std::vector<std::string> chunks;
    //this should split up the input into numProcs chunks of roughly equal length
    std::cout<<"made it 4"<<std::endl;
    std::string information = "";
    int lineCount = 0;
    for (int i = 0; i< numberOfProcesses; i++)
    {
        lineCount++;
        std::getline(file, line);
        if (line.length()<2) lineCount--;
        else
         information.append("\n"+line);
      //  chunks.push_back(line);
    }
    for (int i = 0; std::getline(file, line); i++)
    {
        lineCount++;
        if (line.length()<2) lineCount--;
        else
            information.append("\n"+line);
    //    chunks[i%numberOfProcesses].append(" "+line);  //the space is a delim used later
    }
    std::cout<<"made it 5"<<std::endl;
    char *send_data = (char *)malloc(sizeof(char) * information.length());  //so here we basically have a massive string
    strcpy(send_data, information.c_str());
    //Scatter the lines to each process in the world
    char * recv_data = (char *)malloc(sizeof(char) * information.length()/numberOfProcesses);
    assert(recv_data != NULL);
    //file.close();
    MPI_Scatter(send_data, information.length()/numberOfProcesses, MPI_CHAR, recv_data,
                information.length()/numberOfProcesses, MPI_CHAR, 0, MPI_COMM_WORLD);
    std::cout<<"Made it 5.1"<<std::endl;
    std::cout<<"get me anything: "<<recv_data[0]<<std::endl;
    std::cout<<"gotten the things"<<std::endl;
    char res = recv_data[0];
    std::cout<<std::endl;
    std::string str(recv_data);
  //  std::cout<<"block "<<str<<" block"<<std::endl;  //so this works and I can get a palindrome from it but how to gather
    //possibly just need to make a massive thing for holding strings

    //Find the largest palindrome
  //  std::string x(&recv_data[0]);//, std::end(recv_data) - std::begin(recv_data)
    std::string palindrome = SearchFromCentre(str);
    std::cout<<"at ;east out"<<std::endl;
    std::cout<<"Here is the Palindrome: "<<palindrome<<std::endl;
    char *gather_data = (char *)malloc(sizeof(char) * palindrome.length()+1);  //so here we basically have a massive string
    palindrome.append("\n");
    strcpy(gather_data, palindrome.c_str());
    //if worldrank = 0 create an array to hold all Paindromes
    //creating a string to append each palinrome on to
    char *gatherResults = NULL;
    if (processId==0){
        gatherResults=(char *)malloc(sizeof(char) * numberOfProcesses*1024); //made really big to hold large palindromes, not an elegant solution
    }

    MPI_Gather(gather_data, palindrome.length(), MPI_CHAR, gatherResults, palindrome.length(), MPI_CHAR, 0, MPI_COMM_WORLD);
    std::cout<<"gathered up"<<std::endl;
    //Gather the processes
    std::string bestString = "";
    if (processId==0) {
        for (int i = 0; i < numberOfProcesses; i++) {
            //if (gatherResults[i].length()>bestString.length()){
            //    bestString = gatherResults[i];
            //}
            std::cout << "We made it!!  " << gatherResults[i] << std::endl;
        }
    }
    std::cout<<"finished?"<<std::endl;
    //if world rank = 0 find largest Palindrome
    // ... Eventually..
  /*std::cout<<"made it 6"<<std::endl;
    if(processId == 0)
    {
        std::cout<<"made it 7"<<std::endl;
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
    */

    MPI_Finalize();

    return 0;
}
