
#include <iostream>
#include <vector>
#include <algorithm> 
#include <set> 
int gcd(int a, int b) 
{ 
    if (a == 0) 
       return b; 
       
    if (b == 0) 
       return a; 
       
    if (a == b) 
        return a; 
  
    // a is greater 
    if (a > b) 
        return gcd(a-b, b); 
    return gcd(a, b-a); 
} 

int lcm(int a, int b)
{
	return a * b / gcd(a, b);
}
using namespace std;

int main()
{
    int n;
    cout << "Enter the number of tasks: ";
    cin >> n;
    cout << "Enter " << n << " tasks:\n";
    cout << "Time period, Release Time, Completion Time, Deadline:\n";
    //release time : arrival time
    //completion time : execution time
    vector<int> t(n), c(n), d(n), r(n);
    
    for (int i = 0; i < n; ++i) {
        cout << "Task " << i + 1 << ": ";
        cin >> t[i] >> r[i] >> c[i] >> d[i];
    }
    
    int hyper = 1;    
    for (int i = 0; i < n; ++i) {
        hyper = lcm(hyper, t[i]);
    }
    cout << "Hyperperiod: " << hyper << endl;
    
    vector<vector<int> > sched(hyper + 1);
    for (int i = 0; i < n; ++i) 
        for (int j = r[i]; j < hyper; j += t[i])
            sched[j].push_back(i);
    
    set<int> tasks;
    vector<int> rem(n);
    int cur = -1;    
    for (int time = 0; time < hyper; ++time) {
        for (int i: sched[time]) {
            rem[i] = c[i];
            tasks.insert(i);
        }
        cur = -1;
        for (int i: tasks)
            if (cur == -1 || d[i] < d[cur])
                cur = i;
        if (cur != -1) {
            rem[cur]--;
            if (rem[cur] == 0)
                tasks.erase(cur);
        }
        cout << "time = " << time << ": Task = " << cur + 1 << endl;
    }
    
    return 0;
}
