#include <iostream>
#include <string>
#include <vector>
#include <fstream>
#include <sstream>
#include <algorithm>
#include <iomanip>
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

void output_csv(vector<ExecutionReport>& excreport, string file_name){
    ofstream file(file_name);
    if (!file.is_open()) {
        cerr << "Error opening file " << file_name << endl;
    }
    file << "order_id,client_order_id,instrument,side,execution_status,quantity,price,reason" << endl;
    for (const auto& report : excreport) {
        if (report.reason == "Missing side"){
            file << report.order_id << "," << report.client_order_id << ","<< report.instrument<<","  << "," << report.execution_status << "," << report.quantity << "," <<fixed<<setprecision(2)<< report.price << "," << report.reason << endl;
        }
        else if (report.reason == "Missing quantity"){
            file << report.order_id << "," << report.client_order_id << "," << report.instrument << "," << report.side << "," << report.execution_status << "," << "," <<fixed<<setprecision(2)<<report.price << "," << report.reason << endl;
        }
        else if (report.reason == "Missing price"){
            file << report.order_id << "," << report.client_order_id << "," << report.instrument << "," << report.side << "," << report.execution_status << "," << report.quantity << "," << "," << report.reason << endl;
        }
        else{
            file << report.order_id << "," << report.client_order_id << "," << report.instrument << "," << report.side << "," << report.execution_status << "," << report.quantity << ","  << fixed << setprecision(2)<< report.price << "," << report.reason << endl;
        }
    }

    file.close();
}


