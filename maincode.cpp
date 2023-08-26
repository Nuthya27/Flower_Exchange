#include <iostream>
#include <string>
#include <vector>
#include <fstream>
#include <sstream>
using namespace std;

struct Order {
    string client_order_id;
    string instrument;
    int side;
    int quantity;
    double price;
    int order_id;
};

struct ExecutionReport {
    string order_id;
    string client_order_id;
    string instrument;
    int side;
    string execution_status;
    int quantity;
    double price;
    string reason;
};


void insSortAsc(vector<Order>& arr)
{
    int n = arr.size();
    int i, j;
    double key;
    Order keyOrder;
    for (i = 1; i < n; i++) {
        key = arr[i].price;
        keyOrder = arr[i];
        j = i - 1;
        while (j >= 0 && arr[j].price > key) {
            arr[j + 1] = arr[j];
            j = j - 1;
        }
        arr[j + 1] = keyOrder;
    }
}


void insSortDesc(vector<Order>& arr)
{
    int n = arr.size();
    int i, j;
    double key;
    Order keyOrder;
    for (i = 1; i < n; i++) {
        key = arr[i].price;
        keyOrder = arr[i];
        j = i - 1;
        while (j >= 0 && arr[j].price < key) {
            arr[j + 1] = arr[j];
            j = j - 1;
        }
        arr[j + 1] = keyOrder;
    }
}

string reject(Order& orderLine, string reason){
    
}


int main()
{
    string file_name;
    file_name = "order.csv";
    ifstream file(file_name);
    if (!file.is_open()) {
        cerr << "Error opening file " << file_name << endl;

    }

    string line;
    
    vector<Order> sell_orders;
    vector<Order> buy_orders;
    vector<ExecutionReport> excreport;

    cout << "Reading file " << file_name << endl;

    getline(file, line);
    
    int ID=1;
    int j=1;
    while (getline(file, line)) {
        //cout<<i--<<endl;
        //cout<< line<<"\n";
        Order order;
        stringstream ss(line);
        string token;
        getline(ss, token, ',');
        order.client_order_id = token;
        getline(ss, token, ',');
        order.instrument = token;
        getline(ss, token, ',');
        order.side = stoi(token);
        getline(ss, token, ',');
        order.quantity = stoi(token);
        getline(ss, token, ',');
        order.price = stod(token);
        order.order_id = ID;
        if (order.side == 1) {
            buy_orders.push_back(order);
            insSortDesc(buy_orders);
        }
        else if (order.side == 2) {
            sell_orders.push_back(order);
            insSortAsc(sell_orders);
        }
        
        j--;
       // cout<< order.client_order_id<<endl;

        if (buy_orders.empty() || sell_orders.empty() || (buy_orders.front().price<sell_orders.front().price)){
            //no matching needed
            ExecutionReport newReport;
            newReport.order_id = ID;
            newReport.client_order_id = order.client_order_id ;
            newReport.instrument =order.instrument;
            newReport.side =order.side;
            newReport.execution_status = "New";
            newReport.quantity = order.quantity;
            newReport.price =order.price;
            newReport.reason = "djsahemk";
            excreport.push_back(newReport);
        }


        //matching
        //conditions for matching happen
        while ((!buy_orders.empty()) && (!sell_orders.empty())){
           // cout<<"1"<<endl;
            if (buy_orders.front().price>=sell_orders.front().price){
               // cout<<"2"<<endl;
                //matching should happen
                ExecutionReport BuyReport;
                BuyReport.order_id = buy_orders.front().order_id;
                BuyReport.client_order_id = buy_orders.front().client_order_id ;
                BuyReport.instrument =buy_orders.front().instrument;
                BuyReport.side =buy_orders.front().side;
                BuyReport.price =buy_orders.front().price;
                BuyReport.reason = "djsahemk";  

                ExecutionReport SellReport;
                SellReport.order_id = sell_orders.front().order_id;
                SellReport.client_order_id = sell_orders.front().client_order_id ;
                SellReport.instrument =sell_orders.front().instrument;
                SellReport.side =sell_orders.front().side;
                SellReport.price =sell_orders.front().price;
                SellReport.reason = "djsahemk"; 
                

                if (buy_orders.front().quantity>sell_orders.front().quantity){
                   // cout<<"3"<<endl;
                    //for buy order
                    BuyReport.execution_status = "PFill";
                    BuyReport.quantity = buy_orders.front().quantity - sell_orders.front().quantity;
                    buy_orders.front().quantity=buy_orders.front().quantity - sell_orders.front().quantity;

                    //for sell order
                    SellReport.execution_status = "Fill";
                    SellReport.quantity = sell_orders.front().quantity;
                    sell_orders.erase(sell_orders.begin());
                                      
                }
                else if (sell_orders.front().quantity>buy_orders.front().quantity){
                   // cout<<"4"<<endl;
                    //for buy order
                    BuyReport.execution_status = "Fill";
                    BuyReport.quantity = buy_orders.front().quantity; 
                    buy_orders.erase(buy_orders.begin());

                    //for sell order
                    SellReport.execution_status = "PFill";
                    SellReport.quantity = sell_orders.front().quantity -buy_orders.front().quantity;
                    sell_orders.front().quantity=sell_orders.front().quantity -buy_orders.front().quantity;               
                }
                else {
                   // cout<<"5"<<endl;
                    //for buy order
                    BuyReport.execution_status = "Fill";
                    BuyReport.quantity = buy_orders.front().quantity;
                    buy_orders.erase(buy_orders.begin()); 

                    //for sell order
                    SellReport.execution_status = "Fill";
                    SellReport.quantity = sell_orders.front().quantity; 
                    sell_orders.erase(sell_orders.begin());                  
                }

                excreport.push_back(BuyReport);
                excreport.push_back(SellReport);

            }
            else {
                break;
            }
        }

        ID++;
    }

    file.close();

    if (buy_orders.size() == 0) {
        cout << "No buy orders" << endl;
    }
    else {
        cout << "Buy orders:" << endl;
        for (int i = 0; i < buy_orders.size(); i++)
        {
            cout << buy_orders[i].client_order_id << '\n';
        }
    }

    if (sell_orders.size() == 0) {
        cout << "No sell orders" << endl;
    }
    else {
        cout << "Sell orders:" << endl;
        for (int i = 0; i < sell_orders.size(); i++)
        {
            cout << sell_orders[i].client_order_id << '\n';
        }
    }
    
    cout << "Report" << endl;
    for (int i = 0; i < excreport.size(); i++){
        cout << excreport[i].client_order_id << " "<<excreport[i].quantity<<" " <<excreport[i].execution_status<< '\n';
    }
    return 0;
}



