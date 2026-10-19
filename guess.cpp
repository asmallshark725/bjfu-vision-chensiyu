#include <iostream>
#include <random>
using namespace std;
int main(){
    random_device rd;
    mt19937 gen(rd());
    uniform_int_distribution<> dist(1, 100);
    int secret = dist(gen);
    int tries = 0;
    while(tries<7){
        int guess;
        cout <<"guess:";
        cin>>guess;
        tries++;
        if(guess>secret){
            cout<<"too big"<<endl;
        }else if(guess<secret){
            cout<<"too small"<<endl;
        }else{
            cout<<"correct"<<" "<<tries<<" "<<"tries"<<endl;
            break;
        }
    }
    return 0;
}