void order_book_handling(vector<Order>& buy_orders, vector<Order>& sell_orders, vector<ExecutionReport>& excreport, Order& order, int& ID){
    if (buy_orders.empty() || sell_orders.empty() || (buy_orders.front().price<sell_orders.front().price)){
        //no matching needed
        ExecutionReport newReport;
        newReport.order_id = "ord" + to_string(ID);
        newReport.client_order_id = order.client_order_id ;
        newReport.instrument =order.instrument;
        newReport.side =order.side;
        newReport.execution_status = "New";
        newReport.quantity = order.quantity;
        newReport.price =order.price;
        newReport.reason = "";
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
            BuyReport.order_id = "ord" + to_string(buy_orders.front().order_id);
            BuyReport.client_order_id = buy_orders.front().client_order_id ;
            BuyReport.instrument =buy_orders.front().instrument;
            BuyReport.side =buy_orders.front().side;
            BuyReport.price =buy_orders.front().price;
            BuyReport.reason = "";  

            ExecutionReport SellReport;
            SellReport.order_id = "ord" + to_string(sell_orders.front().order_id);
            SellReport.client_order_id = sell_orders.front().client_order_id ;
            SellReport.instrument =sell_orders.front().instrument;
            SellReport.side =sell_orders.front().side;
            SellReport.price =sell_orders.front().price;
            SellReport.reason = ""; 
            

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
}



string reason(Order& orderLine){

    if (orderLine.instrument != "Rose" && orderLine.instrument != "Lavender" && orderLine.instrument != "Lotus" && orderLine.instrument != "Tulip" && orderLine.instrument != "Orchid"   ){
        return "Invalid instrument";
    }

    if (orderLine.side != 1 && orderLine.side != 2){
        return "Invalid side";
    }

    if (orderLine.quantity < 10 || orderLine.quantity > 1000  || orderLine.quantity % 10 != 0){
        return "Invalid quantity";
    }

    if (orderLine.price <= 0){
        return "Invalid price";
    }
    
    return "Valid";
}

void rejection(vector<ExecutionReport>& excreport,Order& order, string reason, int& ID){
    ExecutionReport newReport;
    newReport.order_id = "ord" + to_string(ID);
    newReport.client_order_id = order.client_order_id ;
    newReport.instrument =order.instrument;
    newReport.side =order.side;
    newReport.execution_status = "Rejected";
    newReport.quantity = order.quantity;
    newReport.price =order.price;
    newReport.reason = reason;
    excreport.push_back(newReport);
    ID++;

    //output_csv(excreport, "execution_rep.csv");
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
    
    vector<Order> rose_sell_orders;
    vector<Order> rose_buy_orders;
    vector<Order> lavender_sell_orders;
    vector<Order> lavender_buy_orders;
    vector<Order> lotus_sell_orders;
    vector<Order> lotus_buy_orders;
    vector<Order> tulip_sell_orders;
    vector<Order> tulip_buy_orders;
    vector<Order> orchid_sell_orders;
    vector<Order> orchid_buy_orders;

    vector<ExecutionReport> excreport;
    

    cout << "Reading file " << file_name << endl;

    getline(file, line);
    string reject_reason;
    int ID=1;
    int j=1;
    bool missing_field;
    while (getline(file, line)) {

        //cout<<ID<<endl;
        //cout<< line<<"\n";
        Order order;
        stringstream ss(line);
        string token;
        
        getline(ss, token, ',');
        if (token == ""){
            reject_reason = "Missing client order id";
            order.client_order_id = "";
            missing_field = true;  
        }
        else{
            order.client_order_id = token;
        }
        getline(ss, token, ',');
        if (token == ""){
            reject_reason = "Missing instrument";
            order.instrument="";
            missing_field = true;
        }
        else{
            order.instrument = token;
        }
        getline(ss, token, ',');
        if (token == ""){
            reject_reason = "Missing side";
            order.side = 0;
            missing_field = true;
        }
        else{
            order.side = stoi(token);
        }
        getline(ss, token, ',');
        if (token == ""){
            reject_reason = "Missing quantity";
            order.quantity=0;
            missing_field = true;
        }
        else{
            order.quantity = stoi(token);
        }
        getline(ss, token, ',');
        if (token == ""){
            reject_reason = "Missing price";
            order.price = 0;
            missing_field = true;
        }
        else{
            order.price = stod(token);
        }
        order.order_id =ID;

        if (missing_field){
            rejection(excreport, order, reject_reason, ID);
            missing_field = false;
            continue;
        }

        reject_reason = reason(order);
        
        if (reject_reason != "Valid"){
            cout << "Reject " << order.client_order_id << " " << reject_reason << endl;
            rejection(excreport, order, reject_reason, ID);
            continue;
        }
        
        if (order.side == 1) {
            if (order.instrument == "Rose"){
                rose_buy_orders.push_back(order);
                insSortDesc(rose_buy_orders);
            }
            else if (order.instrument == "Lavender"){
                lavender_buy_orders.push_back(order);
                insSortDesc(lavender_buy_orders);
            }
            else if (order.instrument == "Lotus"){
                lotus_buy_orders.push_back(order);
                insSortDesc(lotus_buy_orders);
            }
            else if (order.instrument == "Tulip"){
                tulip_buy_orders.push_back(order);
                insSortDesc(tulip_buy_orders);
            }
            else if (order.instrument == "Orchid"){
                orchid_buy_orders.push_back(order);
                insSortDesc(orchid_buy_orders);
            }
        }
        else if (order.side == 2) {
            if (order.instrument == "Rose"){
                rose_sell_orders.push_back(order);
                insSortAsc(rose_sell_orders);
                for (int i = 0; i < rose_sell_orders.size(); i++)
                {
                    cout << rose_sell_orders[i].client_order_id << '\n';
                }
            }
            else if (order.instrument == "Lavender"){
                lavender_sell_orders.push_back(order);
                insSortAsc(lavender_sell_orders);
            }
            else if (order.instrument == "Lotus"){
                lotus_sell_orders.push_back(order);
                insSortAsc(lotus_sell_orders);
            }
            else if (order.instrument == "Tulip"){
                tulip_sell_orders.push_back(order);
                insSortAsc(tulip_sell_orders);
            }
            else if (order.instrument == "Orchid"){
                orchid_sell_orders.push_back(order);
                insSortAsc(orchid_sell_orders);
            }
        }
        
        j--;
       // cout<< order.client_order_id<<endl;

        if(order.instrument == "Rose"){
            order_book_handling(rose_buy_orders, rose_sell_orders, excreport, order, ID);
        }
        else if(order.instrument == "Lavender"){
            order_book_handling(lavender_buy_orders, lavender_sell_orders, excreport, order, ID);
        }
        else if(order.instrument == "Lotus"){
            order_book_handling(lotus_buy_orders, lotus_sell_orders, excreport, order, ID);
        }
        else if(order.instrument == "Tulip"){
            order_book_handling(tulip_buy_orders, tulip_sell_orders, excreport, order, ID);
        }
        else if(order.instrument == "Orchid"){
            order_book_handling(orchid_buy_orders, orchid_sell_orders, excreport, order, ID);
        }

        ID++;
        
    }

    file.close();

    output_csv(excreport, "execution_rep.csv");




    if (rose_buy_orders.size() == 0) {
        cout << "No rose buy orders" << endl;
    }
    else {
        cout << "Buy rose orders:" << endl;
        for (int i = 0; i < rose_buy_orders.size(); i++)
        {
            cout << rose_buy_orders[i].client_order_id << '\n';
        }
    }

    if (lavender_buy_orders.size() == 0) {
        cout << "No lavender buy orders" << endl;
    }
    else {
        cout << "Buy lavender orders:" << endl;
        for (int i = 0; i < lavender_buy_orders.size(); i++)
        {
            cout << lavender_buy_orders[i].client_order_id << '\n';
        }
    }

    if (lotus_buy_orders.size() == 0) {
        cout << "No lotus buy orders" << endl;
    }
    else {
        cout << "Buy lotus orders:" << endl;
        for (int i = 0; i < lotus_buy_orders.size(); i++)
        {
            cout << lotus_buy_orders[i].client_order_id << '\n';
        }
    }

    if (rose_sell_orders.size() == 0) {
        cout << "No sell orders" << endl;
    }
    else {
        cout << "Sell orders:" << endl;
        for (int i = 0; i < rose_sell_orders.size(); i++)
        {
            cout << rose_sell_orders[i].client_order_id << '\n';
        }
    }
    
    cout << "Report" << endl;
    for (int i = 0; i < excreport.size(); i++){
        cout << excreport[i].order_id<< " "<< excreport[i].client_order_id << " "<<excreport[i].quantity<<" " <<excreport[i].execution_status<<" " <<fixed<<setprecision(2)<<excreport[i].price<< '\n';
    }
    return 0;
}



