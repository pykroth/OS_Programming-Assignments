//CODE WAS inspired from prof Rincon Lectures/Boiler
#include <algorithm>
#include <cmath>
#include <iomanip>
#include <iostream>
#include <netdb.h>
#include <netinet/in.h>
#include <pthread.h>
#include <queue>
#include <sstream>
#include <string>
#include <sys/socket.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>
#include <vector>
#include <strings.h>
struct Task
{
 std::string name;
 int WCET;
 int period;
 int initialWCET;
bool operator<(const Task& other) const{
 return(period <other.period) || (name < other.name && period == other.period);
}
 Task(const std::string& _name, int _wcet, int _period, int _initialWCET)
 {
 name = _name;
 WCET = _wcet;
 period = _period;
 initialWCET = _initialWCET;
 }
};
bool compareTasks(const Task& a, const Task& b) {
 return a.period < b.period;
}
struct TaskInt
{
 std::string name;
 int start;
 int end;
 bool hasStopped;
 TaskInt(const std::string& _name, int _start, int _end, bool _hasStopped)
 {
 name = _name;
 start = _start;
 end = _end;
 hasStopped = false;
 }
};
struct CPUDetails
{//Create struct of CPUDetails
public:
 std::string task;
 std::string* output;
 int CPUnum;
 CPUDetails() = default;
 //Constructor
 CPUDetails(const std::string& _task, std::string* _output, int _cpuName )
 {
 task = _task;
 output = _output;
 CPUnum = _cpuName;
 }
};
//HyperPeriod
int gcd(int a, int b)
{
 for (;;)
 {
 if (a == 0) return b;
 b %= a;
 if (b == 0) return a;
 a %= b;
 }
}
int lcm(int a, int b)
{
int temp = gcd(a, b)
int temp gcd(a, b);
 return temp ? (a / temp * b) : 0; //Inspired by:https://stackoverflow.com/questions/4229870/c-algorithm-to-calculate-least-common-mul
}
int hyperPeriod(const std::vector<Task>& temp)
{
 if (temp.empty())
 return 0;
 int result = temp[0].period;
 for (int i = 1; i < temp.size(); ++i)
 {
 result = lcm(result, temp[i].period);
 }
 return result;
}
double utilization(const std::vector<Task>& temp)
{
 if(temp.empty())
 return 0.0 ;
 double total = 0.0;
 for(int i = 0; i <temp.size(); i++)
 {
 total+= (double)temp[i].WCET / temp[i].period;
 }
 return total;
}
bool greater_than(const Task& m, const Task& n)
{
 return n < m;
}
std:: string solve(const std::string& in)
{
//instance Variables
std::stringstream input(in);
std::stringstream str;
 std::vector<Task> totalTask;
 std::string taskName;
 int wcet = 0;
 int period = 0;
 std::vector<TaskInt> taskInt;
 std::string temp;
 input >> temp;
 int cpuNum = stoi(temp);
 while(input >> taskName >> wcet >> period)
 {
 totalTask.emplace_back(taskName, wcet, period, wcet);
 }
 //str << totalTask.at(1).WCET;
 double utilCalc = utilization(totalTask);
 int hyperPeriodCalc = hyperPeriod(totalTask);
 std::priority_queue<Task, std::vector<Task>, decltype(&greater_than)> priQue(totalTask.begin(), totalTask.end(), &greater_than);
 //Formula = n * pow^(2/n) - 1
 double taskUnknown = priQue.size() * (pow(2, 1.0 / priQue.size()) - 1);
 if(cpuNum>1)
 {
 str << "\n\n";
 }
 str << "CPU " << cpuNum << std::endl << "Task scheduling information: ";
 for(int i = 0; i < totalTask.size(); i++)
 {
 str << totalTask[i].name << " (WCET: " << totalTask[i].WCET << ", Period: " << totalTask[i].period << ")";
 if(i != totalTask.size()-1)
 {
 str << ", ";
 }
 }
 str << std::endl << "Task set utilization: " << std::setprecision(2) << std::fixed << utilCalc << std::endl;
 str << "Hyperperiod: " << hyperPeriodCalc << std::endl;
 str << "Rate Monotonic Algorithm execution for CPU" << cpuNum << ":";
 if(utilCalc > 1)
 {
 str << std::endl << "The task set is not schedulable";
 return str.str();
 }
 else if(utilCalc > taskUnknown)
 {
 str << std::endl << "Task set schedulability is unknown";
 return str.str();
 }
 else
 str << std::endl << "Scheduling Diagram for CPU" << cpuNum << ": ";
 // Scheduling diagram
 std::sort(totalTask.begin(), totalTask.end(), compareTasks);

 for(int i = 0; i<hyperPeriodCalc; ++i)
 {
 bool isIdle = false;
 for(int k= 0; k<totalTask.size(); k++)
 {
 if(i % totalTask.at(k).period == 0)
 {
 totalTask.at(k).WCET = totalTask.at(k).initialWCET;
 }
 for(int j = 0; j<taskInt.size(); j++)
 {
 //Sets everything in taskInt to false
 taskInt.at(j).hasStopped = true;
 }
 }
 for(int k = 0; k<totalTask.size(); ++k)
 {
 if(totalTask.at(k).WCET > 0)
 {
 isIdle = true;
 int curWCET = totalTask.at(k).WCET;
 int curWCETInt = totalTask.at(k).initialWCET;
 if(curWCET == curWCETInt)
 {
 taskInt.push_back({totalTask.at(k).name, i, i+1, false });
 }
 else
 {
 bool stillRun = false;
 for(int j = 0; j<taskInt.size();j++)
 {
 if(taskInt.at(j).name == totalTask.at(k).name && !taskInt.at(j).hasStopped)
 {
 taskInt.at(j).end = i+1;
 stillRun = true;
 break;
 }
 }//end for
 if(stillRun == false)
 {
 taskInt.push_back({totalTask.at(k).name, i, i+1, false});
 }
 }//end else
 totalTask.at(k).WCET = totalTask.at(k).WCET - 1;
 break;
 }//end if
 }
 if(isIdle == false)
 {
 taskInt.push_back({"Idle", i, i+1, false });
 }
 }
 //deletes duplicates for idle
 for(int i = 0; i<taskInt.size()-1; i++)
 {
 if(taskInt.at(i).name == "Idle" && taskInt.at(i+1).name == "Idle")
 {
 taskInt.at(i).end = taskInt[i+1].end;
 taskInt.erase(taskInt.begin() + i + 1);
 --i;
 }

 }

 //deletes duplicates for name
 for(int i = 0; i<taskInt.size()-1; i++)
 {
 if(taskInt.at(i).name == taskInt.at(i+1).name )
 {
 taskInt.at(i).end = taskInt[i+1].end;
 taskInt.erase(taskInt.begin() + i + 1);
 --i;
 }
}
 std::string prev = " ";
 for(int i = 0; i<taskInt.size(); i++)
 {
 if(taskInt.at(i).name != "Idle")
 {
 int temp = taskInt.at(i).end - taskInt.at(i).start;
 str << taskInt.at(i).name << "(" << std::to_string(temp) << ")";
 prev = taskInt.at(i).name;
 }
 else
 {
 if(prev != "Idle")
 {
 int temp = taskInt.at(i).end - taskInt.at(i).start;
 str << "Idle(" << temp <<")";
 }
 }
 if(i != taskInt.size()-1)
 {
 str << ", ";
 }
 }
 return str.str();
}
void fireman(int)
{
 while (waitpid(-1, NULL, WNOHANG) > 0)
 ;
}
int main(int argc, char *argv[])
{
 int count = 0;
 int sockfd, newsockfd, portno, clilen;
 struct sockaddr_in serv_addr, cli_addr;
 // Check the commandline arguments
 if (argc != 2)
 {
 std::cerr << "Port not provided" << std::endl;
 exit(0);
 }
 // Create the socket
 sockfd = socket(AF_INET, SOCK_STREAM, 0);
 if (sockfd < 0)
 {
 std::cerr << "Error opening socket" << std::endl;
 exit(0);
 }
 // Populate the sockaddr_in structure
 bzero((char *)&serv_addr, sizeof(serv_addr));
 portno = atoi(argv[1]);
 serv_addr.sin_family = AF_INET;
 serv_addr.sin_addr.s_addr = INADDR_ANY;
 serv_addr.sin_port = htons(portno);
 // Bind the socket with the sockaddr_in structure
 if (bind(sockfd, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) < 0)
 {
 std::cerr << "Error binding" << std::endl;
 exit(0);
 }
 // Set the max number of concurrent connections
 listen(sockfd, 5);
 clilen = sizeof(cli_addr);
 signal(SIGCHLD, fireman);
 while(true){
 // Accept a new connection
 newsockfd = accept(sockfd, (struct sockaddr *)&cli_addr, (socklen_t *)&clilen);
 if(fork() == 0){

 if (newsockfd < 0)
 {
 std::cerr << "Error accepting new connections" << std::endl
 exit(0);
 }
 int n, msgSize = 0;
 n = read(newsockfd, &msgSize, sizeof(int));
 if (n < 0)
 {
 std::cerr << "Error reading from socket" << std::endl;
 exit(0);
 }
 char *tempBuffer = new char[msgSize + 1];
 bzero(tempBuffer, msgSize + 1);
 n = read(newsockfd, tempBuffer, msgSize + 1);
 if (n < 0)
 {
 std::cerr << "Error reading from socket" << " server side" << std::endl;
 exit(0);
 }
 std::string buffer = tempBuffer;
 delete[] tempBuffer;
 buffer = solve(buffer);
 count++;
 msgSize = buffer.size();
 n = write(newsockfd, &msgSize, sizeof(int));
 if (n < 0)
 {
 std::cerr << "Error writing to socket" << std::endl;
 exit(0);
 }
 n = write(newsockfd, buffer.c_str(), msgSize);
 if (n < 0)
 {
 std::cerr << "Error writing to socket" << std::endl;
 exit(0);
 }
}
}
 close(newsockfd);
 close(sockfd);
 return 0;
 }
