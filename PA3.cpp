#include <iostream>
#include <vector>
#include <sstream>
#include <cmath>
#include <numeric>
#include <queue>
#include <pthread.h>
#include <queue>
#include <iomanip>
#include <algorithm>
std::vector<std::string> addStringToVec(){
 std::vector<std::string> inputsVector;
 std::string in;
 while(std::getline(std::cin, in)){
 if (!in.empty()){
 inputsVector.push_back(in);
 }
 }
 return inputsVector;
}
//References: https://gitgud.io/MadDawg/cosc3360_spring2024_guides/-/blob/master/assignment1guide.md //Inspired by:https://stackoverflow
struct Task
{
 std::string name;
 int WCET;
 int period;
 int initialWCET;
bool operator<(const Task& other) const{
 return(period <other.period) || (name < other.name && period == other.period);
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
};
struct CPUDetails
{//Create struct of CPUDetails
public:
 std::string task;
 std::string output;
 int CPUnum;
 pthread_mutex_t *bsem;
 pthread_mutex_t *bsemTwo;
 pthread_cond_t *condition;
 int *counter;
 std::string input = "";
 std::vector<TaskInt> taskInt;
 std::vector<Task> totalTask;
 int maxSize;
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
 int temp = gcd(a, b);
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
std:: string solve(const std::string& in, int cpuNum, std::vector<Task>& totalTask, std::vector<TaskInt>& taskInt)
{
//instance Variables
std::stringstream input(in);
std::stringstream str;
 std::string taskName;
 int wcet = 0;
 int period = 0;
 while(input >> taskName >> wcet >> period)
 {
 Task task;
 task.name = taskName;
 task.WCET = wcet;
 task.period = period;
 task.initialWCET = wcet;
 totalTask.push_back(task);
 }
 //str << totalTask.at(1).WCET;
 double utilCalc = utilization(totalTask);
 int hyperPeriodCalc = hyperPeriod(totalTask);
 std::priority_queue<Task, std::vector<Task>, decltype(&greater_than)> priQue(totalTask.begin(), totalTask.end(), &greater_than);
 //Formula = n * pow^(2/n) - 1
 double taskUnknown = priQue.size() * (pow(2, 1.0 / priQue.size()) - 1);

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
//Code for threading. Will call on another function to do everything
void *func(void * arg)
{
 CPUDetails temp = *(CPUDetails*) arg;
 std::string localInput = temp.input;
 int localID = temp.CPUnum-1;
 pthread_mutex_unlock(temp.bsem);
 std::string temper = solve(temp.input, temp.CPUnum, temp.totalTask, temp.taskInt);
 pthread_mutex_lock(temp.bsemTwo);
 while(*temp.counter!=localID)
 {
 pthread_cond_wait(temp.condition, temp.bsemTwo);
 }
 pthread_mutex_unlock(temp.bsemTwo );
 if(localID+1!=temp.maxSize)
 std::cout << temper << std::endl << std::endl << std::endl;
 else
 std::cout << temper;
 pthread_mutex_lock(temp.bsemTwo);
 (*temp.counter)++;
 pthread_cond_broadcast((temp.condition));
 pthread_mutex_unlock(temp.bsemTwo);
 return nullptr;
}
int main()
{
 //std::vector<pthread_t> threads(in.size());
 const std::vector<std::string> in = addStringToVec();
 CPUDetails tempThread;
 pthread_mutex_t bsem;
 pthread_mutex_t bsemTwo;
 pthread_mutex_init(&bsem,nullptr);
 pthread_mutex_init(&bsemTwo,nullptr);
 pthread_cond_t condition = PTHREAD_COND_INITIALIZER;
 static int counter = 0;
 tempThread.bsem = &bsem;
 tempThread.bsemTwo = &bsemTwo;
 tempThread.condition = &condition;
 tempThread.counter = &counter;
 std::vector<pthread_t> threads;
 //Pthread create -> Pthread Join
 for(int i = 0; i<in.size(); ++i)
 {
 pthread_t newThread;
 pthread_mutex_lock(&bsem);
 tempThread.input = in[i];
 tempThread.CPUnum = i+1;
 tempThread.maxSize = in.size();
 if (pthread_create(&newThread, NULL, func, &tempThread))
 {
 fprintf(stderr, "Error creating thread\n");
 return 1;
 }
 threads.push_back(newThread);
 }
 //Join
 for(int i = 0; i<in.size(); ++i)
 {
 pthread_join(threads[i], NULL);
 }
}