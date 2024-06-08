//CODE WAS inspired from prof Rincon Lectures/Boiler code
//Client
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
struct CPUDetails
{//Create struct of CPUDetails
public:
 std::string task;
 std::string* output;
 int CPUnum;
 char* serverIP;
char* portno;
 CPUDetails() = default;
 //Constructor
 CPUDetails(std::string& _task, std::string* _out,int _cpuName, char* _serverIP, char* _portno )
 {
 task = _task;
 output = _out;
 CPUnum = _cpuName;
 serverIP = _serverIP;
 portno = _portno;
 }
};
void *func(void * arg)
{
 CPUDetails* temp = (CPUDetails*) arg;

 int sockfd, portno, n;
 std::string buffer;
 struct sockaddr_in serv_addr;
 struct hostent *server;
 std::string* out = temp->output;
 portno = atoi(temp->portno);
 buffer = std::to_string(temp->CPUnum) + " " + temp->task;
 sockfd = socket(AF_INET, SOCK_STREAM, 0);
 if (sockfd < 0)
 {
 std::cerr << "ERROR opening socket" << std::endl;
 exit(0);
 }
 server = gethostbyname(temp->serverIP);
 if (server == NULL) {
 std::cerr << "ERROR, no such host" << std::endl;
 exit(0);
 }
 bzero((char *) &serv_addr, sizeof(serv_addr));
 serv_addr.sin_family = AF_INET;
 bcopy((char *)server->h_addr,
 (char *)&serv_addr.sin_addr.s_addr,
 server->h_length);
 serv_addr.sin_port = htons(portno);
 if (connect(sockfd,(struct sockaddr *)&serv_addr,sizeof(serv_addr)) < 0)
 {
 std::cerr << "ERROR connecting" << std::endl;
 exit(0);
 }

 int msgSize = sizeof(buffer);
n = write(sockfd,&msgSize,sizeof(int));
if (n < 0)
 {
 std::cerr << "ERROR writing to socket" << std::endl;
 exit(0);
 }
 n = write(sockfd,buffer.c_str(),msgSize);
 if (n < 0)
 {
 std::cerr << "ERROR writing to socket" << std::endl;
 exit(0);
 }
 n = read(sockfd,&msgSize,sizeof(int));
 if (n < 0)
 {
 std::cerr << "ERROR reading from socket" << std::endl;
 exit(0);
 }
 char *tempBuffer = new char[msgSize+1];
 bzero(tempBuffer,msgSize+1);
 n = read(sockfd,tempBuffer,msgSize);
 if (n < 0)
 {
 std::cerr << "ERROR reading from socket" << std::endl;
 exit(0);
 }
 buffer = tempBuffer;
 delete [] tempBuffer;

 close(sockfd);
 *out = buffer;
 return nullptr;

}
 int main(int argc, char *argv[]) {
 // // Check if there are enough command-line arguments
 if (argc < 3) {
 std::cerr << "Usage: " << argv[0] << " <serverIP> <portno>" << std::endl;
 return 1;
 }
 // Instantiate Variables
 std::vector<std::string> in = addStringToVec();
 std::vector<std::string> out(in.size());
 // std::vector<pthread_t> threads(in.size());
 std::vector<CPUDetails> obj;

 std::vector<pthread_t> threads(in.size());

 for(int i = 0; i < in.size(); ++i) {
 obj.emplace_back(in[i], &out[i], i + 1, argv[1], argv[2]);
 }
 //Pthread create -> Pthread Join
 for(int i = 0; i < in.size(); ++i) {

 pthread_create(&threads[i], nullptr, func, &obj[i]);
 }
 // Join threads
 for(int i = 0; i < in.size(); ++i) {
 pthread_join(threads[i], nullptr);
 }
// Output results
 for(int i = 0; i < out.size(); ++i) {
 std::cout << out.at(i);
 if(i < out.size()-1) {
 std::cout << "\n";
 }
 }
 }